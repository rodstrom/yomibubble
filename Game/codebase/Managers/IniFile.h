//Copyright (c) 2012 Olof Wallentin  <olov.wallentin@gmail.com>

//This software is provided 'as-is', without any express or implied
//warranty. In no event will the authors be held liable for any damages
//arising from the use of this software.

//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions:

   //1. The origin of this software must not be misrepresented; you must not
   //claim that you wrote the original software. If you use this software
   //in a product, an acknowledgment in the product documentation would be
   //appreciated but is not required.

   //2. Altered source versions must be plainly marked as such, and must not be
   //misrepresented as being the original software.

   //3. This notice may not be removed or altered from any source
   //distribution.

#ifndef _INIFILE_H
#define _INIFILE_H

#include <map>
#include <string>


typedef std::map<std::string, std::map<std::string, std::string>> m_sm_ss;
typedef std::map<std::string, std::string> m_ss;

class IniFile
{
public:
	IniFile(const std::string& filepath);
	virtual ~IniFile(void);
	const std::string& getValue(const std::string& group, const std::string& key, const std::string& defaultValue = " ");	// Get value as string
	float getValueAsFloat(const std::string& group, const std::string& key, float defaultValue = 0.0f);	// get the value as a float
	int getValueAsInt(const std::string& group, const std::string& key, int defaultValue = 0);	// get the value as an int

	void addModify(const std::string& group, const std::string& key, const std::string& value);	// when you want to change values
	void save();	// run after addModify() to save changes and write to file
	bool parse();	// read data. this must be run or else you will have zero data in your map

private:
	void cleanString(std::string& line);	// removes white spaces from the string

	m_sm_ss _options;
	std::string _filepath;
};

#endif // _INIFILE_H

