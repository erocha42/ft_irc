#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <iostream>
#include <vector>

class Message
{
  	private:
		std::string _prefix;
        std::string _cmd;
        std::vector<std::string> _settings;

  	public:
		Message();
		Message(std::string receive);
        Message(Message const &src);
        Message &operator=(Message const &rhs);
        ~Message();

		const std::string &getCmd() const;
		const std::vector<std::string> &getSettings() const;
	
};

#endif