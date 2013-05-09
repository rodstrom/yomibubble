#include "stdafx.h"
#include "VariableManager.h"

VariableManager::VariableManager(){
};
	
VariableManager::~VariableManager(){
};

void VariableManager::Init(){
	m_variable_map.insert(std::pair<std::string, float>("Player_Speed", 5.0));
	m_variable_map.insert(std::pair<std::string, float>("Player_Max_Speed", 1.0));
	m_variable_map.insert(std::pair<std::string, float>("Player_Deceleration", 10.0));
	m_variable_map.insert(std::pair<std::string, float>("Player_Jump_Power", 300.0));
	m_variable_map.insert(std::pair<std::string, float>("Player_Max_Jump_Height", 1.7));
	m_variable_map.insert(std::pair<std::string, float>("Bubble_Linear_Damping", 1.5));
	m_variable_map.insert(std::pair<std::string, float>("Bubble_Angular_Damping", 1.5));
	m_variable_map.insert(std::pair<std::string, float>("Min_Bubble_Size", 0.805));
	m_variable_map.insert(std::pair<std::string, float>("Max_Bubble_Size", 1.907));
	m_variable_map.insert(std::pair<std::string, float>("Bounce_Jump_Mod", 1.3));
	m_variable_map.insert(std::pair<std::string, float>("Level_Choice", 1));
};

void VariableManager::LoadVariables(){
	m_config_file = new Config("player_variables.ini");
	m_config_file->parse();

	std::map<std::string, float>::iterator it;

	for (it = m_variable_map.begin(); it != m_variable_map.end(); it++){
		m_variable_map.find(it->first)->second = m_config_file->getAsFloat(it->first);
	}

	m_config_file->save();
};
	
void VariableManager::SaveVariables(){
};

float VariableManager::GetValue(std::string name){
	return m_variable_map.find(name)->second;
};