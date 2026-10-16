#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <iostream>
#include <vector>
#include "Client.hpp"

class Channel
{
  	private:
		std::string _name;
		std::string _topic;
		std::vector<Client*> _user_in_Channel;
		std::vector<Client*> _user_op;
		std::vector<Client*> _invited_client;
		bool _inviteOnly;
		bool _topicRestricted;
		std::string _password;
		int _user_limit;
		Channel( Channel const &src );
        Channel &operator=( Channel const & rhs );

  	public:
		Channel();
		Channel(std::string name);
        ~Channel();
	
};

#endif