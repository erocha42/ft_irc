#include "Server.hpp"
#include <cstdlib>

static bool	check_initer(char *port, std::string const& pwd)
{
	for (int i = 0; port[i]; i++)
	{
		if (!isdigit(port[i]))
			return false;
	}
	int port_nbr = atoi(port);
	std::cout << port_nbr << std::endl; // a enlever
	if (port_nbr < 1 || port_nbr > 65535 || pwd.empty())
		return false;
	for (int i = 0; pwd[i]; i++)
	{
		if (pwd[i] == ' ')
			return false;
	}
	return true;
}

int	main(int ac, char **av)
{
	if (ac != 3 || !check_initer(av[1], av[2]))
	{
		std::cout << ac << "\n";
		return 1;
	}
	Server	srv(stoi(static_cast<std::string const&>(av[1])), av[2]);
}