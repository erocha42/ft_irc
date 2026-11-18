#include "Server.hpp"

volatile sig_atomic_t	flag = 0;

Server::Server() : _fdl(-1), _port(-1), _pollfd_list(), _all_clients(), _all_channels(), _password("Default") {	
}

Server::Server(int port, std::string password) : _fdl(-1), _pollfd_list(), _all_clients(), _all_channels() {
	_port = port;
	_password = password;
}

Server::~Server() {
	std::map<int, Client *>::iterator it;
	std::map<std::string, Channel*>::iterator it2;
	if (_fdl != -1)
		close(_fdl);
	for (it = _all_clients.begin(); it != _all_clients.end(); it++) {
		close(it->second->getFd());
        delete it->second;
    }
	for (it2 = _all_channels.begin(); it2 != _all_channels.end(); it2++) {   
        delete it2->second;
    }
    _all_clients.clear();
	_all_channels.clear();
}

bool	Server::fdl_init()
{
	_fdl = socket(AF_INET, SOCK_STREAM, 0);
	if (_fdl == -1)
	{
		perror("socket");
		return false;
	}
	int one = 1;
	if (setsockopt(_fdl, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1)
	{
		perror("setsockopt");
		return false;
	}
	if (fcntl(_fdl, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return false;
	}
	sockaddr_in addr;
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_fdl, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
	{
		perror("bind");
		return false;
	}
	if (listen(_fdl, SOMAXCONN) == -1)
	{
		perror("listen");
		return false;
	}
	return true;
}

void Server::addClient(Client* c) {
    if (c)
        _all_clients[c->getFd()] = c;
}

void	Server::newClientHandling()
{
	pollfd newPoll;
	memset(&newPoll, 0, sizeof(newPoll));
	newPoll.events = POLLIN;
	sockaddr_in cli;
	socklen_t cli_len = sizeof(cli);
	if ((newPoll.fd = accept(_fdl, 
		reinterpret_cast<struct sockaddr*>(&cli), &cli_len)) == -1)
	{
		perror("accept");
		return ;
	}
	if (fcntl(newPoll.fd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		close(newPoll.fd);
		return ;
	}
	_pollfd_list.push_back(newPoll);
	Client *newClient = new Client(newPoll.fd);
	addClient(newClient);
}

void	Server::removeClient(int fd)
{
	Client *client;
	std::map<int, Client *>::iterator it = _all_clients.find(fd);
	if (it == _all_clients.end())
		return ;
	client = it->second;
	close(fd);
	for (size_t i = 0; i < _pollfd_list.size(); i++)
	{
		if (_pollfd_list[i].fd == fd)
		{
			_pollfd_list.erase(_pollfd_list.begin() + i);
			break;
		}
	}
	for (std::map<std::string, Channel*>::iterator itc = _all_channels.begin(); itc != _all_channels.end();) {
		itc->second->removeClient(client);
		if (itc->second->sizeUser() == 0) {
			std::map<std::string, Channel*>::iterator temp_it = itc;
			itc++;
			delete temp_it->second;
			_all_channels.erase(temp_it->first);
		}
		else
			itc++;
	}
	_all_clients.erase(fd);
	delete client;
}

void	Server::sendM(Client *cli, std::string const& mess)
{
	cli->appendBuffer(mess, OUT);
	for (size_t i = 0; i < _pollfd_list.size(); i++)
	{
		if (cli->getFd() == _pollfd_list[i].fd)
			_pollfd_list[i].events |= POLLOUT;
	}
}

void	Server::connection_loop()
{
	pollfd fdl_;
	memset(&fdl_, 0, sizeof(fdl_));
	fdl_.events = POLLIN;
	fdl_.fd = _fdl;
	_pollfd_list.push_back(fdl_);
	while (!flag)
	{
		if (poll(&_pollfd_list[0], _pollfd_list.size(), -1) == -1)
			return ;
		for (size_t i = 0; i < _pollfd_list.size(); i++)
		{
			if (_pollfd_list[i].revents & POLLIN)
			{
				if (_pollfd_list[i].fd == _fdl)
					newClientHandling();
				else
				{
					char buff[512];
					Client *current_cli = _all_clients.find(_pollfd_list[i].fd)->second;
					ssize_t n = recv(_pollfd_list[i].fd, buff, sizeof(buff), 0);
					if (n > 0)
					{
						current_cli->appendBuffer(std::string(buff, n), IN);
						std::string &buff_cli = current_cli->getBuffer(IN);
						size_t pos = buff_cli.find("\n", 0);
						bool	cli_exist = true;
						while (pos != std::string::npos)
						{
							std::string line = buff_cli.substr(0, pos);
							Message mess(line);
							int temp_fd = _pollfd_list[i].fd;
							executeCommand(mess, current_cli);
							if (_all_clients.find(temp_fd) == _all_clients.end())
							{
								i--;
								cli_exist = false;
								break;
							}
							buff_cli.erase(0, pos + 1);
							pos = buff_cli.find("\n", 0);
						}
						if (cli_exist && buff_cli.size() > 512)
							buff_cli.clear();
						if (!cli_exist)
							continue;
					}
					else
					{
						removeClient(_pollfd_list[i].fd);
						i--;
						continue;
					}
				}
			}
			if (_pollfd_list[i].revents & POLLOUT)
			{
				Client *current_cli = _all_clients.find(_pollfd_list[i].fd)->second;
				std::string &buff_cli = current_cli->getBuffer(OUT);
				ssize_t send_result = send(_pollfd_list[i].fd, buff_cli.c_str(), buff_cli.size(), MSG_NOSIGNAL);
				if (send_result > 0)
				{
					buff_cli.erase(0, send_result);
					if (buff_cli.empty())
						_pollfd_list[i].events &= ~POLLOUT;
				}
				else if (send_result < 0)
				{
					removeClient(_pollfd_list[i].fd);
					i--;
					continue;
				}

			}
			if (_pollfd_list[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				removeClient(_pollfd_list[i].fd);
				i--;
				continue;
			}
		}
	}
}

void Server::executeCommand(const Message& m, Client* c) {
	if (m.getCmd() == "PASS")
		cmdPass(m, c);
	else if (m.getCmd() == "NICK")
		cmdNick(m, c);
	else if (m.getCmd() == "USER")
		cmdUser(m, c);
	else if (m.getCmd() == "PRIVMSG") {
		if (c->getIsSignup())
			cmdPrivmsg(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "JOIN"){
		if (c->getIsSignup())
			cmdJoin(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "PART") {
		if (c->getIsSignup())
			cmdPart(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "QUIT")
		cmdQuit(m, c);
	else if (m.getCmd() == "KICK") {
		if (c->getIsSignup())
			cmdKick(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "INVITE") {
		if (c->getIsSignup())
			cmdInvite(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "TOPIC") {
		if (c->getIsSignup())
			cmdTopic(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "MODE") {
		if (c->getIsSignup())
			cmdMode(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "PING") {
		if (c->getIsSignup())
			cmdPing(m, c);
		else {
			sendReply(m, c, 451);
			return;
		}
	}
	else if (m.getCmd() == "CAP") 
		return;
	else if (!m.getCmd().empty()) {
		sendReply(m, c, 421);
		return;
	}
}

void Server::cmdPing(const Message& m, Client* c) {
	if (m.getSettings().size() == 0) {
		sendReply(m, c, 409);
		return;
	}
	std::string msg = ":Best_Server PONG Best_Server :" + m.getSettings()[0] + "\r\n";
	sendM(c, msg);
	return;
}

void Server::cmdPass(const Message& m, Client* c) {
	if (c->getIsSignup() == true) {
		sendReply(m, c,462);
		return;
	}
	if (m.getSettings().size() < 1) {
		sendReply(m, c,461);
		return;
	}
	if (m.getSettings()[0] != _password) {
		sendReply(m, c,464);
		return;
	}
	else
		c->setPasswordCheck(true);
	checkRegistration(m, c);
}

void Server::cmdNick(const Message& m, Client* c) {
	if (!c->getPasswordcheck()) {
		sendReply(m, c,464);
		return;
	}
	if (m.getSettings().size() < 1 || m.getSettings()[0].empty()) {
		sendReply(m, c,431);
		return;
	}
	if (!isalpha(m.getSettings()[0][0]) && std::strchr("[]\\'\"^_-{|}", m.getSettings()[0][0]) == NULL) {
		sendReply(m, c,432);
		return;
	}
	for (size_t i = 1; i < m.getSettings()[0].length(); i++) {
		if (!isalnum(m.getSettings()[0][i]) && std::strchr("[]\\'\"^_-{|}", m.getSettings()[0][i]) == NULL ){
			sendReply(m, c,432);
			return;
		}
	}
	if (c->getNickname() == m.getSettings()[0]) {
        return;
	}
	for (std::map<int, Client*>::iterator it = _all_clients.begin(); it != _all_clients.end(); it++ ) {
		if (it->second->getNickname() == m.getSettings()[0]){
			sendReply(m, c,433);
			return;
		}
	}
	if (c->getIsSignup()) {
		std::string msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 NICK :" + m.getSettings()[0] + "\r\n";
		for (std::map<int, Client*>::iterator it = _all_clients.begin(); it != _all_clients.end(); it++) {
            if (it->second->getIsSignup()) {
                sendM(it->second, msg);
            }
        }
	}
	c->setNickname(m.getSettings()[0]);
	checkRegistration(m, c);
}

void Server::cmdUser(const Message& m, Client* c) {
	if (!c->getPasswordcheck()) {
		sendReply(m, c,464);
		return;
	}
	if (c->getIsSignup() == true) {
		sendReply(m, c,462);
		return;
	}
	if (m.getSettings().size() != 4|| m.getSettings()[0].empty()) {
		sendReply(m, c,461);
		return;
	}
	c->setUsername(m.getSettings()[0]);
	c->setRealname(m.getSettings()[3]);
	checkRegistration(m, c);
}

void Server::cmdPrivmsg(const Message& m, Client* c) {
	if (m.getSettings().size() == 0) {
		sendReply(m, c,411);
		return;
	}
	if (m.getSettings().size() == 1 || m.getSettings()[1].empty()) {
		sendReply(m, c,412);
		return;
	}
	if (m.getSettings()[0][0] == '#' || m.getSettings()[0][0] == '&') {
		std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
		if (it != _all_channels.end()) {
			if (!it->second->IsInChannel(c)) {
				sendReply(m, c, 404);
				return;
			}
			std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 PRIVMSG " + m.getSettings()[0] + " :" + m.getSettings()[1] +"\r\n";
			std::vector<Client*> participants = it->second->getUsers();
			for (size_t i = 0; i < participants.size(); i++) {
				if (participants[i] != c)
					sendM(participants[i], msg);
			}
			return;
		}
	}
	else {
		for (std::map<int, Client*>::iterator it = _all_clients.begin(); it != _all_clients.end(); it++ ){
			if (it->second->getNickname() == m.getSettings()[0]){
				std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 PRIVMSG " + m.getSettings()[0] + " :" + m.getSettings()[1] +"\r\n";
				sendM(it->second, msg);
				return;
			}
		}
	}
	sendReply(m, c, 401);
	return;
}

void Server::cmdJoin(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(m, c,461);
		return;
	}
	if ((m.getSettings()[0][0] != '#' && m.getSettings()[0][0] != '&') || m.getSettings()[0].length() < 2) {
		sendReply(m, c, 403);
		return;
	}
	Channel* target_channel;
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) {
		Channel* new_channel = new Channel(m.getSettings()[0]);
		target_channel = new_channel;
		new_channel->addInChannel(c);
		new_channel->addOp(c);
		_all_channels[m.getSettings()[0]] = new_channel;
	}
	else {
		target_channel = it->second;
		if (target_channel->IsInChannel(c))
			return;
		if (target_channel->getInviteOnly() && !target_channel->IsInvited(c)) {
			sendReply(m, c, 473);
			return;
		}
		if (target_channel->getPassword() != "") {
			if (m.getSettings().size() < 2 || m.getSettings()[1] != target_channel->getPassword()) {
				sendReply(m, c, 475);
				return;
			}
		}
		if (target_channel->getUserLimit() > 0 && target_channel->getUsers().size() >= target_channel->getUserLimit()) {
			sendReply(m, c,471);
			return;
		}
		target_channel->addInChannel(c);
		target_channel->removeInv(c);
	}
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 JOIN :" + m.getSettings()[0] +"\r\n";
	std::vector<Client*> participants = target_channel->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		sendM(participants[i], msg);
	}
	std::string list_name = "";
	for (size_t i = 0; i < participants.size(); i++){
		if (target_channel->IsOp(participants[i]))
			list_name += "@" + participants[i]->getNickname() + " ";
		else
			list_name += participants[i]->getNickname() + " ";
	}
	if (target_channel->getTopic() != "") {
    	std::string msg332 = ":Best_Server 332 " + c->getNickname() + " " + m.getSettings()[0] + " :" + target_channel->getTopic() + "\r\n";
    	sendM(c, msg332);
	}
	std::string msg353 = ":Best_Server 353 " + c->getNickname() + " = " + m.getSettings()[0] + " :" + list_name + "\r\n";
	sendM(c, msg353);
	std::string endofname = ":Best_Server 366 " + c->getNickname() + " " + m.getSettings()[0] + " :End of /NAMES list.\r\n";
	sendM(c, endofname);
}

void Server::cmdPart(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(m, c, 461);
		return;
	}
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) {
		sendReply(m, c, 403);
		return;
	}
	if (!it->second->IsInChannel(c)) {
		sendReply(m, c, 442);
		return;
	}
	std::string msg;
	if (m.getSettings().size() >= 2)
		msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 PART " + m.getSettings()[0] + " :" + m.getSettings()[1] + "\r\n";
	else
		msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 PART " + m.getSettings()[0] + " :Bye\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		sendM(participants[i], msg);
	}
	it->second->removeClient(c);
	if (it->second->sizeUser() == 0) {
		delete it->second;
		_all_channels.erase(m.getSettings()[0]);
	}
}

void Server::cmdQuit(const Message& m, Client* c) {
	for (std::map<std::string, Channel*>::iterator it = _all_channels.begin(); it != _all_channels.end();) {
		if (it->second->IsInChannel(c)) {
			std::string msg;
			if (m.getSettings().size() == 1)
				msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 QUIT " + ":"+ m.getSettings()[0] + "\r\n";
			else
				msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 QUIT " + ":Quit\r\n";
			it->second->removeClient(c);
			std::vector<Client*> participants = it->second->getUsers();
			for (size_t i = 0; i < participants.size(); i++)
				sendM(participants[i], msg);
			std::map<std::string, Channel*>::iterator temp_it;
			if (it->second->sizeUser() == 0) {
				temp_it = it;
				it++;
				delete temp_it->second;
				_all_channels.erase(temp_it->first);
			}
			else
				it++;
		}
		else
			it++;
	}
	removeClient(c->getFd());
}

void Server::cmdKick(const Message& m, Client* c) {
	if (m.getSettings().size() < 2) {
		sendReply(m, c, 461);
		return;
	}
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) {
		sendReply(m, c, 403);
		return;
	}
	if (!it->second->IsInChannel(c)) {
		sendReply(m, c, 442);
		return;
	}
	if (!it->second->IsOp(c)) {
		sendReply(m, c, 482);
		return;
	}
	if (!it->second->IsInChannelStr(m.getSettings()[1])) {
		sendReply(m, c, 441);
		return;
	}
	std::string msg;
	if (m.getSettings().size() >= 3)
		msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 KICK " + m.getSettings()[0] + " " + m.getSettings()[1] + " :" + m.getSettings()[2] + "\r\n";
	else
		msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 KICK " + m.getSettings()[0] + " " + m.getSettings()[1] + " :Reason\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		sendM(participants[i], msg);
	}
	Client* target = NULL;
	for (size_t i = 0; i < participants.size(); i++) {
		if (participants[i]->getNickname() == m.getSettings()[1]) {
			target = participants[i];
			break;
		}
	}
	it->second->removeClient(target);
	if (it->second->sizeUser() == 0) {
		delete it->second;
		_all_channels.erase(m.getSettings()[0]);
	}
}

void Server::cmdInvite(const Message& m, Client* c) {
	if (m.getSettings().size() < 2) {
		sendReply(m, c,461);
		return;
	}
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[1]);
	if (it == _all_channels.end()) {
		sendReply(m, c, 403);
		return;
	}
	if (!it->second->IsInChannel(c)) {
		sendReply(m, c, 442);
		return;
	}
	int bin = 0;
	Client* target = NULL;
	for (std::map<int, Client*>::iterator it2 = _all_clients.begin(); it2 != _all_clients.end(); it2++ ) {
		if (it2->second->getNickname() == m.getSettings()[0]){
			bin = 1;
			target = it2->second;
			break;
		}
	}
	if (bin == 0) {
		sendReply(m, c, 401);
		return;
	}
	if (it->second->IsInChannelStr(m.getSettings()[0])) {
		sendReply(m, c, 443);
		return;
	}
	if (!it->second->IsOp(c)) {
		sendReply(m, c, 482);
		return;
	}
	it->second->addinv(target);
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 INVITE " + m.getSettings()[0] + " :" + m.getSettings()[1] +"\r\n";
	sendM(target, msg);
	sendReply(m, c, 341);
}

void Server::cmdTopic(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(m, c,461);
		return;
	}
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) {
		sendReply(m, c, 403);
		return;
	}
	if (!it->second->IsInChannel(c)) {
		sendReply(m, c, 442);
		return;
	}
	std::string nick = c->getNickname(); 
	if (nick.empty())
		nick = "*";
	if (m.getSettings().size() == 1) {
		if(it->second->getTopic() == "") {
			std::string msg = ":Best_Server 331 " + nick + " " + m.getSettings()[0] + " :No topic is set\r\n";
			sendM(c, msg);
			return;
		}
		else {
			std::string msg = ":Best_Server 332 " + nick + " " + m.getSettings()[0] + " :" + it->second->getTopic() +"\r\n";
			sendM(c, msg);
			return;
		}
	}
	if (m.getSettings().size() == 2) {
		if (!it->second->IsOp(c) && it->second->getTopicRestricted() == true) {
			sendReply(m, c, 482);
			return;
		}
		it->second->setTopic(m.getSettings()[1]);
		std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 TOPIC " + m.getSettings()[0] +" :" + m.getSettings()[1] + "\r\n";
		std::vector<Client*> participants = it->second->getUsers();
		for (size_t i = 0; i < participants.size(); i++) {
			sendM(participants[i], msg);
		}
	}
}

void Server::cmdMode(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(m, c, 461);
		return;
	}
	if (m.getSettings()[0][0] != '#' && m.getSettings()[0][0] != '&')
		return;
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) {
		sendReply(m, c, 403);
		return;
	}
	if (m.getSettings().size() == 1) {
		std::string activeModes = "+";
		std::string modeParams = "";

		if (it->second->getInviteOnly())
			activeModes += "i";
		if (it->second->getTopicRestricted())
			activeModes += "t";
		if (it->second->getPassword() != "") {
			activeModes += "k";
			modeParams += " " + it->second->getPassword();
		}
		if (it->second->getUserLimit() > 0) {
			activeModes += "l";
			std::ostringstream ss;
			ss << it->second->getUserLimit();
			modeParams += " " + ss.str();
		}
		std::string msg = ":Best_Server 324 " + c->getNickname() + " " + m.getSettings()[0] + " " + activeModes + modeParams + "\r\n";
		sendM(c, msg);
		return;
	}
	if (m.getSettings()[1] == "b" || m.getSettings()[1] == "+b")
		return;
	if (!it->second->IsOp(c)) {
		sendReply(m, c, 482);
		return;
	}
	bool adding = true;
	size_t argIndex = 2;
	std::string modes = m.getSettings()[1];
	for (size_t i = 0; i < modes.length(); i++) {
		char ch = modes[i];
		if (ch == '+')
			adding = true;
		else if (ch == '-')
			adding = false;
		else if (ch == 'i')
			it->second->setInviteOnly(adding);
		else if (ch == 't')
			it->second->setTopicRestricted(adding);
		else if (ch == 'k') {
			if (adding) {
				if (argIndex < m.getSettings().size() && !m.getSettings()[argIndex].empty()) {
					it->second->setPassword(m.getSettings()[argIndex]);
					argIndex++;
				}
				else {
					sendReply(m, c, 461);
					return;
				}
			}
			else {
				it->second->setPassword("");
				if (argIndex < m.getSettings().size())
					argIndex++;
			}
		}
		else if (ch == 'l') {
			if (adding) {
				if (argIndex < m.getSettings().size()) {
					int limit = atoi(m.getSettings()[argIndex].c_str());
					if (limit <= 0)
						return;
					it->second->setUserLimit(limit);
					argIndex++;
				}
				else {
					sendReply(m, c, 461);
					return;
				}
			}
			else {
				it->second->setUserLimit(0);
			}
		}
		else if (ch == 'o') {
			if (argIndex < m.getSettings().size()) {
				std::string targetNick = m.getSettings()[argIndex];
				Client* target = NULL;
				std::vector<Client*> participants = it->second->getUsers();
				for (size_t j = 0; j < participants.size(); j++) {
					if (participants[j]->getNickname() == targetNick) {
						target = participants[j];
						break;
					}
				}
				if (target != NULL) {
					if (adding)
						it->second->addOp(target);
					else
						it->second->RemoveOp(target);
				}
				else {
					sendReply(m, c, 441);
					return;
				}
				argIndex++;
			}
			else {
				sendReply(m, c, 461);
				return;
			}
		}
		else {
			sendReply(m, c, 472);
			return;
		}
	}
	std::string modeArgs = "";
	for (size_t i = 2; i < argIndex; i++)
		modeArgs += " " + m.getSettings()[i];
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 MODE " + m.getSettings()[0] + " " + modes + modeArgs + "\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		sendM(participants[i], msg);
	}
}

void Server::sendReply(const Message& m, Client* c, int err) {
	std::string msg;
	std::string nick = c->getNickname(); 
	if (nick.empty())
		nick = "*";
	if (err == 475)
    	msg = ":Best_Server 475 " + nick + " " + m.getSettings()[0] + " :Cannot join channel (+k)\r\n";
	else if (err == 472)
        msg = ":Best_Server 472 " + nick + " " + m.getSettings()[0] + " :is unknown mode char to me\r\n";
	else if (err == 473)
    	msg = ":Best_Server 473 " + nick + " " + m.getSettings()[0] + " :Cannot join channel (+i)\r\n";
	else if (err == 471)
    	msg = ":Best_Server 471 " + nick + " " + m.getSettings()[0] + " :Cannot join channel (+l)\r\n";
	else if (err == 341)
        msg = ":Best_Server 341 " + nick + " " + m.getSettings()[0] + " " + m.getSettings()[1] + "\r\n";
	else if (err == 462)
        msg = ":Best_Server 462 " + nick + " :You may not reregister\r\n";
    else if (err == 461)
        msg = ":Best_Server 461 " + nick + " " + m.getCmd() + " :Not enough parameters\r\n";
    else if (err == 464)
        msg = ":Best_Server 464 " + nick + " :Password incorrect\r\n";
	else if (err == 431)
		msg = ":Best_Server 431 " + nick + " :No nickname given\r\n";
	else if (err == 432)
		msg = ":Best_Server 432 " + nick + " :Erroneous Nickname\r\n";
	else if (err == 433)
		msg = ":Best_Server 433 " + nick + " :Nickname is already in use.\r\n";
	else if (err == 482)
		msg = ":Best_Server 482 " + nick + " " + m.getSettings()[0] + " :You're not channel operator\r\n";
	else if (err == 442)
		msg = ":Best_Server 442 " + nick + " " + m.getSettings()[0] + " :You're not on that channel\r\n";
	else if (err == 443)
		msg = ":Best_Server 443 " + nick + " " + m.getSettings()[0] + " " + m.getSettings()[1] + " :is already on channel\r\n";	
	else if (err == 441) {
    	if (m.getCmd() == "MODE" && m.getSettings().size() >= 3)
        	msg = ":Best_Server 441 " + nick + " " + m.getSettings().back() + " " + m.getSettings()[0] + " :They aren't on that channel\r\n";
    	else
        	msg = ":Best_Server 441 " + nick + " " + m.getSettings()[1] + " " + m.getSettings()[0] + " :They aren't on that channel\r\n";
	}
	else if (err == 412)
		msg = ":Best_Server 412 " + nick + " :No text to send\r\n";
	else if (err == 451)
		msg = ":Best_Server 451 " + nick + " :You have not registered\r\n";
	else if (err == 409)
    	msg = ":Best_Server 409 " + nick + " :No origin specified\r\n";
	else if (err == 411)
		msg = ":Best_Server 411 " + nick + " :No recipient given " + m.getCmd() + "\r\n";
	else if (err == 401)
		msg = ":Best_Server 401 " + nick + " " + m.getSettings()[0] + " :No such nick/channel\r\n";
	else if (err == 403)
		msg = ":Best_Server 403 " + nick + " " + m.getSettings()[0] + " :No such channel\r\n";
	else if (err == 421)
    	msg = ":Best_Server 421 " + nick + " " + m.getCmd() + " :Unknown command\r\n";
	else if (err == 404)
		msg = ":Best_Server 404 " + nick + " " + m.getSettings()[0] + " :Cannot send to channel\r\n";
	else if (err == 1)
		msg = ":Best_Server 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + c->getUsername() + "@127.0.0.1\r\n";
	sendM(c, msg);
}

void Server::checkRegistration(const Message& m, Client* c) {
	if (c->getNamecheck() == true && c->getNicknamecheck() == true && c->getPasswordcheck() == true && c->getIsSignup() == false) {
		c->setSignup(true);
		sendReply(m, c, 1);
	}
}