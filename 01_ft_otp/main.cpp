/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/18 21:13:23 by ccottin          ###   ########.fr       */
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

int				time_steps_calculator(void)
{
	time_t	current_time;
	int		time_ref;
	int		x;

	if (TIME_STEPS == 0)
		ft_error(5);
	if (time(&current_time) == -1)
		ft_error(4);						//we get time since t0, the begening of
	x = 60 / TIME_STEPS;					//Unix time, then we divide it by time
	time_ref = current_time / x;			//steps per seconds and truncate the result to get
	return (time_ref);						//the moving factor 'C' (or 'T')

}

void		num_to_string(int number, unsigned char *ret)
{
	//recheck this in order to make the string fits perfectly w/ this one and the concat
	for (unsigned int i = sizeof(int); i > 0; --i)
	{
		ret[i] = number;
		printf("%d, %d\n", ret[i], i);
		number >>= 8;
	}
	ret[sizeof(int)] = 0;
}

unsigned char	*call_SHA_1(const unsigned char *to_hash, unsigned char *hashed)
{
    SHA1(to_hash, strlen((const char *)to_hash), hashed);
	hashed[SHA_DIGEST_LENGTH] = 0;
	return (hashed);
}

void		hmac_calculator(std::string key, int time_ref)
{
	unsigned char	ipad[65 + sizeof(int)];				//create 2 string with fixed values to
	unsigned char	opad[65];							//apply a XOR operation on the hashed output
	unsigned char	key_c[65];							//later on.
	unsigned char	time_str[sizeof(int) + 1];			//ipad = inner padding opad = outer
	unsigned char	hashed[SHA_DIGEST_LENGTH + 1];		//this algo helps with eccficiency issues
													//by precomputing hashed keys, making the auth
	memset(key_c, 0, 65);							//check faster for tres demandes servers
	memset(key_c, 0, 65);							//check faster for tres demandes servers
	memset(ipad, 0, 65 + sizeof(int));	
	
	std::cout << key << "=========" << time_ref << "=======" << std::endl;	
	//first we need the key to be no longer than the block of data B for SHA = 64
	if (key.length() > 64)
		memcpy(key_c, call_SHA_1((const unsigned char*)key.c_str(), hashed), 20);
	//we should print the key in a bloc of memory B-sized to XOR it
	else
		memcpy(key_c, key.c_str(), key.length());	
	//then we'll do SHA(Key XOR opad, (SHA(key XOR ipad, time))) step by step

	//	std::cout << key_c << "=========" << time_ref << "=======" << std::endl;

	memcpy(ipad, key_c, 65);	
	memcpy(opad, key_c, 65);	
	for (int i = 0; i < 64; i++) {
                ipad[i] ^= 0x36;
                opad[i] ^= 0x5c;
        }
	for (unsigned int i = 0; i < strlen((char*)ipad); i++)
		printf("%d = %d\n", i, ipad[i]);

	num_to_string(time_ref, time_str);
	for (unsigned int i = 0; i < strlen((char*)ipad); i++)
		printf("%d = %d\n", i, time_str[i]);

	//im tired and i cannot make this works
	memcpy(&ipad[strlen((char*)ipad)], time_str, strlen((char*)time_str));

	for (unsigned int i = 0; i < strlen((char*)ipad); i++)
		printf("%d = %d\n", i, ipad[i]);
	call_SHA_1((unsigned char*)ipad, hashed);

	for (unsigned int i = 0; i < strlen((char*)hashed); i++)
		printf("%d = %d\n", i, hashed[i]);

	
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
