/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/13 21:09:51 by ccottin          ###   ########.fr       */
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

int			ft_error(void)
{
	std::cout << "Usage : ./ft_otp [-g key file] or [-k password file]";
	return (1);
}

void		stock_new_key(std::string key)
{
	//open modes neededs = trunc for discarding all previous contents
//in for input


}

int			main(int ac, char **av)
{
	if (ac != 3 || strncmp(av[1], "-g", 3) || strncmp(av[1], "-k", 3)
				|| (!strncmp(av[1], "-k", 3) && strncmp(av[2], "ft_otp.key", 11)))
		return (ft_error());
	
	if (strncmp(av[1], "-g", 3))
		stock_new_key(av[2]);
	if (strncmp(av[1], "-k", 3))
		//generate password
}
