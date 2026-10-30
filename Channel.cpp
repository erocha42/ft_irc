#include "Channel.hpp"

Channel::Channel() : _name("Default"), _topic("Default"), _user_in_channel(), _user_op(), _invited_client(),
 	_inviteOnly(false), _topicRestricted(false), _password("Default"), _user_limit(-1) {	
}

Channel::Channel(std::string name) : _topic("Default"), _user_in_channel(), _user_op(), _invited_client(),
 	_inviteOnly(false), _topicRestricted(false), _password("Default"), _user_limit(-1){
	_name = name;
}
        
Channel::~Channel() {
	
}

const std::string &Channel::getName() const {
	return _name;
}

const std::vector<Client*> &Channel::getUsers() const {
	return _user_in_channel;
}