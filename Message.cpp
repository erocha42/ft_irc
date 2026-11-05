#include "Message.hpp"

Message::Message() : _prefix(), _cmd(), _settings() {
}

Message::Message(std::string receive) {
	while (!receive.empty() && (receive[receive.size() - 1] == '\r' || receive[receive.size() - 1] == '\n'))
   		receive.erase(receive.size() - 1);
    while (!receive.empty() && receive[0] == ' ')
        receive.erase(0, 1);
    if (!receive.empty() && receive[0] == ':') {
        size_t pos = receive.find(' ');
        if (pos != std::string::npos) {
            _prefix = receive.substr(1, pos - 1);
            receive.erase(0, pos + 1);
        } 
		else {
            _prefix = receive.substr(1);
            receive.clear();
        }
    }
    while (!receive.empty() && receive[0] == ' ')
        receive.erase(0, 1);
    if (receive.find(' ') != std::string::npos) {
        size_t j = receive.find(' ');
        _cmd = receive.substr(0, j);
        receive.erase(0, j + 1);
    } 
	else {
        _cmd = receive;
        receive.clear();
    }
    while (!receive.empty()) {
        while (!receive.empty() && receive[0] == ' ')
            receive.erase(0, 1);
        if (receive.empty())
            break;
        if (receive[0] == ':') {
            _settings.push_back(receive.substr(1));
            break;
        } 
		else {
            size_t i = receive.find(' ');
            if (i != std::string::npos) {
                _settings.push_back(receive.substr(0, i));
                receive.erase(0, i + 1);
            } 
			else {
                _settings.push_back(receive);
                receive.clear();
            }
        }
    }
}

Message::Message(Message const &src) : _prefix(src._prefix), _cmd(src._cmd), _settings(src._settings){
}

Message &Message::operator=(Message const &rhs) {
    if (this != &rhs){
        this->_prefix = rhs._prefix;
        this->_cmd = rhs._cmd;
        this->_settings = rhs._settings;
    }
    return *this;
}

Message::~Message() {
}

const std::string &Message::getCmd() const {
	return _cmd;
}

const std::vector<std::string> &Message::getSettings() const {
	return _settings;
}