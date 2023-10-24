/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/24 15:33:00 by ccottin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

std::string	file_error(void)
{
	std::cerr << "Problem with key file.\n";
	exit(1);
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
		case 4:
			std::cerr << "error: could not retrieve calendar time.";
			break ;
		case 5:
			std::cerr << "error: time steps is 0.";
			break ;

	}
	exit(1);
}

uint64_t		time_steps_calculator(void)
{
	time_t	current_time;
	uint64_t	time_ref;

	if (TIME_STEPS == 0)
		ft_error(5);
	if (time(&current_time) == -1)
		ft_error(4);						//we get time since t0, the begening of
											//Unix time, then we divide it by time
	time_ref = current_time / TIME_STEPS;	//steps per seconds and truncate the result to get
	return (time_ref);						//the moving factor 'C' (or 'T')

}

unsigned char		*to_hexa(const char *string, unsigned int len, unsigned char *ret)
{
	char res;
	char test[3];
	
	test[2] = 0;
	
	for (unsigned int i = 0 ; i < len ; i +=2)
	{
		test[0] = string[i];
		test[1] = string[i + 1];
		res = 0;
		res = (unsigned char)strtol(test, NULL, 16);
		ret[i / 2] = res;
	}
	return (ret);
}

void		hotp_calculator(std::string key, uint64_t time_ref)
{
	uint8_t			* hmac_result;
	int				offset;
	int				bin_code;
	unsigned char	hexa_key[(key.length() / 2) + 1];

	time_ref = 0;
	time_ref = (time(NULL)) / TIME_STEPS;

	//42 system is little endian so we reverse 
	uint32_t	endianness = 0xdeadbeef;
	if ((*(const uint8_t *)&endianness) == 0xef)
	{
		time_ref = ((time_ref & 0x00000000ffffffff) << 32) | ((time_ref & 0xffffffff00000000) >> 32);
		time_ref = ((time_ref & 0x0000ffff0000ffff) << 16) | ((time_ref & 0xffff0000ffff0000) >> 16);
		time_ref = ((time_ref & 0x00ff00ff00ff00ff) <<  8) | ((time_ref & 0xff00ff00ff00ff00) >>  8);
	};

	//convertir la chaine key en hexa, puis la nourrir au hmac ! :)	
	to_hexa(key.c_str(), key.length(), hexa_key);	
	hmac_result = (uint8_t*)HMAC(EVP_sha1(), (unsigned char*)hexa_key, key.length()/2,
			(unsigned char*)&time_ref, sizeof(time_ref), NULL, NULL);
	
	//get the lower 4 bits and convert them to int, that shloud contain result 
	//between 0 & 15
	offset = hmac_result[19] & 0xf;
	//then we get bytes starting at offset, but we mask the first one to avoid
	//signed-unsigned convertion problem
	bin_code = (hmac_result[offset]  & 0x7f) << 24
           | (hmac_result[offset+1] & 0xff) << 16
           | (hmac_result[offset+2] & 0xff) <<  8
           | (hmac_result[offset+3] & 0xff) ;
	
	bin_code %= 1000000;
	std::cout << std::setw(6) << std::setfill('0') << bin_code << std::endl;
}


void		generate_password(void)
{
	unsigned int		i;
	
	std::string	key = file_manager("ft_otp.key", std::ios_base::in, NULL);
	if (key.empty())
		ft_error(3);
	i = 0;
	while (std::isxdigit(key[i]))
		++i;
	if (i != key.length() || i < 64)
		ft_error(2);

	hotp_calculator(key, time_steps_calculator());
}

void		stock_new_key(char  *in_file)
{
	std::string			key;
	unsigned int		i;
	
	key = file_manager(in_file, std::ios_base::in, NULL);
	i = 0;
	while (std::isxdigit(key[i]))
		++i;
	if (i != key.length() || i < 64 ||
			(i == key.length() - 1 && key[i] == '\n'))
		ft_error(2);
	file_manager("ft_otp.key", std::ios_base::trunc | std::ios_base::out, key.c_str());
}

int			main(int ac, char **av)
{
  if (ac != 3 || (strncmp(av[1], "-g", 3) && strncmp(av[1], "-k", 3))
				|| (!strncmp(av[1], "-k", 3) && strncmp(av[2], "ft_otp.key", 11)))
		ft_error(1);
	
	if (!strncmp(av[1], "-g", 3))
		stock_new_key(av[2]);
	else if (!strncmp(av[1], "-k", 3))
		generate_password();
}
