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
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sstream>
#include <cstdlib>


class Server
{
	private:
		int _fdl;
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
		void cmdInvite(const Message& m, Client* c);
		void cmdTopic(const Message& m, Client* c);
		void cmdMode(const Message& m, Client* c);
		void cmdPing(const Message& m, Client* c);

		void checkRegistration(const Message& m, Client* c);

  	public:
		Server();
		Server(int port, std::string password);
        ~Server();

		bool	fdl_init();
		void	executeCommand(const Message& m, Client* c);
};

#endif