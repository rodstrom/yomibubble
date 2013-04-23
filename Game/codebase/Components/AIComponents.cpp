#include "stdafx.h"
#include "AIComponents.h"
#include "ComponentMessenger.h"
#include "PhysicsComponents.h"
	
void WayPointComponent::Notify(int type, void* message){
};

void WayPointComponent::Shut(){
	m_way_point = NULL;
};

void WayPointComponent::Init(Ogre::SceneNode* node, float walk_speed){
	m_way_point = new WayPoint();
	m_way_point->Init(node, walk_speed);
	float turn_speed = m_way_point->m_walk_speed * 2.0f;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_TURN_SPEED_SET, &turn_speed);
};


void WayPointComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
};

void WayPointComponent::Update(float dt){
	m_way_point->Update(dt);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &m_way_point->m_direction);
	float speed = m_way_point->getSpeed();
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_VELOCITY_SET, &speed);
};

void WayPointComponent::AddWayPoint(Ogre::Vector3 way_point){
	m_way_point->AddWayPoint(way_point);
};

void WayPointComponent::AddWayPoint(Ogre::SceneNode* scene_node){
	m_way_point->AddWayPoint(scene_node);
};