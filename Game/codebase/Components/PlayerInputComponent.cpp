#include "PlayerInputComponent.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"
#include "..\BtOgreGP.h"

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
	if (move_left || move_right || move_back || move_forward){
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
	}

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

