#include "stdafx.h"
#include "AIComponents.h"
#include "ComponentMessenger.h"
#include "PhysicsComponents.h"
#include "VisualComponents.h"
#include "GameObject.h"

#include <functional>

void WayPointComponent::Notify(int type, void* message){
	switch(type){
	case MSG_IN_DISTANCE_PLAYER:
		std::cout << "Following player" << std::cout;
		m_way_point->AddWayPoint(static_cast<NodeComponent*>(m_owner->GetGameObject("GameObject1")->GetComponent(EComponentType::COMPONENT_NODE))->GetSceneNode());
			break;
	case MSG_IN_DISTANCE_FRIENDS:
		break;
	case MSG_DEFAULT_WAY_POINTS:
		m_way_point->AddWayPoint(Ogre::Vector3(170, 75, 173));
		m_way_point->AddWayPoint(Ogre::Vector3(150, 75, 173));
		break;
	default:
		break;
	}
};

void WayPointComponent::Shut(){
	delete m_way_point;
	m_way_point = NULL;
	m_messenger->Unregister(MSG_IN_DISTANCE_PLAYER, this);
	m_messenger->Unregister(MSG_IN_DISTANCE_FRIENDS, this);
	m_messenger->Unregister(MSG_DEFAULT_WAY_POINTS, this);
};

void WayPointComponent::Init(Ogre::SceneNode* node, const Ogre::String& p_loop_way_points){
	m_way_point = new WayPoint();
	m_way_point->Init(node);
	m_way_point->setLoopable(p_loop_way_points);

	m_messenger->Notify(MSG_DEFAULT_WAY_POINTS, this);
};


void WayPointComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_IN_DISTANCE_PLAYER, this);
	m_messenger->Register(MSG_IN_DISTANCE_FRIENDS, this);
	m_messenger->Register(MSG_DEFAULT_WAY_POINTS, this);
};

void WayPointComponent::Update(float dt){
	m_way_point->Update(dt);
	Ogre::Vector3 dir = m_way_point->GetDirection().normalisedCopy();
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);

	if(m_way_point->withinDistance(5.0f)){
		m_messenger->Notify(MSG_IN_DISTANCE_PLAYER, this);
	}
	
};

void WayPointComponent::SetLoopable(const Ogre::String& loop){
	m_way_point->setLoopable(loop);
	
};

void WayPointComponent::AddWayPoint(Ogre::Vector3 way_point){
	m_way_point->AddWayPoint(way_point);
};

void WayPointComponent::AddWayPoint(Ogre::SceneNode* scene_node){
	m_way_point->AddWayPoint(scene_node);
};