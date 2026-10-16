#include "Client.hpp"

Client::Client() : _fd(-1), _nickname("Default"), _username("Default"), _realname("Default"), _password_check(false),
	_nickname_check(false), _name_check(false), _is_signup(false){	
}

Client::Client(int fd) : _nickname("Default"), _username("Default"), _realname("Default"), _password_check(false),
	_nickname_check(false), _name_check(false), _is_signup(false) { 
	_fd = fd;
}

Client::~Client() {
	
}