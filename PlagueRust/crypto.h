#pragma once
#include <winsvc.h>
#pragma warning (disable: 4191 4099)
#pragma comment (lib, "cryptlib.lib")
#include "./crypto/aes.h"
#include "./crypto/modes.h"
#include "./crypto/base64.h"
#include "./crypto/hex.h"
#include <iosfwd>
#include "crypto/stdcpp.h"
#include <algorithm>
#include <winioctl.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include <random>

class c_crypto
{
public:
	static std::string encrypt(const std::string str_in, const std::string key, const std::string iv);
	static std::string decrypt(const std::string str_in, const std::string key, const std::string iv);
	static std::string xor (std::string value, std::string key);
	static std::vector<unsigned char> encrypt_decrypt_data(std::vector<unsigned char> data, std::vector<unsigned char> key);
	static std::string random_string(int len);
	static std::string random_num(int len);
	static std::string random_string2(int len);
	std::string key_enc;
	std::string key;
	std::string entity;
	std::string iv;
	std::string aid;
	std::string secret;
	std::string apikey;
	std::string version;
	std::string appname;
	std::string login_status;
	std::string register_status;
	std::string freemode_status;
private:
};
