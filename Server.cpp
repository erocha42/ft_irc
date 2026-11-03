#include "Server.hpp"

Server::Server() : _port(-1), _all_clients(), _all_channels(), _password("Default") {	
}

Server::Server(int port, std::string password) : _all_clients(), _all_channels() {
	_port = port;
	_password = password;
}

Server::~Server() {
	std::map<int, Client *>::iterator it;
	std::map<std::string, Channel*>::iterator it2;
	for (it = _all_clients.begin(); it != _all_clients.end(); it++) {   
        delete it->second;
    }
	for (it2 = _all_channels.begin(); it2 != _all_channels.end(); it2++) {   
        delete it2->second;
    }
    _all_clients.clear();
	_all_channels.clear();
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
	std::string msg = "PONG Best_Server " + m.getSettings()[0] + "\r\n";
	send(c->getFd(), msg.c_str(), msg.length(), 0);
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
	if (m.getSettings().size() < 1) {
		sendReply(m, c,431);
		return;
	}
	if (!isalpha(m.getSettings()[0][0]) && std::strchr("[]\\'\"^_-{ | }", m.getSettings()[0][0]) == NULL) {
		sendReply(m, c,432);
		return;
	}
	for (size_t i = 1; i < m.getSettings()[0].length(); i++) {
		if (!isalnum(m.getSettings()[0][i]) && std::strchr("[]\\'\"^_-{ | }", m.getSettings()[0][i]) == NULL ){
			sendReply(m, c,432);
			return;
		}
	}
	for (std::map<int, Client*>::iterator it = _all_clients.begin(); it != _all_clients.end(); it++ ) {
		if (it->second->getNickname() == m.getSettings()[0]){
			sendReply(m, c,433);
			return;
		}
	}
	if (c->getIsSignup()) {
		std::string msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 NICK :" + m.getSettings()[0] + "\r\n";
		send(c->getFd(), msg.c_str(), msg.length(), 0);
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
	if (m.getSettings().size() != 4) {
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
	if (m.getSettings().size() == 1) {
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
					send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
			}
			return;
		}
	}
	else {
		for (std::map<int, Client*>::iterator it = _all_clients.begin(); it != _all_clients.end(); it++ ){
			if (it->second->getNickname() == m.getSettings()[0]){
				std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 PRIVMSG " + m.getSettings()[0] + " :" + m.getSettings()[1] +"\r\n";
				send(it->second->getFd(), msg.c_str(), msg.length(), 0);
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
	if (m.getSettings()[0][0] != '#' && m.getSettings()[0][0] != '&') {
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
		send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
	}
	std::string list_name = "";
	for (size_t i = 0; i < participants.size(); i++){
		if (target_channel->IsOp(participants[i]))
			list_name += "@" + participants[i]->getNickname() + " ";
		else
			list_name += participants[i]->getNickname() + " ";
	}
	std::string msg353 = ":Best_Server 353 " + c->getNickname() + " = " + m.getSettings()[0] + " :" + list_name + "\r\n";
	send(c->getFd(), msg353.c_str(), msg353.length(), 0);
	std::string endofname = ":Best_Server 366 " + c->getNickname() + " " + m.getSettings()[0] + " :End of /NAMES list.\r\n";
	send(c->getFd(), endofname.c_str(), endofname.length(), 0);
}

void Server::cmdPart(const Message& m, Client* c) {
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
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 PART " + m.getSettings()[0] +" :Leaving in progress\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
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
				send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
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
	_all_clients.erase(c->getFd());
	close(c->getFd());
	delete c;
}

void Server::cmdKick(const Message& m, Client* c){
	if (m.getSettings().size() < 2) {
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
	if (!it->second->IsOp(c)) {
		sendReply(m, c, 482);
		return;
	}
	if (!it->second->IsInChannelStr(m.getSettings()[1])) {
		sendReply(m, c, 441);
		return;
	}
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 KICK " + m.getSettings()[0] + " " + m.getSettings()[1] +" :was kick !\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
	}
	Client* target;
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
	send(target->getFd(), msg.c_str(), msg.length(), 0);
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
			send(c->getFd(), msg.c_str(), msg.length(), 0);
			return;
		}
		else {
			std::string msg = ":Best_Server 332 " + nick + " " + m.getSettings()[0] + " :" + it->second->getTopic() +"\r\n";
			send(c->getFd(), msg.c_str(), msg.length(), 0);
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
			send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
}

void Server::cmdMode(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(m, c,461);
		return;
	}
	std::map<std::string, Channel*>::iterator it = _all_channels.find(m.getSettings()[0]);
	if (it == _all_channels.end()) { 
		sendReply(m, c, 403); 
		return; 
	}
	if (m.getSettings().size() == 1) {
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
    		if (activeModes == "+") {
        		activeModes = "";
			}
			std::string msg = ":Best_Server 324 " + c->getNickname() + " " + m.getSettings()[0] + " " + activeModes + modeParams + "\r\n";
    		send(c->getFd(), msg.c_str(), msg.length(), 0);
    		return;
		}
	}
	if (m.getSettings().size() >= 2) {
		if (!it->second->IsOp(c)) {
			sendReply(m, c, 482);
			return;
		}
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
			if(adding && argIndex < m.getSettings().size()) {
    	    	it->second->setPassword(m.getSettings()[argIndex]);
    	    	argIndex++;
			}
			else if (!adding) {
    	        it->second->setPassword("");
				if (argIndex < m.getSettings().size())
					argIndex++;
    	    }
    	}
    	else if (ch == 'l') {
    	    if (adding && argIndex < m.getSettings().size()) {
				it->second->setUserLimit(atoi(m.getSettings()[argIndex].c_str()));
				argIndex++;
			}
    	    if (!adding)
				it->second->setUserLimit(0);
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
		}
    	else {
        	sendReply(m, c, 472);
			return;
    	}
	}
	std::string modeArgs = "";
	for (size_t i = 2; i < m.getSettings().size(); i++)
		modeArgs += " " + m.getSettings()[i];
	std::string msg = ":" + c->getNickname() + "!" + c->getUsername() + "@127.0.0.1 MODE " + m.getSettings()[0] + " " + modes + modeArgs + "\r\n";
	std::vector<Client*> participants = it->second->getUsers();
	for (size_t i = 0; i < participants.size(); i++) {
		send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
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
	else if (err == 441)
		msg = ":Best_Server 441 " + nick + " " + m.getSettings()[1] + " " + m.getSettings()[0] + " :They aren't on that channel\r\n";
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
	send(c->getFd(), msg.c_str(), msg.length(), 0);
}

void Server::checkRegistration(const Message& m, Client* c) {
	if (c->getNamecheck() == true && c->getNicknamecheck() == true && c->getPasswordcheck() == true && c->getIsSignup() == false) {
		c->setSignup(true);
		sendReply(m, c, 1);
	}
}