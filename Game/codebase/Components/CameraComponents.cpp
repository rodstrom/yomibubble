#include "stdafx.h"
#include "CameraComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"

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
	if (camera_id == Ogre::StringUtil::BLANK){
		m_camera_id = m_owner->GetId() + "Camera";
	}
	else{
		m_camera_id = camera_id;
	}
	m_camera = m_scene_manager->createCamera(m_camera_id);
	if (activate){
		m_viewport->setCamera(m_camera);
	}
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
}

void CameraComponent::Update(float dt){

}

void CameraComponent::ActivateCamera(){
	m_viewport->setCamera(m_camera);
}

void FollowCameraComponent::Notify(int type, void* msg){
	CameraComponent::Notify(type, msg);
	switch (type){
	case MSG_CAMERA_GET_CAMERA_NODE:
		*static_cast<Ogre::SceneNode**>(msg) = m_camera_node;
		break;
	default:
		break;
	}
}

void FollowCameraComponent::Shut(){
	CameraComponent::Shut();
	m_messenger->Unregister(MSG_CAMERA_GET_CAMERA_NODE, this);
}

void FollowCameraComponent::SetMessenger(ComponentMessenger* messenger){
	CameraComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CAMERA_GET_CAMERA_NODE, this);
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
}

void FollowCameraComponent::Update(float dt){
	InputManager* input = NULL;
	m_messenger->Notify(MSG_INPUT_MANAGER_GET, &input);
	if (input){
		OIS::MouseState ms = input->GetMouseState();
		UpdateCameraGoal(-0.1f * ms.X.rel, -0.1f * ms.Y.rel, -0.0005f * ms.Z.rel);
	}
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		m_camera_pivot->setPosition(node->getPosition() + Ogre::Vector3::UNIT_Y * 0.2f);
		Ogre::Vector3 goal_offset = m_camera_goal->_getDerivedPosition() - m_camera_node->getPosition();
		m_camera_node->translate(goal_offset * dt * 2.0f);
		m_camera_node->lookAt(m_camera_pivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
	}
}

void FollowCameraComponent::UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom){
	m_camera_pivot->yaw(Ogre::Degree(delta_yaw), Ogre::Node::TS_WORLD);
	if (!(m_pivot_pitch + delta_pitch > 15 && delta_pitch > 0) && 
		!(m_pivot_pitch + delta_pitch < -60 && delta_pitch < 0)){
			m_camera_pivot->pitch(Ogre::Degree(delta_pitch), Ogre::Node::TS_LOCAL);
			m_pivot_pitch += delta_pitch;
	}
	Ogre::Real dist = m_camera_goal->_getDerivedPosition().distance(m_camera_pivot->_getDerivedPosition());
	Ogre::Real dist_change = delta_zoom * dist;

	if (!(dist + dist_change < 2 && dist_change < 0) &&
		!(dist + dist_change > 25 && dist_change > 0)){
			m_camera_goal->translate(0,0, dist_change, Ogre::Node::TS_LOCAL);
	}
}