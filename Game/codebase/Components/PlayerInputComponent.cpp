#include "stdafx.h"
#include "PlayerInputComponent.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"
#include "..\BtOgreGP.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"

void PlayerInputComponent::Update(float dt){
	(this->*m_states[m_player_state])(dt);
	m_messenger->Notify(MSG_SFX2D_PLAY, &m_def_music);
}

void PlayerInputComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_INPUT_MANAGER_GET:
		*static_cast<InputManager**>(msg) = m_input_manager;
		break;
	case MSG_PLAYER_INPUT_SET_BUBBLE:
		m_current_bubble = *static_cast<GameObject**>(msg);
		break;
	case MSG_PLAYER_INPUT_SET_STATE:
		m_player_state = *static_cast<int*>(msg);
		break;
	default:
		break;
	}
}

void PlayerInputComponent::Shut(){
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_STATE, this);
}

void PlayerInputComponent::Init(InputManager* input_manager, SoundManager* sound_manager){
	m_input_manager = input_manager;
	m_messenger->Register(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Register(MSG_PLAYER_INPUT_SET_STATE, this);

	m_walk_sound = sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
	m_def_music= sound_manager->Create2DData("Menu_Theme", false, false, false, false, 1.0f, 1.0f);
	m_test_sfx = sound_manager->Create2DData("Dun_Dun", true, false, false, false, 1.0f, 1.0f);
	m_3D_music_data = sound_manager->Create3DData("Main_Theme", "", false, false, false, 1.0f, 1.0f);
	m_leaf_sfx = sound_manager->Create2DData("Take_Leaf", false, false, false, false, 1.0f, 1.0f);

	m_states[PLAYER_STATE_NORMAL] =			&PlayerInputComponent::Normal;
	m_states[PLAYER_STATE_ON_BUBBLE] =		&PlayerInputComponent::OnBubble;
	m_states[PLAYER_STATE_INSIDE_BUBBLE] =	&PlayerInputComponent::InsideBubble;
	m_states[PLAYER_STATE_BOUNCING] =		&PlayerInputComponent::Bouncing;
}

void PlayerInputComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_INPUT_MANAGER_GET, this);
}

void PlayerInputComponent::Normal(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	/*if (m_input_manager->IsButtonDown(BTN_LEFT)){
		dir += Ogre::Vector3(-1.0f, 0.0f, 0.0f);
	}
	else if (m_input_manager->IsButtonDown(BTN_RIGHT)){
		dir += Ogre::Vector3(1.0f, 0.0f, 0.0f);
	}*/

	/*if (m_input_manager->IsButtonDown(BTN_UP)){
		dir += Ogre::Vector3(0.0f, 0.0f, -1.0f);
		m_messenger->Notify(MSG_SFX2D_PLAY, &m_test_sfx);
		//m_messenger->Notify(MSG_SFX3D_STOP, &m_3D_music_data);
	}
	else if (m_input_manager->IsButtonDown(BTN_DOWN)){
		dir += Ogre::Vector3(0.0f, 0.0f, 1.0f);
	}*/

	if (dir != Ogre::Vector3::ZERO){
		m_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
		AnimationMsg msg;
		msg.id="RunBase"; //SliceHorizontal är en top anim
		msg.index=0;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
		//m_messenger->Notify(MSG_ANIMATION_BLEND, &msg);
		AnimationMsg msg2;
		msg.id="SliceHorizontal"; //SliceHorizontal är en top anim
		msg.index=1;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);

	}
	else
	{
		m_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
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
		bool jumping = true;
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, &jumping);
	}
	else if (m_input_manager->IsButtonReleased(BTN_START)){
		bool jumping = false;
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, &jumping);
	}
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

void PlayerInputComponent::OnBubble(float dt){
	Ogre::SceneNode* node = NULL;
	m_current_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		Ogre::Vector3 pos = node->_getDerivedPosition();
		pos.y += 2.0f;
		btRigidBody* body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
		if (body){
			body->setLinearVelocity(btVector3(0,0,0));
			body->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(pos));
		}
	}
	if (m_input_manager->IsButtonPressed(BTN_START)){
		m_player_state = PLAYER_STATE_NORMAL;
		bool jump = true;
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, &jump);
		return;
	}

	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	if (m_input_manager->IsButtonDown(BTN_LEFT)){
		dir += Ogre::Vector3(-1.0f, 0.0f, 0.0f);
	}
	else if (m_input_manager->IsButtonDown(BTN_RIGHT)){
		dir += Ogre::Vector3(1.0f, 0.0f, 0.0f);
	}

	if (m_input_manager->IsButtonDown(BTN_UP)){
		dir += Ogre::Vector3(0.0f, 0.0f, -1.0f);
	}
	else if (m_input_manager->IsButtonDown(BTN_DOWN)){
		dir += Ogre::Vector3(0.0f, 0.0f, 1.0f);
	}
	float speed = 10.0f * dt;
	bool follow_cam = false;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, &follow_cam);
	if (follow_cam){
		Ogre::SceneNode* cam_node = NULL;
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &cam_node);
		if (cam_node && node){
			Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
			goal_dir += dir.z * cam_node->getOrientation().zAxis();
			goal_dir += dir.x * cam_node->getOrientation().xAxis();
			goal_dir.y = 0.0f;
			goal_dir.normalise();
			btRigidBody* body = NULL;
			m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &body);
			if (body){
				body->applyCentralImpulse(BtOgre::Convert::toBullet(goal_dir * speed));
			}
		}
	}
	else{
		btRigidBody* body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
		if (body){
			body->applyCentralImpulse(BtOgre::Convert::toBullet(dir * speed));
		}
	}
	
}

void PlayerInputComponent::InsideBubble(float dt){

}

void PlayerInputComponent::Bouncing(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;

	if (m_input_manager->IsButtonDown(BTN_LEFT)){
		dir += Ogre::Vector3(-1.0f, 0.0f, 0.0f);
	}
	else if (m_input_manager->IsButtonDown(BTN_RIGHT)){
		dir += Ogre::Vector3(1.0f, 0.0f, 0.0f);
	}

	if (m_input_manager->IsButtonDown(BTN_UP)){
		dir += Ogre::Vector3(0.0f, 0.0f, -1.0f);
	}
	else if (m_input_manager->IsButtonDown(BTN_DOWN)){
		dir += Ogre::Vector3(0.0f, 0.0f, 1.0f);
	}

	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}