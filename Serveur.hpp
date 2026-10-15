#ifndef SERVEUR_HPP
#define SERVEUR_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Channel.hpp"
#include "Client.hpp"


class Serveur
{
	private:
		std::map<int, Client> _all_clients;
		std::map<std::string, Channel> _all_channels;
		std::string _password;
  	public:
		Serveur();
		Serveur(int port, std::string password);
        Serveur( Serveur const &src );
        Serveur &operator=( Serveur const & rhs );
        ~Serveur();
};

#endif