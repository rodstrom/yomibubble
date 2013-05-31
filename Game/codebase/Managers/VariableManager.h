#ifndef VARIABLE_MANAGER_H
#define VARIABLE_MANAGER_H

#include <map>
#include <string>
#include "../ConfigFile.h"

enum EIniFile{
	INIFILE_PLAYER_VARIABLES = 0,
	INIFILE_VISUAL_CONFIG
};

class IniFile;
class VariableManager{
public:
	static VariableManager* GetSingletonPtr();
	static void CleanSingleton();


	bool Init();

	const std::string& GetAsString(const std::string& value, int inifile = INIFILE_PLAYER_VARIABLES, const std::string& default_group = "VALUES", const std::string& def = " ");
	float GetAsFloat(const std::string& value, int inifile = INIFILE_PLAYER_VARIABLES, const std::string& default_group = "VALUES", float def = 0.0f);
	int GetAsInt(const std::string& value, int inifile = INIFILE_PLAYER_VARIABLES, const std::string& default_group = "VALUES", int def = 0);
	bool GetAsBool(const std::string& value, int inifile = INIFILE_PLAYER_VARIABLES, const std::string& default_group = "VALUES", bool def = true);

private:
	VariableManager();
	~VariableManager();

	void FixVisualIniFile(IniFile* inifile);	// if the visual config file cannot be found or read create a new with default values

	static VariableManager* m_instance;
	IniFile* m_values;
	IniFile* m_visuals;
};

#endif VARIABLE_MANAGER_H