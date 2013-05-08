#include "stdafx.h"
#include "StringConverter.h"

#include <algorithm>
#include <sstream>

StringConverter::StringConverter(void)
{
}

StringConverter::~StringConverter(void)
{
}

bool StringConverter::convertToBool(const std::string& value)
{
	if(value == "1" || value == "true" || value == "yes")
	{
		return(true);
	}
	return(false);
}

int StringConverter::convertToInt(const std::string& value)
{
	int returnValue = 0;

	std::istringstream stream(value);
	stream >> returnValue;

	return(returnValue);
}

float StringConverter::convertToFloat(const std::string &value)
{
	float returnValue = 0.0;

	std::istringstream stream(value);
	stream >> returnValue;

	return(returnValue);
}

std::vector<std::string> StringConverter::convertToStringVector(const std::string& value, const std::string& delimiter /* = "\t\n "*/)
{
	std::vector<std::string> values;

	size_t startPos = 0;
	size_t currentPos = 0;

	do 
	{
		currentPos = value.find_first_of(delimiter, startPos);
		if(currentPos == startPos)
		{
			// We skip the delimiter.
			startPos = currentPos + 1;
		}
		else if(currentPos == std::string::npos)
		{
			// Copying the rest of the string as there are no more delimiters.
			values.push_back(value.substr(startPos));
			break;
		}
		else
		{
			values.push_back(value.substr(startPos, currentPos - startPos));
			startPos = currentPos + 1;
		}

		startPos = value.find_first_not_of(delimiter, startPos);

	} while(currentPos != std::string::npos);

	return(values);
}

std::string StringConverter::toString(bool value, bool getAsYesNo)
{
	if(value == true)
	{
		if(getAsYesNo == true)
		{
			return("yes");
		}
		else
		{
			return("true");
		}
	}
	else
	{
		if(getAsYesNo == true)
		{
			return("no");
		}
		else
		{
			return("false");
		}
	}
}

std::string StringConverter::toString(int value, unsigned short width /*= 0*/, char fill /*= ' '*/, std::ios::fmtflags flags /*= std::ios::fmtflags(0)*/)
{
	std::stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if(flags)
	{
		stream.setf(flags);
	}
	stream << value;
	return(stream.str());
}

std::string StringConverter::toString(float value, unsigned short width /*= 0*/, char fill /*= ' '*/, std::ios::fmtflags flags /*= std::ios::fmtflags(0)*/)
{
	std::stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if(flags)
	{
		stream.setf(flags);
	}
	stream << value;
	return(stream.str());
}

void StringConverter::toLowerCase(std::string& value)
{
	std::transform(value.begin(), value.end(), value.begin(), tolower);
}

void StringConverter::toUpperCase(std::string& value)
{
	std::transform(value.begin(), value.end(), value.begin(), toupper);
}

bool StringConverter::isNumber(const std::string& string)
{
	std::stringstream value(string);
	float test = 0.0;
	value >> test;
	return(value.fail() == false && value.eof());
}