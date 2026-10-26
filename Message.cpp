#include "Message.hpp"

Message::Message() : _prefix(), _cmd(), _settings() {
}

Message::Message(std::string receive) {
    int i = 0;
    int j = 0;
    if (receive[0] == ':') {
        _prefix = receive.substr(1, receive.find(' ') - 1);
        receive.erase(0, receive.find(' ') + 1);
    }
    if (receive.find(' ') != std::string::npos) {
        j = receive.find(' ');
        _cmd = receive.substr(0, j);
        receive.erase(0, j + 1);
    }
    else {
        j = receive.length();
        _cmd = receive.substr(0, j);
        receive.erase(0, j);
    }
    while (receive.length() > 0) {
        if (receive[0] == ':') {
            _settings.push_back(receive.substr(1,receive.length() - 1));
            break;
        }
        else {
            if (receive.find(' ') != std::string::npos) {
                i = receive.find(' ');
                _settings.push_back(receive.substr(0, i));
                receive.erase(0, i + 1);
            }
            else {
                i = receive.length();
                _settings.push_back(receive.substr(0, i));
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