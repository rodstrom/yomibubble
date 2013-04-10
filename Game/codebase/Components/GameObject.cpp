#include "..\stdafx.h"
#include "GameObject.h"
#include "Components.h"

int GameObject::m_object_counter = -1;

GameObject::GameObject(void){
	m_messenger = new ComponentMessenger;
	m_object_counter++;
	m_id = "GameObject" + NumberToString(m_object_counter);
}

GameObject::GameObject(const Ogre::String& id) : m_id(id){
	m_messenger = new ComponentMessenger;
	m_object_counter++;
}

GameObject::~GameObject(void){}

void GameObject::Update(float dt){
	if (!m_updateables.empty()){
		for (unsigned int i = 0; i < m_updateables.size(); i++){
			m_updateables[i]->Update(dt);
		}
	}
}

void GameObject::LateUpdate(float dt){
	if (!m_late_updates.empty()){
		for (unsigned int i = 0; i < m_late_updates.size(); i++){
			m_late_updates[i]->LateUpdate(dt);
		}
	}
}

Component* GameObject::GetComponent(int type){
	if (!m_components.empty()){
		for (unsigned int i = 0; i < m_components.size(); i++){
			if (m_components[i]->GetComponentType() == type){
				return m_components[i];
			}
		}
	}
	return NULL;
}

void GameObject::AddComponent(Component* component){
	m_components.push_back(component);
	component->SetOwner(this);
	component->SetMessenger(m_messenger);
}

void GameObject::AddUpdateable(IComponentUpdateable* updateable){
	m_updateables.push_back(updateable);
}

void GameObject::AddLateUpdate(IComponentLateUpdate* late_update){
	m_late_updates.push_back(late_update);
}

void GameObject::Shut(){
	if (m_updateables.empty()){
		m_updateables.clear();
	}
	if (!m_late_updates.empty()){
		m_late_updates.clear();
	}
	if (!m_components.empty()){
		for (unsigned int i = 0; i < m_components.size(); i++){
			m_components[i]->Shut();
			delete m_components[i];
			m_components[i] = NULL;
		}
		m_components.clear();
	}
	if (m_messenger){
		delete m_messenger;
		m_messenger = NULL;
	}
}

bool GameObject::DoUpdate(){
	if (!m_updateables.empty()){
		return true;
	}
	return false;
}

bool GameObject::DoLateUpdate(){
	if (!m_late_updates.empty()){
		return true;
	}
	return false;
}

void GameObject::GetComponents(int type, std::vector<Component*>& list){
	for (unsigned int i = 0; i < m_components.size(); i++){
		if (m_components[i]->GetComponentType() == type){
			list.push_back(m_components[i]);
		}
	}
}