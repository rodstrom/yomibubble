#ifndef _STRING_CONVERTER_H
#define _STRING_CONVERTER_H

/*
  * StringConverter class
  * Converts values (used in ConfigFile)
  * Written by Jens Berglind
  * Last modified 20-11-2012
*/

#include <string>
#include <vector>

class StringConverter
{
public:
	static bool convertToBool(const std::string& value);
	static int convertToInt(const std::string& value);
	static float convertToFloat(const std::string& value);
	static std::vector<std::string> convertToStringVector(const std::string& value, const std::string& delimiter = "\t\n ");

	static std::string toString(bool value, bool getAsYesNo);
	static std::string toString(int value, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
	static std::string toString(float value, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

	static void toLowerCase(std::string& value);
	static void toUpperCase(std::string& value);

	static bool isNumber(const std::string& string);

private:
	StringConverter(void);
	~StringConverter(void);
};

#endif	// _STRING_CONVERTER_H