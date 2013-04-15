#include "stdafx.h"
#include "ComponentMessenger.h"

ComponentMessenger::ComponentMessenger(void){
	m_listeners.resize(MSG_SIZE);
}

ComponentMessenger::~ComponentMessenger(void){Shut();}

void ComponentMessenger::Notify(int type, void* message){
	if (!m_listeners[type].empty()){
		for (unsigned int i = 0; i < m_listeners[type].size(); i++){
			m_listeners[type][i]->Notify(type, message);
		}
	}
}

void ComponentMessenger::Register(int type, IComponentObserver* obj){
	m_listeners[type].push_back(obj);
}

void ComponentMessenger::Unregister(IComponentObserver* ob){
	for (unsigned int i = 0; i < m_listeners.size(); i++){
		for (unsigned int j = 0; j < m_listeners[i].size(); j++){
			if (ob == m_listeners[i][j]){
				m_listeners[i].erase(m_listeners[i].begin() + j);
				j--;
			}
		}
	}
}

void ComponentMessenger::Unregister(int type, IComponentObserver* ob){
	if (!m_listeners[type].empty()){
		for (unsigned int i = 0; i < m_listeners[type].size(); i++){
			if (ob == m_listeners[type][i]){
				m_listeners[type].erase(m_listeners[type].begin() + i);
			}
		}
	}
}

inline void ComponentMessenger::Shut(){
	for (unsigned int i = 0; i < m_listeners.size(); i++){
		if (!m_listeners[i].empty()){
			m_listeners[i].clear();
		}
	}
	m_listeners.clear();
}