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
	m_messenger->Notify(MSG_CHARACTER_CONROLLER_TURN_SPEED_SET, &m_way_point->m_walk_speed);
};


void WayPointComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
};

void WayPointComponent::Update(float dt){
	m_way_point->Update(dt);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &m_way_point->m_direction);
};

void WayPointComponent::AddWayPoint(Ogre::Vector3 way_point){
	m_way_point->AddWayPoint(way_point);
};

void WayPointComponent::AddWayPoint(Ogre::SceneNode* scene_node){
	m_way_point->AddWayPoint(scene_node);
};