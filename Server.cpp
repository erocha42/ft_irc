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
		cmdUser(m, c)
	else if (m.getCmd() == "PRIVMSG")
		...
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
		sendReply(c,462);
		return;
	}
	if (m.getSettings().size() < 1) {
		sendReply(c,461);
		return;
	}
	if (m.getSettings()[0] != _password) {
		sendReply(c,464);
		return;
	}
	else
		c->setPasswordCheck(true);
	checkRegistration(c);
}

void Server::cmdNick(const Message& m, Client* c) {
	if (m.getSettings().size() < 1) {
		sendReply(c,431);
		return;
	}
	if (!isalpha(m.getSettings()[0][0]) && std::strchr("[]\\'\"^_-mo{ | }", m.getSettings()[0][0]) == NULL) {
		sendReply(c,432);
		return;
	}
	for (int i = 1; i < m.getSettings()[0].length(); i++) {
		if (!isalnum(m.getSettings()[0][i]) && std::strchr("[]\\'\"^_-{ | }", m.getSettings()[0][i]) == NULL ){
			sendReply(c,432);
			return;
		}
	}
	for (int i = 0; _all_clients.size() > i; i++) {
		if (_all_clients[i]->getNickname() == m.getSettings()[0] ){
			sendReply(c,433);
			return;
		}
	}
	c->setNickname(m.getSettings()[0]);
	checkRegistration(c);
}

void Server::cmdUser(const Message& m, Client* c) {
	if (c->getIsSignup() == true) {
		sendReply(c,462);
		return;
	}
	if (m.getSettings().size() != 4) {
		sendReply(c,461);
		return;
	}
	c->setUsername(m.getSettings()[0]);
	c->setRealname(m.getSettings()[3]);
	checkRegistration(c);
}

void Server::cmdPrivmsg(Client* c1, Client* c2) {
	
}

void Server::sendReply(Client* c, int err) {
	std::string msg;
	std::string nick = c->getNickname(); 
    if (nick.empty())
        nick = "*";
	if (err == 462)
        msg = ":Best_Server 462 " + nick + " :You may not reregister\r\n";
    else if (err == 461)
        msg = ":Best_Server 461 " + nick + " PASS :Not enough parameters\r\n";
    else if (err == 464)
        msg = ":Best_Server 464 " + nick + " :Password incorrect\r\n";
	else if (err == 431)
		msg = ":Best_Server 431 " + nick + " :No nickname given\r\n";
	else if (err == 432)
		msg = ":Best_Server 432 " + nick + " :Erroneous Nickname\r\n";
	else if (err == 433)
		msg = ":Best_Server 433 " + nick + " :Nickname is already in use.\r\n";
	else if (err == 001)
		msg = ":Best_Server 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + c->getUsername() + "@127.0.0.1\r\n";
	send(c->getFd(), msg.c_str(), msg.length(), 0);
}

void Server::checkRegistration(Client* c) {
	if (c->getNamecheck() == true && c->getNicknamecheck() == true && c->getPasswordcheck() == true && c->getIsSignup() == false) {
		c->setSignup(true);
		sendReply(c,001);
	}
}