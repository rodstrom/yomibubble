#ifndef VARIABLE_MANAGER_H
#define VARIABLE_MANAGER_H

#include <map>
#include <string>
#include "../ConfigFile.h"

class IniFile;
class VariableManager{
public:
	static VariableManager* GetSingletonPtr();
	static void CleanSingleton();
	
	bool Init();

	const std::string& GetAsString(const std::string& value, const std::string& def = " ");
	float GetAsFloat(const std::string& value, float def = 0.0f);
	int GetAsInt(const std::string& value, int def = 0);
	
private:
	VariableManager();
	~VariableManager();

	static VariableManager* m_instance;
	IniFile* m_values;
};

#endif //VARIABLE_MANAGER_H