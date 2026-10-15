#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

class Client
{
  	private:
		int _fd;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		bool _password_check;
		bool _nickname_check;
		bool _name_check;
		bool _is_signup;
  	public:
		Client();
		Client(int fd);
        Client( Client const &src );
        Client &operator=( Client const & rhs );
        ~Client();
};

#endif