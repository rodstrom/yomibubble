#ifndef VARIABLE_MANAGER_H
#define VARIABLE_MANAGER_H

#include <map>
#include <string>
#include "../ConfigFile.h"

class VariableManager{
public:
	VariableManager();
	~VariableManager();

	void Init();

	float GetValue(std::string name);

	void LoadVariables();
	void SaveVariables();

private:
	Config* m_config_file;
	std::map<std::string, float> m_variable_map;
};

#endif //VARIABLE_MANAGER_H