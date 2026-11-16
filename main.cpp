#include "Server.hpp"
#include <cstdlib>
#include <cctype>

void handler(int sig)
{
	flag = sig;
}

static bool	check_initer(char *port, std::string const& pwd)
{
	for (int i = 0; port[i]; i++)
	{
		if (!isdigit(port[i]))
		{
			std::cerr << "Port error" << std::endl;
			return false;
		}
	}
	int port_nbr = atoi(port);
	if (port_nbr < 1 || port_nbr > 65535 || pwd.empty())
		return false;
	for (int i = 0; pwd[i]; i++)
	{
		if (pwd[i] == ' ')
		{
			std::cerr << "A space in password" << std::endl;
			return false;
		}
	}
	return true;
}

int	main(int ac, char **av)
{
	if (ac != 3 || !check_initer(av[1], av[2]))
	{
		std::cerr << "Usage: ./ircserv <port> <password>." << "\n";
		return 1;
	}
	Server	srv(atoi(av[1]), av[2]);
	if (!srv.fdl_init())
		return 1;
	signal(SIGINT, handler);
	srv.connection_loop();
}