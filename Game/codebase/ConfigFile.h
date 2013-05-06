#ifndef _CONFIG_FILE_H
#define _CONFIG_FILE_H

/*
  * ConfigFile class
  * Reads and writes from a text file
  * Written by Jens Berglind
  * Last modified 29-11-2012
*/

#include <string>
#include <vector>
#include <map>

class Config
{
public:
	Config(const std::string& filename);
	~Config();

	bool parse(); // Returns true if success.
	void save();

	const std::string& getFilename(void) const {return(_filename);}

	bool isEmpty(void) {return(_keys.empty());}
	bool doesKeyExist(const std::string& key);

	void setKey(const std::string& key, const std::string& value); // If the key does not exists, it will be created.

	std::string getAsString(const std::string& key);
	bool getAsBool(const std::string& key);
	int getAsInt(const std::string& key);
	float getAsFloat(const std::string& key);
	std::vector<std::string> getAsStringVector(const std::string& key);

private:
	std::string _filename;
	std::string _separators;
	std::map<std::string, std::string> _keys;
};

#endif	// _CONFIG_FILE_H