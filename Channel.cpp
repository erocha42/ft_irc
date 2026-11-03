#include "Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _user_in_channel(), _user_op(), _invited_client(),
 	_inviteOnly(false), _topicRestricted(false), _password(""), _user_limit(0) {	
}

Channel::Channel(std::string name) : _topic(""), _user_in_channel(), _user_op(), _invited_client(),
 	_inviteOnly(false), _topicRestricted(false), _password(""), _user_limit(0){
	_name = name;
}
        
Channel::~Channel() {
	
}

const std::string &Channel::getName() const {
	return _name;
}

const std::string &Channel::getTopic() const {
	return _topic;
}

bool Channel::getInviteOnly() const {
	return _inviteOnly;
}

bool Channel::getTopicRestricted() const { 
	return _topicRestricted; 
}

std::string Channel::getPassword() const { 
	return _password; 
}

size_t Channel::getUserLimit() const { 
	return _user_limit; 
}

const std::vector<Client*> &Channel::getUsers() const {
	return _user_in_channel;
}

void Channel::addInChannel(Client* c) {
	for (size_t i = 0; i < _user_in_channel.size(); i++) {
		if (_user_in_channel[i] == c)
			return;
	}
	_user_in_channel.push_back(c);
}

void Channel::addOp(Client* c) {
	for (size_t i = 0; i <_user_op.size(); i++) {
		if (_user_op[i] == c)
			return;
	}
	_user_op.push_back(c);
}

void Channel::addinv(Client* c) {
	for (size_t i = 0; i <_invited_client.size(); i++) {
		if (_invited_client[i] == c)
			return;
	}
	_invited_client.push_back(c);
}

void Channel::RemoveOp(Client* c) {
	for (size_t i = 0; i <_user_op.size(); i++) {
		if (_user_op[i] == c) {
			_user_op.erase(_user_op.begin() + i);
			return;
		}
	}
}

bool Channel::IsInChannel(Client *c) {
	for (size_t i = 0; i < _user_in_channel.size(); i++) {
		if (_user_in_channel[i] == c)
			return true;
	}
	return false;
}

bool Channel::IsInChannelStr(std::string c) {
	for (size_t i = 0; i < _user_in_channel.size(); i++) {
		if (_user_in_channel[i]->getNickname() == c)
			return true;
	}
	return false;
}

bool Channel::IsOp(Client *c) {
	for (size_t i = 0; i < _user_op.size(); i++) {
		if (_user_op[i] == c)
			return true;
	}
	return false;
}

bool Channel::IsInvited(Client *c) {
	for (size_t i = 0; i < _invited_client.size(); i++) {
		if (_invited_client[i] == c)
			return true;
	}
	return false;
}

void Channel::removeInv(Client *c) {
	for (std::vector<Client*>::iterator it = _invited_client.begin(); it != _invited_client.end(); it++) {
		if (*it == c) {
			_invited_client.erase(it);
			break;
		}
	}
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
	removeInv(c);
}

int Channel::sizeUser() {
	return _user_in_channel.size();
}

void Channel::setTopic(std::string topic) {
	_topic = topic;
}

void Channel::setInviteOnly(bool state) { 
	_inviteOnly = state; 
}

void Channel::setTopicRestricted(bool state) { 
	_topicRestricted = state; 
}

void Channel::setPassword(const std::string& password) { 
	_password = password; 
}

void Channel::setUserLimit(size_t limit) { 
	_user_limit = limit; 
}