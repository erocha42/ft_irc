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

void Channel::addInChannel(Client* c) {
	for (int i = 0; _user_in_channel.size(); i++) {
		if (_user_in_channel[i] == c)
			return;
	}
	_user_in_channel.push_back(c);
}

void Channel::addOp(Client* c) {
	for (int i = 0; _user_op.size(); i++) {
		if (_user_op[i] == c)
			return;
	}
	_user_op.push_back(c);
}

bool Channel::IsInChannel(Client *c) {
	for (int i = 0; _user_in_channel.size(); i++) {
		if (_user_in_channel[i] == c)
			return true;
	}
	return false;
}

bool Channel::IsOp(Client *c) {
	for (int i = 0; _user_op.size(); i++) {
		if (_user_op[i] == c)
			return true;
	}
	return false;
}

void Channel::removeClient(Client *c) {
	for (std::vector<Client*>::iterator it = _user_in_channel.begin(); it != _user_in_channel.end(); it++ ) {
		if (*it == c){
			_user_in_channel.erase(it);
			break;
		}
	}
	for (std::vector<Client*>::iterator it = _user_op.begin(); it != _user_op.end(); it++ ) {
		if (*it == c){
			_user_op.erase(it);
			break;
		}
	}
}

int Channel::sizeUser() {
	return _user_in_channel.size();
}