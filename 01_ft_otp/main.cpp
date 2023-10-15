/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/15 20:15:13 by ccottin          ###   ########.fr       */
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
			std::cerr << "Usage : ./ft_otp [-g key] or [-k password file]";
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

long int		time_steps_calculator(void)
{
	time_t	current_time;
	long int		time_ref;
	int		x;

	if (TIME_STEPS == 0)
		ft_error(5);
	if (time(&current_time) == -1)
		ft_error(4);						//we get time since t0, the begening of
	x = 60 / TIME_STEPS;					//Unix time, then we divide it by time
	time_ref = current_time / x;			//steps per seconds and truncate the result to get
	return (time_ref);						//the moving factor 'C' (or 'T')

}

std::string	call_SHA_1(const char *to_hash)
{
    unsigned char	hashed[SHA_DIGEST_LENGTH];

	//this is chatgpts's so if something doenst work try a differnet cast
    SHA1(reinterpret_cast<const unsigned char*>(to_hash), std::strlen(to_hash), hashed);
	std::string ret(reinterpret_cast<char*>(hashed));
	return (ret);
}

void		hmac_calculator(std::string key, long int time_ref)
{
	unsigned char	ipad[65];				//create 2 string with fixed values to
	unsigned char	opad[65];				//apply a XOR operation on the hashed output
	unsigned char	key_c[65];				//later on.
											//ipad = in padding outpad = outpadding
	memset(ipad, 0x36, 64);					//this algo helps with eccficiency issues
	memset(opad, 0x5c, 64);					//by precomputing hashed keys, making the auth
	memset(key_c, 0, 64);					//check faster for tres demandes servers
	ipad[64] = 0;
	opad[64] = 0;

	std::cout << key << time_ref;	
	//first we need the key to be no longer than the block of data B for SHA = 64
	if (key.length() > 64)
		key = call_SHA_1(key.c_str());
	std::cout << SHA_DIGEST_LENGTH;
	//we should print the key in a bloc of memory B-sized to XOR it
	memcpy(key_c, key.c_str(), key.length());	
	//then we'll do SHA(Key XOR opad, (SHA(key XOR ipad, time))) step by step
	//std::cout << key + time_ref;	
		//find a way to add the value of time_ref at the end of the key
}

void		hotp_calculator(std::string key, long int time_ref)
{
	hmac_calculator(key, time_ref);

}

void		generate_password(void)
{
	std::string	key = file_manager("ft_otp.key", std::ios_base::in, NULL);
	if (key.empty())
		ft_error(3);
	hotp_calculator(key, time_steps_calculator());
}

void		stock_new_key(std::string key)
{
	unsigned int		i;

	i = 0;
	while (std::isxdigit(key[i]))
		++i;
	if (i != key.length() || i < 64)
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
