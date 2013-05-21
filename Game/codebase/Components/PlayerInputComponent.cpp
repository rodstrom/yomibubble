#include "stdafx.h"
#include "PlayerInputComponent.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"
#include "..\BtOgreGP.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "..\PhysicsEngine.h"
#include "VisualComponents.h"
#include "..\PhysicsPrereq.h"
#include "..\MessageSystem.h"
#include <iostream>
#include "..\Managers\AnimationManager.h"
#include "..\AnimPrereq.h"
#include "..\Managers\PlayerStateManager.h"
#include "PlayerState.h"
#include "..\Managers\VariableManager.h"

void PlayerInputComponent::Update(float dt){
	m_direction.x = m_input_manager->GetMovementAxis().x;
	m_direction.z = m_input_manager->GetMovementAxis().z;
	m_player_state_manager->Update(dt);
	m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music);
}

inline void PlayerInputComponent::Jump(float dt){

}

void PlayerInputComponent::SetCustomVariables(float min_bubble_size, float max_bubble_size, float on_bubble_mod, float in_bubble_mod){
	m_min_bubble_size = min_bubble_size;
	m_max_bubble_size = max_bubble_size;
	m_on_bubble_speed_mod = on_bubble_mod;
	m_in_bubble_speed_mod = in_bubble_mod;
};

void PlayerInputComponent::OntoBubbleTransition(float dt){

}

void PlayerInputComponent::IntoBubbleTransition(float dt){

}

inline void PlayerInputComponent::BlowBubble(float dt){

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
		m_player_state_manager->SetPlayerState(m_player_state_manager->GetPlayerState(*static_cast<int*>(msg)));
		break;
	case MSG_PLAYER_INPUT_STATE_GET:
		*static_cast<int*>(msg) = m_player_state_manager->GetCurrentType();
		break;
	case MSG_ON_GROUND:
		m_on_ground = *static_cast<bool*>(msg);
		break;
	default:
		break;
	}
}

void PlayerInputComponent::Shut(){
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_STATE, this);
	m_messenger->Unregister(MSG_PLAYER_INPUT_STATE_GET, this);
	m_messenger->Unregister(MSG_ON_GROUND, this);
	if (m_animation_manager){
		delete m_animation_manager;
		m_animation_manager = NULL;
	}
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void PlayerInputComponent::Init(InputManager* input_manager, SoundManager* sound_manager, PhysicsEngine* physics_engine, MessageSystem* message_system){
	m_input_manager = input_manager;
	m_physics_engine = physics_engine;

	m_start_music = sound_manager->Create2DData("Tutorial_Theme", false, false, false, false, 1.0f, 1.0f);
	m_day_music = sound_manager->Create2DData("Day_Theme", false, false, false, false, 1.0f, 1.0f);;
	m_night_music = sound_manager->Create2DData("Night_Theme", false, false, false, false, 1.0f, 1.0f);;

	m_bounce_sound = sound_manager->Create2DData("Bounce", false, false, false, false, 1.0f, 1.0f);
	m_leaf_sfx = sound_manager->Create2DData("Take_Leaf", false, false, false, false, 1.0f, 1.0f);
	m_walk_sound = sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
	m_test_sfx = sound_manager->Create2DData("Dun_Dun", true, false, false, false, 1.0f, 1.0f);
	m_3D_music_data = sound_manager->Create3DData("Main_Theme", "", false, false, false, 1.0f, 1.0f);
	//m_leaf_sfx = sound_manager->Create2DData("Take_Leaf", false, false, false, false, 1.0f, 1.0f);

	m_jump_sound = sound_manager->Create2DData("Jump", false, false, false, false, 1.0f, 1.0f);
	//m_bounce_sound = sound_manager->Create2DData("Bounce", false, false, false, false, 1.0f, 1.0f);
	m_bubble_burst_sound = sound_manager->Create2DData("Bubble_Burst", false, false, false, false, 1.0f, 1.0f);
	m_bubble_blow_sound = sound_manager->Create2DData("Blow_Bubble", false, false, false, false, 1.0f, 1.0f);

	/*m_states[PLAYER_STATE_NORMAL] =					&PlayerInputComponent::Normal;
	m_states[PLAYER_STATE_ON_BUBBLE] =				&PlayerInputComponent::OnBubble;
	m_states[PLAYER_STATE_INSIDE_BUBBLE] =			&PlayerInputComponent::InsideBubble;
	m_states[PLAYER_STATE_BOUNCING] =				&PlayerInputComponent::Bouncing;
	m_states[PLAYER_STATE_INTO_BUBBLE_TRANSITION] =	&PlayerInputComponent::IntoBubbleTransition;
	m_states[PLAYER_STATE_ONTO_BUBBLE_TRANSITION] =	&PlayerInputComponent::OntoBubbleTransition;*/

	//m_min_bubble_size = 0.805f;	// Tiny was here
	//m_max_bubble_size = 1.907f;

	m_player_action = 0;
	m_animation_manager = new AnimationManager(m_messenger);
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle", 0, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Run", 0, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk", 0, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_End", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_Loop", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_Start", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_End", 0, 0.2f, "Top_Jump_End"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_Loop", 0, 0.2f, "Top_Jump_Loop"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_Start", 0, 0.2f, "Top_Jump_Start"));
	m_animation_manager->AddAnimation(AnimationDef("Base_PickUpLeaf_State", 0, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Idle", 1, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Run", 1, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Walk", 1, 0.2f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk_On_Bubble", 0, 0.2f, "Top_Walk_On_Bubble"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle_On_Bubble", 0, 0.2f, "Top_Idle_On_Bubble"));
	m_player_state_manager = new PlayerStateManager;

	PlayerState::Init(m_owner->GetComponentMessenger(), m_animation_manager, this, m_player_state_manager, sound_manager);
	m_player_state_manager->AddPlayerState(new PlayerIdle);
	m_player_state_manager->AddPlayerState(new PlayerStateMove);
	m_player_state_manager->AddPlayerState(new PlayerBlowBubble);
	m_player_state_manager->AddPlayerState(new PlayerJump);
	m_player_state_manager->AddPlayerState(new PlayerFalling);
	m_player_state_manager->AddPlayerState(new PlayerLand);
	m_player_state_manager->AddPlayerState(new PlayerBounce);
	m_player_state_manager->AddPlayerState(new PlayerOnBubble(message_system));
	m_player_state_manager->AddPlayerState(new PlayerInsideBubble(message_system));
	m_player_state_manager->AddPlayerState(new PlayerHoldObject(physics_engine));
	m_player_state_manager->Init();
	m_player_state_manager->SetPlayerState(m_player_state_manager->GetPlayerState(PLAYER_STATE_FALLING));

	m_physics_engine->AddObjectSimulationStep(this);
}

void PlayerInputComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_INPUT_MANAGER_GET, this);
	m_messenger->Register(MSG_PLAYER_INPUT_STATE_GET, this);
	m_messenger->Register(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Register(MSG_PLAYER_INPUT_SET_STATE, this);
	m_messenger->Register(MSG_ON_GROUND, this);
}

void PlayerInputComponent::Normal(float dt){

}

void PlayerInputComponent::OnBubble(float dt){

}

void PlayerInputComponent::InsideBubble(float dt){

}

void PlayerInputComponent::Bouncing(float dt){

}

void PlayerInputComponent::CreateTriggerForBubble(){

}

void PlayerInputComponent::SimulationStep(btScalar time_step){

}

void BubbleController::Notify(int type, void* msg){
	switch (type){
		case MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE:
			m_apply_impulse = true;
			m_impulse = *static_cast<Ogre::Vector3*>(msg);
			break;
		case MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET:
			*static_cast<bool*>(msg) = m_can_be_attached;
			break;
		case MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET:
			m_can_be_attached = *static_cast<bool*>(msg);
			break;
		case MSG_BUBBLE_CONTROLLER_TIMER_RUN:
			m_run_timer = *static_cast<bool*>(msg);
			break;
	default:
		break;
	}
}

void BubbleController::Shut(){
	BubbleEvent evt;
	evt.m_type = EVT_BUBBLE_REMOVE;
	evt.bubble = m_owner;
	m_message_system->Notify(&evt);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_TIMER_RUN, this);
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void BubbleController::Init(PhysicsEngine* physics_engine, MessageSystem* message_system, float velocity, float max_velocity){
	m_physics_engine = physics_engine;
	m_message_system = message_system;
	m_velocity = velocity;
	m_max_velocity = max_velocity;
	m_max_life_time = VariableManager::GetSingletonPtr()->GetAsFloat("PinkBubbleLifetime");
	physics_engine->AddObjectSimulationStep(this);
}

void BubbleController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_TIMER_RUN, this);
}

void BubbleController::Update(float dt){
	if (m_apply_impulse){
		Ogre::Vector3 impulse = (m_impulse * m_velocity) * dt;
		m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse, "body");
		m_apply_impulse = false;
	}
	if (m_run_timer){
		m_life_timer = std::min(m_life_timer + dt, m_max_life_time);
		if (m_life_timer >= m_max_life_time){
			m_owner->RemoveGameObject(m_owner);
		}
	}
}

void BubbleController::SimulationStep(btScalar time_step){
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	if (body){
		const btScalar max_speed = (btScalar)m_max_velocity;
		btVector3 vel = body->getLinearVelocity();
		btScalar speed = vel.length();
		if (speed >= max_speed){
			//vel *= max_speed/speed;
			vel = vel / speed * m_max_velocity;
			body->setLinearVelocity(vel);
		}
	}
}