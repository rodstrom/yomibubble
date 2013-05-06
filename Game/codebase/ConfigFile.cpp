#include "stdafx.h"
#include "ConfigFile.h"
#include "StringConverter.h"

#include <fstream>

Config::Config(const std::string& filename) : _filename(filename), _separators("=:\t")
{
}

Config::~Config()
{
}

bool Config::parse()
{
	if(!_keys.empty())
	{
		_keys.clear();
	}

	std::ifstream file;
	file.open(_filename.c_str(), std::ios::in | std::ios::binary);
	if(file.bad())
	{
		file.close();
		return(false);
	}
	else
	{
		std::string line;
		while(!file.eof())
		{
			std::getline(file, line, '\n');

			std::string::size_type strip = line.find_last_not_of(" \t\n\r");
			if(strip != std::string::npos)
			{
				line = line.substr(0, strip + 1);
			}

			std::string::size_type separatorPos = line.find_first_of(_separators, 0);
			if(separatorPos != std::string::npos)
			{
				std::string key = line.substr(0, separatorPos);
				std::string::size_type nonSeparator = line.find_first_not_of(_separators, separatorPos);
				if(nonSeparator != std::string::npos)
				{
					_keys[key] = line.substr(nonSeparator);
				}
			}
		}
		file.close();
	}
	return(true);
}

void Config::save()
{
	std::map<std::string, std::string>::iterator it;
	std::ofstream file(_filename.c_str());

	for(it = _keys.begin(); it != _keys.end(); it++)
	{
		file << it->first << "=" << it->second << std::endl;
	}

	file.close();
}

bool Config::doesKeyExist(const std::string &key)
{
	if(!_keys.empty())
	{
		std::map<std::string, std::string>::iterator it = _keys.find(key);
		if(it != _keys.end())
		{
			return(true);
		}
	}
	return(false);
}

void Config::setKey(const std::string &key, const std::string &value)
{
	_keys[key] = value;
}

bool Config::getAsBool(const std::string& key)
{
	bool value = false;
	if(!_keys.empty())
	{
		std::map<std::string, std::string>::iterator it = _keys.find(key);
		if(it != _keys.end())
		{
			value = StringConverter::convertToBool(it->second);
		}
	}
	return(value);
}

int Config::getAsInt(const std::string& key)
{
	int value = 0;
	if(!_keys.empty())
	{
		std::map<std::string, std::string>::iterator it = _keys.find(key);
		if(it != _keys.end())
		{
			value = StringConverter::convertToInt(it->second);
		}
	}
	return(value);
}

float Config::getAsFloat(const std::string& key)
{
	float value = 0.0;
	if(!_keys.empty())
	{
		std::map<std::string, std::string>::iterator it = _keys.find(key);
		if(it != _keys.end())
		{
			value = StringConverter::convertToFloat(it->second);
		}
	}
	return(value);
}

std::vector<std::string> Config::getAsStringVector(const std::string &key)
{
	std::vector<std::string> list;

	if(!_keys.empty())
	{
		std::map<std::string, std::string>::iterator it = _keys.find(key);
		if(it != _keys.end())
		{
			list = StringConverter::convertToStringVector(it->second);
		}
	}

	return(list);
}

std::string Config::getAsString(const std::string& key)
{
	if (_keys.empty()) { return ""; }
	std::map<std::string, std::string>::iterator it = _keys.find(key);

	if (it != _keys.end())
	{
		return it->second; //first = nyckeln, second = datat
	}
	return "";
}