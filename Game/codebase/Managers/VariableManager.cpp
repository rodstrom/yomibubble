#include "stdafx.h"
#include "VariableManager.h"
#include "IniFile.h"

VariableManager* VariableManager::m_instance = NULL;

VariableManager::VariableManager(){}
VariableManager::~VariableManager(){}

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

const std::string& VariableManager::GetAsString(const std::string& value, const std::string& def){
	return m_values->getValue("VALUES", value, def);
}

float VariableManager::GetAsFloat(const std::string& value, float def){
	return m_values->getValueAsFloat("VALUES", value, def);
}

int VariableManager::GetAsInt(const std::string& value, int def){
	return m_values->getValueAsInt("VALUES", value, def);
}

bool VariableManager::Init(){
	m_values = new IniFile("../../resources/config/player_variables.ini");
	return m_values->parse();
}