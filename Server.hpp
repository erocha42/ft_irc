#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Channel.hpp"
#include "Client.hpp"


class Server
{
	private:
		int _port;
		std::map<int, Client*> _all_clients;
		std::map<std::string, Channel*> _all_channels;
		std::string _password;
		Server( Server const &src );
        Server &operator=( Server const & rhs );
  	public:
		Server();
		Server(int port, std::string password);
        ~Server();
};

#endif