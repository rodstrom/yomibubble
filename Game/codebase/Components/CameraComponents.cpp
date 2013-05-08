#include "stdafx.h"
#include "CameraComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"
#include "GameObjectPrereq.h"

#include <iostream> //for debugging, wohoo

void CameraComponent::Notify(int type, void* msg){
	switch(type){
	case MSG_CAMERA_GET_CAMERA:
		*static_cast<Ogre::Camera**>(msg) = m_camera;
		break;
	case MSG_CAMERA_SET_ACTIVE:
		m_viewport->setCamera(m_camera);
		break;
	}
}

void CameraComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_CAMERA_GET_CAMERA, this);
		m_messenger->Unregister(MSG_CAMERA_SET_ACTIVE, this);
	}
}

void CameraComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CAMERA_GET_CAMERA, this);
	m_messenger->Register(MSG_CAMERA_SET_ACTIVE, this);
}

void CameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	m_scene_manager = scene_manager;
	m_viewport = viewport;
	m_camera = m_scene_manager->getCamera("MainCamera");
	/*if (camera_id == Ogre::StringUtil::BLANK){
		m_camera_id = m_owner->GetId() + "Camera";
	}
	else{
		m_camera_id = camera_id;
	}
	m_camera = m_scene_manager->createCamera(m_camera_id);
	if (activate){
		m_viewport->setCamera(m_camera);
	}
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));*/
}

void CameraComponent::Update(float dt){

}

void CameraComponent::ActivateCamera(){
	//m_viewport->setCamera(m_camera);
}

void FollowCameraComponent::Notify(int type, void* msg){
	CameraDataDef def;
	
	switch (type){
	case MSG_CAMERA_GET_CAMERA_NODE:
		CameraComponent::Notify(type, msg);
		*static_cast<Ogre::SceneNode**>(msg) = m_camera_node;
		break;
	case MSG_DEFAULT_CAMERA_POS:
		def = *static_cast<CameraDataDef*>(msg);
		m_player_direction = def.player_direction;
		break;
	case MSG_FOLLOW_CAMERA_GET_ORIENTATION:
		{
			Ogre::Vector3 goal = Ogre::Vector3::ZERO;
			Ogre::Vector3 dir = *static_cast<Ogre::Vector3*>(msg);
			goal += dir.z * m_camera_node->getOrientation().zAxis();
			goal += dir.x * m_camera_node->getOrientation().xAxis();
			goal.y = 0.0f;
			*static_cast<Ogre::Vector3*>(msg) = goal;	
		}
		break;
	default:
		break;
	}
}

void FollowCameraComponent::Shut(){
	CameraComponent::Shut();
	m_messenger->Unregister(MSG_CAMERA_GET_CAMERA_NODE, this);
	m_messenger->Unregister(MSG_DEFAULT_CAMERA_POS, this);
	m_messenger->Unregister(MSG_FOLLOW_CAMERA_GET_ORIENTATION, this);
}

void FollowCameraComponent::SetMessenger(ComponentMessenger* messenger){
	CameraComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CAMERA_GET_CAMERA_NODE, this);
	m_messenger->Register(MSG_DEFAULT_CAMERA_POS, this);
	m_messenger->Register(MSG_FOLLOW_CAMERA_GET_ORIENTATION, this);
}

void FollowCameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	CameraComponent::Init(scene_manager, viewport, activate, camera_id);
	m_camera_pivot = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_goal = m_camera_pivot->createChildSceneNode(Ogre::Vector3(0,0,2));
	m_camera_node = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_node->setPosition(m_camera_pivot->getPosition() + m_camera_goal->getPosition());
	m_camera_pivot->setFixedYawAxis(true);
	m_camera_goal->setFixedYawAxis(true);
	m_camera_node->setFixedYawAxis(true);
	m_camera_node->attachObject(m_camera);
	m_getting_input = false;
	m_player_direction = Ogre::Vector3::ZERO;
	m_camera_goal->setPosition(0,0,12);
	m_default_distance = 12.0;
	m_default_pitch = -25.7;
	m_camera_pivot->pitch(Ogre::Degree(m_default_pitch), Ogre::Node::TS_LOCAL);
}

void FollowCameraComponent::Update(float dt){
	InputManager* input = NULL;
	m_messenger->Notify(MSG_INPUT_MANAGER_GET, &input);
	if (input){
		CameraAxis axis = input->GetCameraAxis();
		//UpdateCameraGoal(-0.1f * axis.x, -0.1f * axis.y, -0.0005f * axis.z);
		UpdateCameraGoal(axis.x * m_movement_speed, axis.y * m_movement_speed, -0.0005f * axis.z);
	}
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		m_camera_pivot->setPosition(node->getPosition() + Ogre::Vector3::UNIT_Y * 0.2f);
		Ogre::Vector3 goal_offset = m_camera_goal->_getDerivedPosition() - m_camera_node->getPosition();
		m_camera_node->translate(goal_offset * dt * 2.0f);
		m_camera_node->lookAt(m_camera_pivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
		//m_default_position = Ogre::Vector3(node->getPosition().x + 0.002, node->getPosition().y + 0.002, node->getPosition().z + 0.002);
		//m_camera_goal->setPosition(m_default_position);
	}

	//m_trigger->GetCollisionDef

	//std::cout << "kiss " << m_camera->getDerivedPosition().x << std::endl;
	m_camera->getDerivedRight(); //x-led
	m_camera->getDerivedUp(); //y-led

	//std::cout << "bajs" << m_camera->getBoundingBox().getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP).x << std::endl;
}

void FollowCameraComponent::UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom){
	if (delta_yaw != 0.0f
		|| delta_pitch != 0.0f
		|| delta_zoom != 0.0f){
			m_getting_input = true;
	}
	else{
		m_getting_input = false;
	}

	if (!m_getting_input){
		m_camera_goal->setPosition(0, 0, m_default_distance);
		m_pivot_pitch = m_default_pitch;
		m_camera_pivot->yaw(Ogre::Degree(m_player_direction.x * -2.15), Ogre::Node::TS_WORLD);
	}
	
	if (m_getting_input){
		m_camera_pivot->yaw(Ogre::Degree(delta_yaw), Ogre::Node::TS_WORLD);
	}
	if (!(m_pivot_pitch + delta_pitch > 15 && delta_pitch > 0) && 
		!(m_pivot_pitch + delta_pitch < -60 && delta_pitch < 0)
		&& m_getting_input){
			m_camera_pivot->pitch(Ogre::Degree(delta_pitch), Ogre::Node::TS_LOCAL);
			m_pivot_pitch += delta_pitch;
			m_default_pitch = m_pivot_pitch;
	}
	if (m_getting_input){
		Ogre::Real dist = m_camera_goal->_getDerivedPosition().distance(m_camera_pivot->_getDerivedPosition());
		Ogre::Real dist_change = delta_zoom * dist;
		m_default_distance += (delta_zoom * dist);

		if (!(dist + dist_change < 2 && dist_change < 0) &&
		!(dist + dist_change > 25 && dist_change > 0)){
			m_camera_goal->translate(0,0, dist_change, Ogre::Node::TS_LOCAL);
		}
	}
	//std::cout << "Pitch degrees: " << m_pivot_pitch << std::endl;
	//std::cout << "Camera goal: " << m_camera_goal->getPosition() << std::endl; //0,0,12
}