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
		Client( Client const &src );
        Client &operator=( Client const & rhs );
  	public:
		Client();
		Client(int fd);
        ~Client();

		const bool &getIsSignup() const;
		const int &getFd() const;
		const std::string &getNickname() const;
		const std::string &getUsername() const;
		const bool &getPasswordcheck() const;
		const bool &getNicknamecheck() const;
		const bool &getNamecheck() const;

		void setPasswordCheck(bool status);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setRealname(std::string realname);
		void setSignup(bool signup);
};

#endif