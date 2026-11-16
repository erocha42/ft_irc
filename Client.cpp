#include "Client.hpp"

Client::Client() : _fd(-1), _nickname(""), _username(""), _realname(""), _password_check(false),
	_nickname_check(false), _name_check(false), _is_signup(false){	
}

Client::Client(int fd) : _nickname(""), _username(""), _realname(""), _password_check(false),
	_nickname_check(false), _name_check(false), _is_signup(false) { 
	_fd = fd;
}

Client::~Client() {
	
}

const bool &Client::getIsSignup() const {
	return _is_signup;
}

const int &Client::getFd() const {
	return _fd;
}

const std::string &Client::getNickname() const {
	return _nickname;
}

const std::string &Client::getUsername() const {
	return _username;
}

const bool &Client::getPasswordcheck() const {
	return _password_check;
}

const bool &Client::getNicknamecheck() const {
	return _nickname_check;
}

const bool &Client::getNamecheck() const {
	return _name_check;
}



void Client::setPasswordCheck(bool status) {
    _password_check = status;
}

void Client::setNickname(std::string nickname) {
    _nickname = nickname;
	_nickname_check = 1;
}

void Client::setUsername(std::string username) {
    _username = username;
}

void Client::setRealname(std::string realname) {
    _realname = realname;
	_name_check = 1;
}

void Client::setSignup(bool signup) {
	_is_signup = signup;
}

std::string& Client::getBuffer(int type) {
	if (type == IN)
		return _in_buffer;
	return _out_buffer;
}

void Client::appendBuffer(const std::string& str, int type) { 
	if (type == IN)
		_in_buffer += str;
	else
		_out_buffer += str;
}
void Client::clearBuffer() {
	_in_buffer.clear();
}