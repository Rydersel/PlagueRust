#include "authgg.h"
#include "lw_http.hpp"
#include "md5wrapper.h"
#include "print.h"
#include "hwid.h"
#include "xor.h"
#include "crypto.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include "globals/utils.hpp"

std::string expiry;
std::string hwid;
std::string ip;
std::string uservariable;
#include <iostream>
#include <fstream>
using namespace std;
std::string email2;
bool xyz;
bool retry = "1";
bool xyz1;
using namespace std;
c_crypto crypto;
string server(string toEncrypt) {
	char key[3] = { 'T', 'C', 'P' }; // TCP encrypted data, jk.. unless...
	string output = toEncrypt;

	for (int i = 0; i < toEncrypt.size(); i++)
		output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))];

	return output;
}
bool replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
void authgg::GenerateSeed()
{
	try
	{
		c_lw_http	lw_http;
		c_lw_httpd	lw_http_d;
		auto md5 = new md5wrapper();
		if (!lw_http.open_session())
		{
			return;
		}
		std::string s_reply;
		lw_http_d.add_field(xor_a("a"), "securexseed");
		const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/encryption.php", s_reply, lw_http_d);
		std::string s = server(s_reply);
		crypto.entity = server(s_reply);
		std::string delimiter = ":";
		std::vector<std::string> outputArr;
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
			outputArr.push_back(token);
		}
		outputArr.push_back(s);
		crypto.key = outputArr[0].c_str();
		crypto.iv = outputArr[1].c_str();
		crypto.key_enc = crypto.random_string(256);
		lw_http.close_session();
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
}
void authgg::Initialize()
{
	try
	{
		c_lw_http	lw_http;
		c_lw_httpd	lw_http_d;
		auto md5 = new md5wrapper();
		if (!lw_http.open_session())
		{
			return;
		}
		std::string s_reply;
		lw_http_d.add_field(xor_a("a"), "start");
		lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
		lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
		lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
		lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
		lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
		const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
		if (s_reply.find("|") != string::npos)
		{

		}
		else
		{
			authgg::GenerateSeed();
			authgg::Initialize();
			return;
		}
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		if (s == "NOT_PREMIUM")
		{
		}
		std::string delimiter = "|";
		std::vector<std::string> outputArr;
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
			outputArr.push_back(token);
		}
		outputArr.push_back(s);
		std::string status_status = outputArr[0].c_str();
		std::string developermode_status = outputArr[1].c_str();
		std::string hash = outputArr[2].c_str();
		std::string version = outputArr[3].c_str();
		std::string downloadlink = outputArr[4].c_str();
		crypto.freemode_status = outputArr[5].c_str();
		crypto.login_status = outputArr[6].c_str();
		crypto.appname = outputArr[7].c_str();
		crypto.register_status = outputArr[8].c_str();

		if (version != crypto.version)
		{
			MessageBoxA(NULL, "Update Availiable!", "Error!", MB_OK | MB_ICONEXCLAMATION);
			ShellExecute(0, 0, downloadlink.c_str(), 0, 0, SW_SHOW);
			Sleep(2000);
			exit(43);
		}
		if (status_status != "Enabled")
		{
			MessageBoxA(NULL, "Application Disabled!", "INFO!", MB_OK | MB_ICONEXCLAMATION);
			Sleep(2000);
			exit(43);
		}
		lw_http.close_session();
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
}

int authgg::Register(const std::string username, const std::string password, const std::string email, const std::string key)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return 0;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "register");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(username, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(password, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("i"), crypto.encrypt(email, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("j"), crypto.encrypt(key, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(xor_w(L"https://api.auth.gg/v6/api.php"), s_reply, lw_http_d);
	lw_http.close_session();
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (crypto.register_status == "Disabled")
	{
		MessageBoxA(NULL, "Register Disabled!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "success")
	{
		MessageBoxA(NULL, "Key Redeemed Try Logging In!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_token")
	{
		MessageBoxA(NULL, "Key Doesn't Exist!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "email_used")
	{
		MessageBoxA(NULL, "Email Taken!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_username")
	{
		MessageBoxA(NULL, "Username Taken!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	MessageBoxA(NULL, "Something Went Wrong!", "Error!", MB_OK | MB_ICONEXCLAMATION);
	Sleep(2000);
	exit(43);
};
void authgg::autologin(const std::string autouser, const std::string autopass)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "login");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(autouser, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(autopass, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
	if (crypto.login_status == "Disabled")
	{
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		Sleep(2000);
		exit(43);
	}
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (s == "hwid_updated")
	{
		MessageBoxA(NULL, "HWID Updated, please Relaunch!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "time_expired")
	{
		MessageBoxA(NULL, "Your Subscription Has Expired!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_hwid")
	{
		MessageBoxA(NULL, "Invalid HWID!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_details")
	{
		authgg::retryauto(autouser, autopass);
	}
	std::string delimiter = "|";
	std::vector<std::string> outputArr;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		outputArr.push_back(token);
	}
	outputArr.push_back(s);
	std::string login = outputArr[0].c_str();
	hwid = outputArr[1].c_str();
	email2 = outputArr[2].c_str();
	ip = outputArr[4].c_str();
	expiry = outputArr[5].c_str();
	uservariable = outputArr[6].c_str();
	if (login == "success" + crypto.apikey + crypto.aid + ip)
	{
		system("cls");
		std::cout << R"( /$$$$$$$  /$$                                                                                    
| $$__  $$| $$                                                                                    
| $$  \ $$| $$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$  /$$  /$$  /$$$$$$   /$$$$$$   /$$$$$$ 
| $$$$$$$/| $$ |____  $$ /$$__  $$| $$  | $$ /$$__  $$| $$ | $$ | $$ |____  $$ /$$__  $$ /$$__  $$
| $$____/ | $$  /$$$$$$$| $$  \ $$| $$  | $$| $$$$$$$$| $$ | $$ | $$  /$$$$$$$| $$  \__/| $$$$$$$$
| $$      | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$ | $$ | $$ /$$__  $$| $$      | $$_____/
| $$      | $$|  $$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$/$$$$/|  $$$$$$$| $$      |  $$$$$$$
|__/      |__/ \_______/ \____  $$ \______/  \_______/ \_____/\___/  \_______/|__/       \_______/
                         /$$  \ $$                                                                
                        |  $$$$$$/                                                                
                         \______/       
                                                          )" << std::endl;
		print::set_text("retried \n", Blue);
		system("Color 1");
		Sleep(175);
		system("Color 2");
		Sleep(175);
		system("Color 3");
		Sleep(175);
		system("Color 4");
		Sleep(175);
		system("Color 6");
		Sleep(175);//one second
		system("Color 7");
		Sleep(175);
		system("Color 8");
		Sleep(175);
		system("Color 9");

		system("Color 5");
		std::string hello = "Preparing Software...!";
		for (int i = 0; hello[i] != '\0'; i++) {
			if (hello[i] == ' ')
				Sleep(50 + rand() % 150);
			else
				Sleep(30 + rand() % 100);
			std::cout << hello[i];
		}
		print::set_text("   [Done] \n", Green);

		Sleep(100);
		PlaySound(TEXT("Voice.wav"), NULL, SND_FILENAME | SND_ASYNC);
		print::set_text("\nExpiry: ", LightBlue);
		print::set_text(expiry.c_str(), LightBlue);
		print::set_text("\n", LightBlue);
		xyz1 = true;
	}
	if (!b_lw_http)
	{
		return;
	}
	lw_http.close_session();
}
void authgg::Login(const std::string username, const std::string password)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "login");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(username, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(password, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
	if (crypto.login_status == "Disabled")
	{
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		Sleep(2000);
		exit(43);
	}
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (s == "hwid_updated")
	{
		MessageBoxA(NULL, "HWID Updated, please Relaunch!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "time_expired")
	{
		MessageBoxA(NULL, "Your Subscription Has Expired!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_hwid")
	{
		MessageBoxA(NULL, "Invalid HWID!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_details")
	{
		authgg::retrylogin(username, password);
	}
	std::string delimiter = "|";
	std::vector<std::string> outputArr;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		outputArr.push_back(token);
	}
	outputArr.push_back(s);
	std::string login = outputArr[0].c_str();
	hwid = outputArr[1].c_str();
	email2 = outputArr[2].c_str();
	ip = outputArr[4].c_str();
	expiry = outputArr[5].c_str();
	uservariable = outputArr[6].c_str();
	if (login == "success" + crypto.apikey + crypto.aid + ip)
	{
		ofstream myfile("C:\\user-plagueware.txt");
		if (myfile.is_open())
		{
			myfile << username << "\n";
			myfile.close();
		}
		ofstream myfile1("C:\\pass-plagueware.txt");
		if (myfile1.is_open())
		{
			myfile1 << password << "\n";
			myfile1.close();
		}
		else cout << "Sorry Auto login won't be possible at this time.";

		system("cls");
		std::cout << R"( /$$$$$$$  /$$                                                                                    
| $$__  $$| $$                                                                                    
| $$  \ $$| $$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$  /$$  /$$  /$$$$$$   /$$$$$$   /$$$$$$ 
| $$$$$$$/| $$ |____  $$ /$$__  $$| $$  | $$ /$$__  $$| $$ | $$ | $$ |____  $$ /$$__  $$ /$$__  $$
| $$____/ | $$  /$$$$$$$| $$  \ $$| $$  | $$| $$$$$$$$| $$ | $$ | $$  /$$$$$$$| $$  \__/| $$$$$$$$
| $$      | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$ | $$ | $$ /$$__  $$| $$      | $$_____/
| $$      | $$|  $$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$/$$$$/|  $$$$$$$| $$      |  $$$$$$$
|__/      |__/ \_______/ \____  $$ \______/  \_______/ \_____/\___/  \_______/|__/       \_______/
                         /$$  \ $$                                                                
                        |  $$$$$$/                                                                
                         \______/       
                                                          )" << std::endl;
		print::set_text("Login sucessful \n", Blue);
		std::string hello = "Preparing Software...!";
		for (int i = 0; hello[i] != '\0'; i++) {
			if (hello[i] == ' ')
				Sleep(50 + rand() % 150);
			else
				Sleep(30 + rand() % 100);
			std::cout << hello[i];
		}
		system("Color 1");
		Sleep(175);
		system("Color 2");
		Sleep(175);
		system("Color 3");
		Sleep(175);
		system("Color 4");
		Sleep(175);
		system("Color 6");
		Sleep(175);//one second
		system("Color 7");
		Sleep(175);
		system("Color 8");
		Sleep(175);
		system("Color 9");

		system("Color 5");
		print::set_text("   [Done] \n", Green);

		Sleep(100);
		PlaySound(TEXT("Voice.wav"), NULL, SND_FILENAME | SND_ASYNC);
		print::set_text("\nExpiry: ", LightBlue);
		print::set_text(expiry.c_str(), LightBlue);
		print::set_text("\n", LightBlue);
		xyz = true;
	}
	if (!b_lw_http)
	{
		return;
	}
	lw_http.close_session();
}
void authgg::Login2(const std::string username, const std::string password)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "login");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(username, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(password, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
	if (crypto.login_status == "Disabled")
	{
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		Sleep(2000);
		exit(43);
	}
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (s == "hwid_updated")
	{
		MessageBoxA(NULL, "HWID Updated Relaunch!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "time_expired")
	{
		MessageBoxA(NULL, "Subscription Expired!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_hwid")
	{
		MessageBoxA(NULL, "Invalid HWID!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_details")
	{
		authgg::retrylogin(username, password);
	}
	std::string delimiter = "|";
	std::vector<std::string> outputArr;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		outputArr.push_back(token);
	}
	outputArr.push_back(s);
	std::string login = outputArr[0].c_str();
	std::string hwid = outputArr[1].c_str();
	std::string email = outputArr[2].c_str();
	std::string rank = outputArr[3].c_str();
	std::string ip = outputArr[4].c_str();
	std::string expiry = outputArr[5].c_str();
	std::string uservariable = outputArr[6].c_str();
	if (!b_lw_http)
	{
		return;
	}
	lw_http.close_session();
}





// the retry login



void authgg::retrylogin(const std::string username, const std::string password)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "login");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(username, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(password, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
	if (crypto.login_status == "Disabled")
	{
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		Sleep(2000);
		exit(43);
	}
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (s == "hwid_updated")
	{
		MessageBoxA(NULL, "HWID Updated, please Relaunch!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "time_expired")
	{
		MessageBoxA(NULL, "Your Subscription Has Expired!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_hwid")
	{
		MessageBoxA(NULL, "Invalid HWID!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_details")
	{
		MessageBoxA(NULL, "Invalid Login!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	std::string delimiter = "|";
	std::vector<std::string> outputArr;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		outputArr.push_back(token);
	}
	outputArr.push_back(s);
	std::string login = outputArr[0].c_str();
	hwid = outputArr[1].c_str();
	email2 = outputArr[2].c_str();
	ip = outputArr[4].c_str();
	expiry = outputArr[5].c_str();
	uservariable = outputArr[6].c_str();
	if (login == "success" + crypto.apikey + crypto.aid + ip)
	{
		ofstream myfile("C:\\user-plagueware.txt");
		if (myfile.is_open())
		{
			myfile << username << "\n";
			myfile.close();
		}
		ofstream myfile1("C:\\pass-plagueware.txt");
		if (myfile1.is_open())
		{
			myfile1 << password << "\n";
			myfile1.close();
		}
		else cout << "Sorry Auto login won't be possible at this time.";

		system("cls");
		std::cout << R"( /$$$$$$$  /$$                                                                                    
| $$__  $$| $$                                                                                    
| $$  \ $$| $$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$  /$$  /$$  /$$$$$$   /$$$$$$   /$$$$$$ 
| $$$$$$$/| $$ |____  $$ /$$__  $$| $$  | $$ /$$__  $$| $$ | $$ | $$ |____  $$ /$$__  $$ /$$__  $$
| $$____/ | $$  /$$$$$$$| $$  \ $$| $$  | $$| $$$$$$$$| $$ | $$ | $$  /$$$$$$$| $$  \__/| $$$$$$$$
| $$      | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$ | $$ | $$ /$$__  $$| $$      | $$_____/
| $$      | $$|  $$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$/$$$$/|  $$$$$$$| $$      |  $$$$$$$
|__/      |__/ \_______/ \____  $$ \______/  \_______/ \_____/\___/  \_______/|__/       \_______/
                         /$$  \ $$                                                                
                        |  $$$$$$/                                                                
                         \______/       
                                                          )" << std::endl;
		print::set_text("Login sucessful \n", Blue);
		std::string hello = "Preparing Software...!";
		for (int i = 0; hello[i] != '\0'; i++) {
			if (hello[i] == ' ')
				Sleep(50 + rand() % 150);
			else
				Sleep(30 + rand() % 100);
			std::cout << hello[i];
		}
		system("Color 1");
		Sleep(175);
		system("Color 2");
		Sleep(175);
		system("Color 3");
		Sleep(175);
		system("Color 4");
		Sleep(175);
		system("Color 6");
		Sleep(175);//one second
		system("Color 7");
		Sleep(175);
		system("Color 8");
		Sleep(175);
		system("Color 9");

		system("Color 5");
		print::set_text("   [Done] \n", Green);

		Sleep(100);
		PlaySound(TEXT("Voice.wav"), NULL, SND_FILENAME | SND_ASYNC);
		print::set_text("\nExpiry: ", LightBlue);
		print::set_text(expiry.c_str(), LightBlue);
		print::set_text("\n", LightBlue);
		xyz = true;
	}
	if (!b_lw_http)
	{
		return;
	}
	lw_http.close_session();
}


void authgg::retryauto(const std::string autouser, const std::string autopass)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), "login");
	lw_http_d.add_field(xor_a("b"), crypto.encrypt(crypto.aid, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("c"), crypto.encrypt(crypto.secret, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("d"), crypto.encrypt(crypto.apikey, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("g"), crypto.encrypt(autouser, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("h"), crypto.encrypt(autopass, crypto.key, crypto.iv).c_str());
	lw_http_d.add_field(xor_a("k"), md5->getHashFromString(hwid::get_hardware_id("1")).c_str());
	lw_http_d.add_field(xor_a("e"), crypto.entity.c_str());
	lw_http_d.add_field(xor_a("seed"), crypto.key_enc.c_str());
	const auto b_lw_http = lw_http.post(L"https://api.auth.gg/v6/api.php", s_reply, lw_http_d);
	if (crypto.login_status == "Disabled")
	{
	{
		std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
		Sleep(2000);
		exit(43);
	}
	std::string s(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	if (s == "hwid_updated")
	{
		MessageBoxA(NULL, "HWID Updated, please Relaunch!", "Success!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "time_expired")
	{
		MessageBoxA(NULL, "Your Subscription Has Expired!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_hwid")
	{
		MessageBoxA(NULL, "Invalid HWID!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	if (s == "invalid_details")
	{
		MessageBoxA(NULL, "Invalid Login!", "Error!", MB_OK | MB_ICONEXCLAMATION);
		Sleep(2000);
		exit(43);
	}
	std::string delimiter = "|";
	std::vector<std::string> outputArr;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		outputArr.push_back(token);
	}
	outputArr.push_back(s);
	std::string login = outputArr[0].c_str();
	hwid = outputArr[1].c_str();
	email2 = outputArr[2].c_str();
	ip = outputArr[4].c_str();
	expiry = outputArr[5].c_str();
	uservariable = outputArr[6].c_str();
	if (login == "success" + crypto.apikey + crypto.aid + ip)
	{
		system("cls");
		std::cout << R"( /$$$$$$$  /$$                                                                                    
| $$__  $$| $$                                                                                    
| $$  \ $$| $$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$  /$$  /$$  /$$$$$$   /$$$$$$   /$$$$$$ 
| $$$$$$$/| $$ |____  $$ /$$__  $$| $$  | $$ /$$__  $$| $$ | $$ | $$ |____  $$ /$$__  $$ /$$__  $$
| $$____/ | $$  /$$$$$$$| $$  \ $$| $$  | $$| $$$$$$$$| $$ | $$ | $$  /$$$$$$$| $$  \__/| $$$$$$$$
| $$      | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$ | $$ | $$ /$$__  $$| $$      | $$_____/
| $$      | $$|  $$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$/$$$$/|  $$$$$$$| $$      |  $$$$$$$
|__/      |__/ \_______/ \____  $$ \______/  \_______/ \_____/\___/  \_______/|__/       \_______/
                         /$$  \ $$                                                                
                        |  $$$$$$/                                                                
                         \______/       
                                                          )" << std::endl;

		system("Color 1");
		Sleep(175);
		system("Color 2");
		Sleep(175);
		system("Color 3");
		Sleep(175);
		system("Color 4");
		Sleep(175);
		system("Color 6");
		Sleep(175);//one second
		system("Color 7");
		Sleep(175);
		system("Color 8");
		Sleep(175);
		system("Color 9");

		system("Color 5");
		std::string hello = "Preparing Software...!";
		for (int i = 0; hello[i] != '\0'; i++) {
			if (hello[i] == ' ')
				Sleep(50 + rand() % 150);
			else
				Sleep(30 + rand() % 100);
			std::cout << hello[i];
		}
		print::set_text("   [Done] \n", Green);

		Sleep(100);
		PlaySound(TEXT("Voice.wav"), NULL, SND_FILENAME | SND_ASYNC);
		print::set_text("\nExpiry: ", LightBlue);
		print::set_text(expiry.c_str(), LightBlue);
		print::set_text("\n", LightBlue);
		xyz1 = true;
	}
	if (!b_lw_http)
	{
		return;
	}
	lw_http.close_session();
}