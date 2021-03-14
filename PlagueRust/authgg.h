#pragma once
#include <string.h>
#include <string>

class authgg
{
public:
	static void GenerateSeed();
	static void Initialize();
	static int Register(const std::string username, const std::string password, const std::string email, const std::string key);
	static void Login(const std::string username, const std::string password);
	static void retrylogin(const std::string username, const std::string password);
	static void autologin(const std::string autouser, const std::string autopass);
	static void retryauto(const std::string autouser, const std::string autopass);
	static void Login2(const std::string username, const std::string password);
};