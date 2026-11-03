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
		std::vector<Client*> _user_in_channel;
		std::vector<Client*> _user_op;
		std::vector<Client*> _invited_client;
		bool _inviteOnly;
		bool _topicRestricted;
		std::string _password;
		size_t _user_limit;
		Channel( Channel const &src );
        Channel &operator=( Channel const & rhs );

  	public:
		Channel();
		Channel(std::string name);
        ~Channel();

		const std::string &getName() const;
		const std::string &getTopic() const;
		bool getInviteOnly() const;
    	bool getTopicRestricted() const;
    	std::string getPassword() const;
    	size_t getUserLimit() const;
		const std::vector<Client*> &getUsers() const;

		void addInChannel(Client* c);
		void addOp(Client* c);
		void addinv(Client* c);
		void RemoveOp(Client* c);
		bool IsInChannel(Client* c);
		bool IsInChannelStr(std::string c);
		bool IsOp(Client* c);

		void removeClient(Client *c);
		int sizeUser();
		void setTopic(std::string topic);
		void setInviteOnly(bool state);
    	void setTopicRestricted(bool state);
    	void setPassword(const std::string& password);
    	void setUserLimit(size_t limit);
		bool IsInvited(Client* c);
		void removeInv(Client* c);
};

#endif