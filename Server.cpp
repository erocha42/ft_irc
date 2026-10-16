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