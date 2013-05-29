#include "stdafx.h"
#include "AIComponents.h"
#include "ComponentMessenger.h"
#include "PhysicsComponents.h"
#include "..\Waypoint.h"
#include <functional>

void WayPointComponent::Notify(int type, void* message){
	switch(type){
	case MSG_WAYPOINT_START:
		m_pause = false;
		break;
	case MSG_WAYPOINT_PAUSE:
		m_pause = true;
		break;
	default:
		break;
	};
};

void WayPointComponent::Shut(){
	m_way_point = NULL;
	m_messenger->Unregister(MSG_WAYPOINT_START, this);
	m_messenger->Unregister(MSG_WAYPOINT_PAUSE, this);
};

void WayPointComponent::Init(Ogre::SceneNode* node, float walk_speed){
	m_way_point = new WayPoint();
	m_way_point->Init(node, walk_speed);
	float turn_speed = m_way_point->m_walk_speed * 2.0f;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_TURN_SPEED_SET, &turn_speed);
	m_pause = false;
};


void WayPointComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_WAYPOINT_START, this);
	m_messenger->Register(MSG_WAYPOINT_PAUSE, this);
};

void WayPointComponent::Update(float dt){
	m_way_point->Update(dt);
	m_way_point->m_direction.y = 0;
	Ogre::Vector3 empty = Ogre::Vector3::ZERO;
	//if (m_pause) { m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &empty); }
	//else { m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &m_way_point->m_direction); }
	float speed = m_way_point->getSpeed();
	
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &m_way_point->m_direction);
	/*
	m_anim_msg.id="walk";
	m_anim_msg.blend = false;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	*/
	/*
	if (speed != 0.0f) {
		m_anim_msg.id="walk";
		m_anim_msg.blend = false;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	}
	else {
		m_anim_msg.id="Idle";
		m_anim_msg.blend = false;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	}
	*/
	Ogre::Vector3 speed3(speed, 0, speed);
	speed3.y = 0;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_VELOCITY_SET, &speed);
};

void WayPointComponent::SetLoopable(Ogre::String loop){
	m_way_point->setLoopable(loop);
};

void WayPointComponent::AddWayPoint(Ogre::Vector3 way_point){
	m_way_point->AddWayPoint(way_point);
};

void WayPointComponent::AddWayPoint(Ogre::SceneNode* scene_node){
	m_way_point->AddWayPoint(scene_node);
};