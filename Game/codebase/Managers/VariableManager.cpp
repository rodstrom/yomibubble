#include "stdafx.h"
#include "VariableManager.h"
#include "IniFile.h"

VariableManager* VariableManager::m_instance = NULL;

VariableManager::VariableManager() : m_values(NULL), m_visuals(NULL){}
VariableManager::~VariableManager(){
	if (m_values){
		delete m_values;
		m_values = NULL;
	}
	if (m_visuals){
		delete m_visuals;
		m_visuals = NULL;
	}
}


VariableManager* VariableManager::GetSingletonPtr(){
	if (m_instance == NULL){
		m_instance = new VariableManager;
	}
	return m_instance;
}

void VariableManager::CleanSingleton(){
	if (m_instance){
		delete m_instance;
		m_instance = NULL;
	}
}

const std::string& VariableManager::GetAsString(const std::string& value, int inifile, const std::string& default_group, const std::string& def){
	if (inifile == INIFILE_PLAYER_VARIABLES){
		return m_values->getValue(default_group, value, def);
	}
	else if (inifile = INIFILE_VISUAL_CONFIG){
		return m_visuals->getValue(default_group, value, def);
	}
	return "";
}

float VariableManager::GetAsFloat(const std::string& value, int inifile, const std::string& default_group, float def){
	if (inifile == INIFILE_PLAYER_VARIABLES){
		return m_values->getValueAsFloat(default_group, value, def);
	}
	else if (inifile = INIFILE_VISUAL_CONFIG){
		return m_visuals->getValueAsFloat(default_group, value, def);
	}
	return 0.0f;
}

int VariableManager::GetAsInt(const std::string& value, int inifile, const std::string& default_group, int def){
	if (inifile == INIFILE_PLAYER_VARIABLES){
		return m_values->getValueAsInt(default_group, value, def);
	}
	else if (inifile = INIFILE_VISUAL_CONFIG){
		return m_visuals->getValueAsInt(default_group, value, def);
	}
	return m_values->getValueAsInt(default_group, value, def);
}

bool VariableManager::GetAsBool(const std::string& value, int inifile, const std::string& default_group, bool def){
	if (inifile == INIFILE_PLAYER_VARIABLES){
		return m_values->getValueAsBool(default_group, value, def);
	}
	else if (inifile = INIFILE_VISUAL_CONFIG){
		return m_visuals->getValueAsBool(default_group, value, def);
	}
	return false;
}

bool VariableManager::Init(){
	bool parse = false;
	m_values = new IniFile("../../resources/config/player_variables.ini");
	parse = m_values->parse();
	m_visuals = new IniFile("video_config.cfg");
	if (!m_visuals->parse()){
		FixVisualIniFile(m_visuals);
	}
	return parse;
}

void VariableManager::FixVisualIniFile(IniFile* inifile){
	inifile->addModify("Shadows", "Show", "true");
	inifile->addModify("Shadows", "TextureSize", "2048");
	inifile->addModify("Shadows", "FarDistance", "30");
	inifile->addModify("Mipmaps", "NumberOfMipmaps", "5");
	inifile->addModify("Bloom", "Show", "true");
	inifile->save();
}