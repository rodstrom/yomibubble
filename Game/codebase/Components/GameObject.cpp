#include "stdafx.h"
#include "GameObject.h"
#include "Components.h"

GameObject::GameObject(void){}
GameObject::~GameObject(void){}

void GameObject::Update(float deltatime){
	if (!m_updateables.empty()){
		for (unsigned int i = 0; i < m_updateables.size(); i++){
			m_updateables[i]->Update(deltatime);
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
}

void GameObject::AddUpdateable(IComponentUpdateable* updateable){
	m_updateables.push_back(updateable);
}

void GameObject::Shut(){
	if (m_updateables.empty()){
		m_updateables.clear();
	}
	if (!m_components.empty()){
		for (unsigned int i = 0; i < m_components.size(); i++){
			m_components[i]->Shut();
			delete m_components[i];
			m_components[i] = NULL;
		}
		m_components.clear();
	}
}

