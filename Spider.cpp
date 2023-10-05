#include <string>
#include <iostream>

int		main(int ac, char **av)
{
	if (ac == 0)
		return (0);

	std::string	args[ac];

	for (int i = 1; i < ac; i++)
		args[i - 1] = av[i];
}

