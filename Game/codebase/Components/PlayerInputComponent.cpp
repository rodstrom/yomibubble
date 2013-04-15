#include "stdafx.h"
#include "PlayerInputComponent.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"
#include "..\BtOgreGP.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"

void PlayerInputComponent::Update(float dt){
	bool move_left = false;
	bool move_right = false;
	bool move_forward = false;
	bool move_back = false;

	if (m_input_manager->IsButtonDown(BTN_LEFT)){
		move_left = true;
	}
	else if (m_input_manager->IsButtonDown(BTN_RIGHT)){
		move_right = true;
	}

	if (m_input_manager->IsButtonDown(BTN_UP)){
		move_forward = true;
	}
	else if (m_input_manager->IsButtonDown(BTN_DOWN)){
		move_back = true;
	}

	if (!m_is_creating_bubble){
		Ogre::SceneNode* node = NULL;
		if (m_input_manager->IsButtonPressed(BTN_LEFT_MOUSE)){
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			if (node){
				m_current_bubble = m_owner->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_BLUE_BUBBLE, node->_getDerivedPosition(), NULL);
				m_bubble_type = BUBBLE_TYPE_BLUE;
				m_is_creating_bubble = true;
			}
		}
		else if (m_input_manager->IsButtonPressed(BTN_RIGHT_MOUSE)){
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			if (node){
				m_current_bubble = m_owner->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_PINK_BUBBLE, node->_getDerivedPosition(), NULL);
				m_bubble_type = BUBBLE_TYPE_PINK;
				m_is_creating_bubble = true;
			}
		}
	}
	else{
		const float SCALE = 0.001f * dt;
		Ogre::Vector3 scale_inc(SCALE,SCALE,SCALE);
		m_current_scale += SCALE;
		if (m_bubble_type == BUBBLE_TYPE_BLUE && m_input_manager->IsButtonReleased(BTN_LEFT_MOUSE)){
			Ogre::Vector3 gravity(0,-10,0);
			m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble = NULL;
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && m_input_manager->IsButtonReleased(BTN_RIGHT_MOUSE)){
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble = NULL;
		}

		if (m_bubble_type == BUBBLE_TYPE_BLUE && m_input_manager->IsButtonDown(BTN_LEFT_MOUSE)){
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			if (node){
				Ogre::Vector3 pos = node->_getDerivedPosition();
				m_current_bubble->GetComponentMessenger()->Notify(MSG_SET_OBJECT_POSITION, &pos);
				m_current_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_inc);
			}
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && m_input_manager->IsButtonDown(BTN_RIGHT_MOUSE)){
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			if (node){
				Ogre::Vector3 pos = node->_getDerivedPosition();
				m_current_bubble->GetComponentMessenger()->Notify(MSG_SET_OBJECT_POSITION, &pos);
				m_current_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_inc);
			}
		}

	}



	if (m_input_manager->IsButtonPressed(BTN_START)){
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, NULL);
	}

	/*if (move_left || move_right || move_back || move_forward){
		AnimationMsg msg1;
		msg1.id = "RunBase";
		msg1.index = 0;
		AnimationMsg msg2;
		msg2.id = "RunTop";
		msg2.index = 1;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg1);
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg2);
	}
	else {
		AnimationMsg msg1;
		msg1.id = "IdleTop";
		msg1.index = 0;
		AnimationMsg msg2;
		msg2.id = "IdleBase";
		msg2.index = 1;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg1);
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg2);
	}*/

	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_MOVE_FORWARD, &move_forward);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS, &move_back);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_MOVE_LEFT, &move_left);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_MOVE_RIGHT, &move_right);
}

void PlayerInputComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_INPUT_MANAGER_GET:
		*static_cast<InputManager**>(msg) = m_input_manager;
		break;
	default:
		break;
	}
}

void PlayerInputComponent::Shut(){
	m_messenger->Unregister(MSG_INPUT_MANAGER_GET, this);
}

void PlayerInputComponent::Init(InputManager* input_manager){
	m_input_manager = input_manager;
	
}

void PlayerInputComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_INPUT_MANAGER_GET, this);
}

