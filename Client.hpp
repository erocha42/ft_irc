#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

#define IN 0
#define OUT 1

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
		std::string _in_buffer;
		std::string _out_buffer;
		Client( Client const &src );
        Client &operator=( Client const & rhs );
  	public:
		Client();
		Client(int fd);
        ~Client();

		const bool &getIsSignup() const;
		const int &getFd() const;
		std::string &getBuffer(int type);
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

		void appendBuffer(const std::string& str, int type);
		void clearBuffer();
};

#endif