#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <map>
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <sys/socket.h>


class Server
{
	private:
		int _port;
		std::map<int, Client*> _all_clients;
		std::map<std::string, Channel*> _all_channels;
		std::string _password;
		Server( Server const &src );
        Server &operator=( Server const & rhs );

		void sendReply(const Message& m, Client* c, int err);

	    void cmdPass(const Message& m, Client* c);
		void cmdNick(const Message& m, Client* c);
		void cmdUser(const Message& m, Client* c);
		void cmdPrivmsg(const Message& m, Client* c);
		void cmdJoin(const Message& m, Client* c);
		void cmdPart(const Message& m, Client* c);
		void cmdQuit(const Message& m, Client* c);
		void cmdKick(const Message& m, Client* c);

		void checkRegistration(const Message& m, Client* c);

  	public:
		Server();
		Server(int port, std::string password);
        ~Server();

		int executeCommand(const Message& m, Client* c);
};

#endif