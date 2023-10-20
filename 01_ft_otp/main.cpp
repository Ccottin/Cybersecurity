/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccottin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:43:44 by ccottin           #+#    #+#             */
/*   Updated: 2023/10/20 15:03:18 by ccottin          ###   ########.fr       */
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

int				time_steps_calculator(void)
{
	time_t	current_time;
	int		time_ref;

	if (TIME_STEPS == 0)
		ft_error(5);
	if (time(&current_time) == -1)
		ft_error(4);						//we get time since t0, the begening of
											//Unix time, then we divide it by time
	time_ref = current_time / TIME_STEPS;	//steps per seconds and truncate the result to get
	return (time_ref);						//the moving factor 'C' (or 'T')

}

void		num_to_string(int number, unsigned char *ret)
{
//	for (unsigned int i = sizeof(int); i > 0;)
	for (unsigned int i = 0; i < sizeof(int); i++)
	{
		ret[i] = number;
//		ret[--i] = number;
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

void		hmac_calculator(std::string key, int time_ref, unsigned char *ret)
{
	unsigned char	ipad[65 + sizeof(int)];				//create 2 string with fixed values to
	unsigned char	opad[65 * 2 + sizeof(int)];			//apply a XOR operation on the hashed output
	unsigned char	key_c[65];							//later on.
	unsigned char	time_str[sizeof(int) + 1];			//ipad = inner padding opad = outer
	unsigned char	hashed[SHA_DIGEST_LENGTH + 1];		//this algo helps with eccficiency issues
													//by precomputing hashed keys, making the auth
	memset(key_c, 0, 65);							//check faster for tres demandes servers
	memset(ipad, 0, 65 + sizeof(int));	
	memset(opad, 0, 65 * 2 + sizeof(int));	

	std::cout << "time = " << time_ref << std::endl;
	//first we need the key to be no longer than the block of data B for SHA = 64
	if (key.length() > 64)
		memcpy(key_c, call_SHA_1((const unsigned char*)key.c_str(), hashed), 20);
	else
		memcpy(key_c, key.c_str(), key.length());

	//then we'll do SHA(Key XOR opad, (SHA(key XOR ipad, time))) step by step
	//we copy the key in strings we'll be padding with fixed values
	memcpy(ipad, key_c, 65);
	memcpy(opad, key_c, 65);
	for (int i = 0; i < 64; i++) {
                ipad[i] ^= 0x36;
                opad[i] ^= 0x5c;
        }
	//transfert the value of time to string to patch it at the end
	num_to_string(time_ref, time_str);
	std::cout << "time = ";
	for (unsigned int i = 0; i < 4; i++)
		printf("%d", time_str[i]);
	std::cout << "\n";
	memcpy(&ipad[strlen((char*)ipad)], time_str, strlen((char*)time_str));
	//hash this first value 
	call_SHA_1((unsigned char*)ipad, hashed);
	//happend hashed to the end of opad and hash this again
	memcpy(&opad[strlen((char*)opad)], hashed, strlen((char*)hashed));
	call_SHA_1((unsigned char*)opad, hashed);
	//transfer the value to the return string
	memcpy(ret, hashed, strlen((char*)hashed));
	std::cout << "1hmac = ";
	for (unsigned int i = 0; i < std::strlen((char*)ret); i++)
		printf("%x", ret[i]);
	std::cout << std::endl;
}


void		hotp_calculator(std::string key,long int time_ref)
{
	unsigned char	hmac_string[SHA_DIGEST_LENGTH + 1];
	unsigned char	hmac1_string[SHA_DIGEST_LENGTH + 1];
	int				offset;
	int				bin_code;

	char	number[5];
	number[4] = 0;
	num_to_string(time_ref,(unsigned char *)number);

	memset(hmac_string, 0, SHA_DIGEST_LENGTH + 1);
	memset(hmac1_string, 0, SHA_DIGEST_LENGTH + 1);

	hmac_calculator(key, time_ref, hmac1_string);
	std::cout << "time = " << time_ref << std::endl;

	HMAC(EVP_sha1(), key.c_str(), key.length(), (unsigned char*)&time_ref,
		8, (unsigned char*)hmac_string, NULL);
	
	std::cout << "hmac = ";
	for (unsigned int i = 0; i < std::strlen((char*)hmac_string); i++)
		printf("%x", hmac_string[i]);
	std::cout << std::endl;


	//get the lower 4 bits and convert them to int, that shloud contain result 
	//between 0 & 15
	offset = hmac_string[19] & 0xf;
	//then we get bytes starting at offset, but we mask the first one to avoid
	//signed-unsigned convertion problem
	bin_code = (hmac_string[offset]  & 0x7f) << 24
           | (hmac_string[offset+1] & 0xff) << 16
           | (hmac_string[offset+2] & 0xff) <<  8
           | (hmac_string[offset+3] & 0xff) ;
	
	bin_code %= 1000000;
	std::cout << bin_code << std::endl;
	offset = hmac1_string[19] & 0xf;
	//then we get bytes starting at offset, but we mask the first one to avoid
	//signed-unsigned convertion problem
	bin_code = (hmac1_string[offset]  & 0x7f) << 24
           | (hmac1_string[offset+1] & 0xff) << 16
           | (hmac1_string[offset+2] & 0xff) <<  8
           | (hmac1_string[offset+3] & 0xff) ;
	
	bin_code %= 1000000;
	std::cout << "deux" << bin_code << std::endl;

}


void		generate_password(void)
{
//	unsigned int		i;
	
	std::string	key = file_manager("ft_otp.key", std::ios_base::in, NULL);
	if (key.empty())
		ft_error(3);
/*	i = 0;
	while (std::isxdigit(key[i]))
		++i;
	if (i != key.length() || i < 64)
		ft_error(2);
*/
	unsigned char binaryKey[1024];
	int binaryKeyLength = EVP_DecodeBlock(binaryKey, (const unsigned char*)key.c_str(), key.length());

    // Encode the binary key to base32
    char base32Key[1024];
    int base32KeyLength = EVP_EncodeBlock((unsigned char*)base32Key, binaryKey, binaryKeyLength);

    // Null-terminate the base32Key
    base32Key[base32KeyLength] = '\0';

	std::cout << base32Key << std::endl;
	hotp_calculator(key, time_steps_calculator());
	std::cout << "base32" << std::endl;
	hotp_calculator(base32Key, time_steps_calculator());
}

void		stock_new_key(char  *in_file)
{
	std::string			key;
	unsigned int		i;
	
	key = file_manager(in_file, std::ios_base::in, NULL);
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
