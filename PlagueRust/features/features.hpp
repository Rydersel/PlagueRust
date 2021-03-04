
#ifndef _FEATURES_HPP
#define _FEATURES_HPP
#include <map>
#include <string>

class Features
{
public:
	std::map<std::string, bool> hooked;
public:
	/* Features Thread */
	void HandleFeatures();
};

extern Features* features;
#endif // _FEATURES_HPP