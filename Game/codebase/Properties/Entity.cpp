#include "..\stdafx.h"
#include "Entity.h"
#include "Properties.h"

Entity::Entity(EntityController* p_entity_controller, EntityPhysics* p_entity_physics) : m_entity_controller(p_entity_controller), m_entity_physics(p_entity_physics) {
	if (m_entity_controller) m_entity_controller->Init(this);
	if (m_entity_physics) m_entity_physics->Init(this);
}

Entity::~Entity(void) {}

void Entity::Update(float p_deltatime){
	if (m_entity_controller) m_entity_controller->Update(p_deltatime);
	if (m_entity_physics) m_entity_physics->Update(p_deltatime);
}