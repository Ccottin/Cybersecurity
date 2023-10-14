/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/14 14:32:42 by ccottin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

//preciser les modes directements dans l invocation de ta fonction comme ca 
//si tu passes un to_write, t as juste a ajouter ta cle izi
//modes neededs = trunc for discarding all previous contents
//in for input
//out for output

std::string	file_error(void)
{
	std::cerr << "Problem with key file.\n";
	exit(1);	//faire mieux maybe
}

std::string	file_manager(const char *filename, std::ios_base::openmode mode,
		const char *to_write)
{
	std::fstream		fs;
	std::string			ret;
	std::stringstream	buffer;
	
	fs.open(filename, mode);
	if (fs.fail())
		return (file_error());
	if (to_write)
		fs << to_write;
	else
	{
		buffer << fs.rdbuf();
		ret = buffer.str();
	}
	fs.close();
	return (ret);
}

void		ft_error(int err)
{
	switch (err) 
	{
		case 1:
			std::cerr << "Usage : ./ft_otp [-g key file] or [-k password file]";
			break ;
		case 2:
			std::cerr << "error: key must be 64 hexadecimal characters.";
			break ;
		case 3:
			std::cerr << "error: key initialasation went wrong.";
			break ;

	}
	exit(1);
}

int			time_steps_calculator(void)
{
	
}

void		generate_password(void)
{
	std::string	key = file_manager("ft_opt.key", std::ios_base::out, NULL);
	if (key.empty())
		ft_error(3);
	//time steps needed
}

void		stock_new_key(std::string key)
{
	int		i;

	i = 0;
	while (std::isalnum(key[i]))
		++i;
	if (i != key.length() || i < 64)
		ft_error(2);
	file_manager("ft_otp.key", std::ios_base::trunc | std::ios_base::in, key.c_str());
}

int			main(int ac, char **av)
{
	if (ac != 3 || strncmp(av[1], "-g", 3) || strncmp(av[1], "-k", 3)
				|| (!strncmp(av[1], "-k", 3) && strncmp(av[2], "ft_otp.key", 11)))
		ft_error(1);
	
	if (strncmp(av[1], "-g", 3))
		stock_new_key(av[2]);
	if (strncmp(av[1], "-k", 3))
		generate_password();
}
