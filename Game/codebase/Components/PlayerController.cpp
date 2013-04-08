#include "stdafx.h"
#include "PlayerController.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"

void PlayerController::Update(float deltatime){
	if (m_input_manager->IsButtonDown(BTN_LEFT)){
		AddForceMsg msg;
		msg.dir = Ogre::Vector3(0.0f,-10.0f,0.0f);
		msg.strength = 50.0f;
		m_messenger->Notify(MSG_ADD_FORCE, &msg);
	}
	else if (m_input_manager->IsButtonDown(BTN_RIGHT)){
		AddForceMsg msg;
		msg.dir = Ogre::Vector3(0.0f,10.0f,0.0f);
		msg.strength = 50.0f;
		m_messenger->Notify(MSG_ADD_FORCE, &msg);
	}

	if (m_input_manager->IsButtonDown(BTN_UP)){
		AddForceMsg msg;
		msg.dir = Ogre::Vector3(0.0f,0.0f,10.0f);
		msg.strength = 50.0f;
		m_messenger->Notify(MSG_ADD_FORCE, &msg);
	}
	else if (m_input_manager->IsButtonDown(BTN_DOWN)){
		AddForceMsg msg;
		msg.dir = Ogre::Vector3(0.0f,0.0f,-10.0f);
		msg.strength = 50.0f;
		m_messenger->Notify(MSG_ADD_FORCE, &msg);
	}

	if (m_input_manager->IsButtonPressed(BTN_W)){
		AnimationMsg msg;
		msg.index = 0;
		msg.id = "RunBase";
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
	}
	if (m_input_manager->IsButtonPressed(BTN_S)){
		AnimationMsg msg;
		msg.index = 0;
		msg.id = "0";
		m_messenger->Notify(MSG_ANIMATION_PAUSE, &msg);
	}

	if (m_input_manager->IsButtonPressed(BTN_A)){
		AnimationMsg msg;
		msg.index = 1;
		msg.id = "RunTop";
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
	}
	if (m_input_manager->IsButtonPressed(BTN_D)){
		AnimationMsg msg;
		msg.index = 1;
		msg.id = "1";
		m_messenger->Notify(MSG_ANIMATION_PAUSE, &msg);
	}

}

void PlayerController::Notify(int type, void* message){

}

void PlayerController::Shut(){

}

void PlayerController::Init(InputManager* input_manager){
	m_input_manager = input_manager;
}

void PlayerController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

