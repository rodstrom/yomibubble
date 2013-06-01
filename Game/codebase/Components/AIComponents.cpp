#include "stdafx.h"
#include "AIComponents.h"
#include "ComponentMessenger.h"
#include "PhysicsComponents.h"
#include "..\Waypoint.h"
#include "..\AI\AIPrereq.h"
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

void TottAIComponent::Notify(int type, void* message){
	bool pause = *static_cast<bool*>(message);

	switch (type){
	case MSG_AI_PAUSE:
		m_pause = pause;

		if (m_pause){
			Ogre::Vector3 dir(0,0,0);
			m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
		}
		break;
	default:
		break;
	}
}

void TottAIComponent::Shut(){
	m_messenger->Unregister(MSG_AI_PAUSE, this);
}

void TottAIComponent::Init(const std::vector<AIState*>& ai_states){
	m_ai_states = ai_states;
	for (unsigned int i = 0; i < m_ai_states.size(); i++){
		m_ai_states[i]->Init(m_messenger);
	}
	m_current_ai_state = m_ai_states.front();
	m_current_ai_state->Enter();
	m_pause = false;
}

void TottAIComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_AI_PAUSE, this);
}

void TottAIComponent::Update(float dt){
	if (!m_pause){
		if (m_current_ai_state->Update(dt)){
			m_current_ai_state->Exit();
			if (m_current_ai_state == m_ai_states.back()) {  // if the AI state is at the end of the list. reset to first position
				m_current_ai_state->Exit();
				m_current_ai_state = m_ai_states.front();
				m_current_ai_state->Enter();
				m_current_index = 0;
			}
			else {
				m_current_index++;
				m_current_ai_state->Exit();
				m_current_ai_state = m_ai_states[m_current_index];
				m_current_ai_state->Enter();
			}
		}
	}
}
