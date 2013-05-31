#include "stdafx.h"
#include "IniFile.h"
#include <fstream>
#include <iostream>

IniFile::IniFile(const std::string& filepath)
{
	_filepath = filepath;
}

IniFile::~IniFile(void){}

void IniFile::addModify(const std::string& group, const std::string& key, const std::string& value)
{
	m_sm_ss::iterator it = _options.find(group);	// search the file for a matching group.
	if (it == _options.end())
	{
		_options.insert(make_pair(group, m_ss()));
		_options[group].insert(make_pair(key, value));
		return;
	}

	m_ss::iterator it2 = _options[group].find(key);	// if the group is NOT missing, search the group for the key
	if (it2 ==_options[group].end())
	{
		_options[group].insert(make_pair(key, value));	// if the key is missing, add it and the value
	}
	else
	{
		_options[group][key] = value;		// finally, if the group AND the key was found, alter the value accordingly
	}
}

bool IniFile::parse()
{
	std::ifstream file(_filepath, std::ios::in);
	if (file.is_open())
	{
		std::string line = "";
		std::string currentGroup = "";
		while(!file.eof())
		{
			getline(file, line);
			if (line != "")
			{
				cleanString(line);		// remove white spaces if any
				if (line[0] == '/')
				{
					continue;	// skip reading if the line is marked as a "comment"
				}
				
				if (line[0] == '[' && line[line.length() - 1] == ']')
				{
					currentGroup = line.substr(1, line.length() - 2);
					_options.insert(make_pair(currentGroup, m_ss()));
				}
				else
				{
					std::string key, value = "";
					size_t find = line.find_first_of("=");
					key = line.substr(0, find);
					value = line.substr(find + 1, line.length() - find);
					_options.insert(make_pair(currentGroup, m_ss()));
					_options[currentGroup].insert(make_pair(key, value));
				}
			}
		}
	}
	else
	{
		std::cout << "ERROR: failed to open file: " + _filepath << std::endl;
		return false;
	}

	// TODO: add verification of config file

	file.close();
	return true;
}

void IniFile::save()
{
	std::ofstream file(_filepath);
	if (!file)
	{
		std::cout << "ERROR: failed to save file: " + _filepath << std::endl;
		return;
	}


	for (auto it = _options.begin(); it != _options.end(); it++)
	{
		std::string currentGroup = it->first;
		file << "[" << currentGroup << "]" << std::endl;
		for (auto it2 = _options[currentGroup].begin(); it2 != _options[currentGroup].end(); it2++)
		{
			file << it2->first << "=" << it2->second << std::endl;
		}
	}
	file.close();
}

const std::string& IniFile::getValue(const std::string& group, const std::string& key, const std::string& defaultValue)
{
	m_sm_ss::iterator it = _options.find(group);
	if (it == _options.end())
	{
		std::cerr << "WARNING: group: " + group + " does not exist in " + _filepath << std::endl;
		return defaultValue;
	}

	m_ss::iterator it2 = _options[group].find(key);
	if (it2 == _options[group].end())
	{
		std::cerr << "WARNING: key: " + key + " does not exist in group " + group + " " + _filepath << std::endl;
		return defaultValue;
	}
	return _options[group][key];
}

float IniFile::getValueAsFloat(const std::string& group, const std::string& key, float defaultValue)
{
	m_sm_ss::iterator it = _options.find(group);
	if (it == _options.end())
	{
		std::cerr << "WARNING: group: " + group + " does not exist in " + _filepath << std::endl;
		return defaultValue;
	}

	m_ss::iterator it2 = _options[group].find(key);
	if (it2 == _options[group].end())
	{
		std::cerr << "WARNING: key: " + key + " does not exist in group " + group + " " + _filepath << std::endl;
		return defaultValue;
	}
	return StringToNumber<float>(_options[group][key]);
}

int IniFile::getValueAsInt(const std::string& group, const std::string& key, int defaultValue)
{
	m_sm_ss::iterator it = _options.find(group);
	if (it == _options.end())
	{
		std::cerr << "WARNING: group: " + group + " does not exist in " + _filepath << std::endl;
		return defaultValue;
	}

	m_ss::iterator it2 = _options[group].find(key);
	if (it2 == _options[group].end())
	{
		std::cerr << "WARNING: key: " + key + " does not exist in group " + group + " " + _filepath << std::endl;
		return defaultValue;
	}
	return StringToNumber<int>(_options[group][key]);
}

bool IniFile::getValueAsBool(const std::string& group, const std::string& key, bool defaultValue){
	m_sm_ss::iterator it = _options.find(group);
	if (it == _options.end())
	{
		std::cerr << "WARNING: group: " + group + " does not exist in " + _filepath << std::endl;
		return defaultValue;
	}

	m_ss::iterator it2 = _options[group].find(key);
	if (it2 == _options[group].end())
	{
		std::cerr << "WARNING: key: " + key + " does not exist in group " + group + " " + _filepath << std::endl;
		return defaultValue;
	}
	return StringToBool(_options[group][key]);
}

void IniFile::cleanString(std::string& line)
{
	for (unsigned int i = 0; i < line.length(); i++)
	{
		if (line[i] == ' ')
		{
			line.erase(i, 1);
			i--;
		}
	}
}

