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

#include <string>
#include <sstream>

namespace{
	const std::string BoolToString(bool p_value);
	template <typename T> const std::string NumberToString(const T);			// converts any given value to a std::string
	template <typename T> T StringToNumber(const std::string&);					// converts a std::string to any valid number
	bool StringToBool(const std::string&);										// converts a string into a bool
}

namespace{
	const std::string BoolToString(bool p_value) {
		if (p_value){
			return "true";
		}
		return "false";
	}
}

namespace{
	template <typename T> const std::string NumberToString(const T p_line) {
		std::stringstream stream;
		stream << p_line;
		return stream.str();
	}
}

namespace{
	template <typename T> T StringToNumber(const std::string& p_line) {
		std::stringstream stream(p_line);
		T result;
		return stream >> result ? result : 0;
	}
}

namespace {
	bool StringToBool(const std::string& p_line) {
		std::string temp = p_line;
		for (unsigned int i = 0; i < temp.length(); i++) {
			temp[i] = tolower(temp[i]);	// convert all characters to lower case, will make checking the string easier.
		}
		return (temp == "true") ? true : false;
	}
}






