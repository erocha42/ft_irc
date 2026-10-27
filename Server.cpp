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
		...;
	else if (m.getCmd() == "USER")
		...
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
	return;
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
	send(c->getFd(), msg.c_str(), msg.length(), 0);
}