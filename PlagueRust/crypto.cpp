#include <Windows.h>
#include "crypto.h"
#include "xor.h"

std::string c_crypto::encrypt(const std::string str_in, const std::string key, const std::string iv)
{
	std::string str_out;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());

	CryptoPP::StringSource encryptor(str_in, true,
		new CryptoPP::StreamTransformationFilter(encryption,
			new CryptoPP::Base64Encoder(
				new CryptoPP::StringSink(str_out),
				false // do not append a newline
			)
		)
	);
	return str_out;
}

std::string c_crypto::decrypt(const std::string str_in, const std::string key, const std::string iv)
{
	std::string str_out;

	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());

	CryptoPP::StringSource decryptor(str_in, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StreamTransformationFilter(decryption,
				new CryptoPP::StringSink(str_out)
			)
		)
	);
	return str_out;
}

std::string c_crypto:: xor (std::string value, std::string key)
{
	std::string text = "";
	DWORD k = 0;
	DWORD v = 0;
	DWORD klen = key.length();
	DWORD vlen = value.length();
	for (DWORD v = 0; v < vlen; v++)
	{
		text += value[v] ^ key[k];
		k = (++k < klen ? k : 0);
	}
	return text;
}

std::vector<unsigned char> c_crypto::encrypt_decrypt_data(std::vector<unsigned char> data, std::vector<unsigned char> key)
{
	int n1 = 11;
	int n2 = 13;
	int ns = 257;
	for (int I = 0; I <= key.size() - 1; I++)
	{
		ns += ns % (key[I] + 1);
	}
	std::vector<unsigned char> out(data.size());
	for (int I = 0; I <= data.size() - 1; I++)
	{
		ns = key[I % key.size()] + ns;
		n1 = (ns + 5) * (n1 & 255) + (n1 >> 8);
		n2 = (ns + 7) * (n2 & 255) + (n2 >> 8);
		ns = ((n1 << 8) + n2) & 255;

		out[I] = static_cast<unsigned char>(data[I] ^ static_cast<unsigned char>(ns));
	}
	return out;
}

std::string c_crypto::random_string(int len)
{
	static const std::string allowed_chars{ "ABCDFGHJKLMNPQRSTVWXZbcdfghjklmnpqrstvwxz" };
	static thread_local std::default_random_engine randomEngine(std::random_device{}());
	static thread_local std::uniform_int_distribution<int> randomDistribution(0, allowed_chars.size() - 1);
	std::string id(len ? len : 32, '\0');
	for (std::string::value_type& c : id) {
		c = allowed_chars[randomDistribution(randomEngine)];
	}
	return id;
}
std::string c_crypto::random_num(int len)
{
	static const std::string allowed_chars{ "0123456789abcdef" };
	static thread_local std::default_random_engine randomEngine(std::random_device{}());
	static thread_local std::uniform_int_distribution<int> randomDistribution(0, allowed_chars.size() - 1);
	std::string id(len ? len : 32, '\0');
	for (std::string::value_type& c : id) {
		c = allowed_chars[randomDistribution(randomEngine)];
	}
	return id;
}
std::string c_crypto::random_string2(int len)
{
	static const std::string allowed_chars{ "¯ﬁ¬Å3ö∆xIÚ™m'+ˇÕ„" };
	static thread_local std::default_random_engine randomEngine(std::random_device{}());
	static thread_local std::uniform_int_distribution<int> randomDistribution(0, allowed_chars.size() - 1);
	std::string id(len ? len : 32, '\0');
	for (std::string::value_type& c : id) {
		c = allowed_chars[randomDistribution(randomEngine)];
	}
	return id;
}