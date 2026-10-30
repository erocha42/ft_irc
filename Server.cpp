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

int Server::executeCommand(const Message& m, Client* c) {
	if (m.getCmd() == "PASS")
		cmdPass(m, c);
	else if (m.getCmd() == "NICK")
		cmdNick(m, c);
	else if (m.getCmd() == "USER")
		cmdUser(m, c);
	else if (m.getCmd() == "PRIVMSG")
		cmdPrivmsg(m, c);
	else if (m.getCmd() == "JOIN")
		...
	else if (m.getCmd() == "PART")
		...
	else if (m.getCmd() == "QUIT")
		...
	else if (m.getCmd() == "KICK")
		...
	else if (m.getCmd() == "INVITE")
		...
	else if (m.getCmd() == "TOPIC")
		...
	else if (m.getCmd() == "MODE")
		...
	else
		...
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
	if (m.getSettings().size() < 1) {
		sendReply(m, c,431);
		return;
	}
	if (!isalpha(m.getSettings()[0][0]) && std::strchr("[]\\'\"^_-mo{ | }", m.getSettings()[0][0]) == NULL) {
		sendReply(m, c,432);
		return;
	}
	for (int i = 1; i < m.getSettings()[0].length(); i++) {
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
	c->setNickname(m.getSettings()[0]);
	checkRegistration(m, c);
}

void Server::cmdUser(const Message& m, Client* c) {
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
		for (std::map<std::string, Channel*>::iterator it = _all_channels.begin(); it != _all_channels.end(); it++ ){
			if (it->second->getName() == m.getSettings()[0]){
				std::string msg = ":" + c->getNickname() + "!" + c->getUsername() +"@127.0.0.1 PRIVMSG " + m.getSettings()[0] + " :" + m.getSettings()[1] +"\r\n";
				std::vector<Client*> participants = it->second->getUsers();
				for (size_t i = 0; i < participants.size(); i++) {
					if (participants[i] != c)
						send(participants[i]->getFd(), msg.c_str(), msg.length(), 0);
				}
				return;
			}
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

void Server::sendReply(const Message& m, Client* c, int err) {
	std::string msg;
	std::string nick = c->getNickname(); 
    if (nick.empty())
        nick = "*";
	if (err == 462)
        msg = ":Best_Server 462 " + nick + " :You may not reregister\r\n";
    else if (err == 461)
        msg = ":Best_Server 461 " + nick + " " + m.getCmd() + ":Not enough parameters\r\n";
    else if (err == 464)
        msg = ":Best_Server 464 " + nick + " :Password incorrect\r\n";
	else if (err == 431)
		msg = ":Best_Server 431 " + nick + " :No nickname given\r\n";
	else if (err == 432)
		msg = ":Best_Server 432 " + nick + " :Erroneous Nickname\r\n";
	else if (err == 433)
		msg = ":Best_Server 433 " + nick + " :Nickname is already in use.\r\n";
	else if (err == 412)
		msg = ":Best_Server 412 " + nick + " :No text to send\r\n";
	else if (err == 411)
		msg = ":Best_Server 411 " + nick + " :No recipient given " + m.getCmd() + "\r\n";
	else if (err == 401)
		msg = ":Best_Server 401 " + nick + " " + m.getSettings()[0] + " :No such nick/channel\r\n";
	else if (err == 001)
		msg = ":Best_Server 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + c->getUsername() + "@127.0.0.1\r\n";
	send(c->getFd(), msg.c_str(), msg.length(), 0);
}

void Server::checkRegistration(const Message& m, Client* c) {
	if (c->getNamecheck() == true && c->getNicknamecheck() == true && c->getPasswordcheck() == true && c->getIsSignup() == false) {
		c->setSignup(true);
		sendReply(m, c,001);
	}
}