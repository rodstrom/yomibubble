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
	
	if (m_level == "try"){ m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music); }
	else if (m_level == "Dayarea") { m_messenger->Notify(MSG_SFX2D_PLAY, &m_day_music); }
	else if (m_level == "NightArea") { m_messenger->Notify(MSG_SFX2D_PLAY, &m_night_music); }
	else { m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music); }
	
	//m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music);
}

inline void PlayerInputComponent::Jump(float dt){

}

void PlayerInputComponent::SetCustomVariables(float min_bubble_size, float max_bubble_size, float on_bubble_mod, float in_bubble_mod, Ogre::String level){
	m_min_bubble_size = min_bubble_size;
	m_max_bubble_size = max_bubble_size;
	m_on_bubble_speed_mod = on_bubble_mod;
	m_in_bubble_speed_mod = in_bubble_mod;
	m_level = level;
	if (m_level == "try"){
		m_can_blow_pink = false;
	}
	else {
		m_can_blow_pink = true;
	}
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
		{
			int player_state = *static_cast<int*>(msg);
			if (player_state == PLAYER_STATE_INSIDE_BUBBLE){
				m_player_state_manager->GoInsideBubble(true);
			}
			else {
				m_player_state_manager->SetPlayerState(m_player_state_manager->GetPlayerState(player_state));
			}
		}
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
	if (m_player_state_manager){
		m_player_state_manager->Shut();
		delete m_player_state_manager;
		m_player_state_manager = NULL;
	}
	m_physics_engine->RemoveObjectSimulationStep(this);

	if (m_level == "try"){ m_messenger->Notify(MSG_SFX2D_STOP, &m_start_music); }
	else if (m_level == "Dayarea") { m_messenger->Notify(MSG_SFX2D_STOP, &m_day_music); }
}

void PlayerInputComponent::Init(InputManager* input_manager, SoundManager* sound_manager, PhysicsEngine* physics_engine, MessageSystem* message_system){
	m_input_manager = input_manager;
	m_physics_engine = physics_engine;

	m_start_music = sound_manager->Create2DData("Tutorial_Theme", false, false, false, false, 1.0f, 1.0f);
	m_day_music = sound_manager->Create2DData("Day_Theme", false, false, false, false, 1.0f, 1.0f);
	m_night_music = sound_manager->Create2DData("Night_Theme", false, false, false, false, 1.0f, 1.0f);

	m_bounce_sound = sound_manager->Create2DData("Bounce", false, false, false, false, 1.0f, 1.0f);
	m_leaf_sfx = sound_manager->Create2DData("Take_Leaf", false, false, false, false, 1.0f, 1.0f);
	m_leaf_giggle_sfx = sound_manager->Create2DData("Yomi_Giggle", false, false, false, false, 1.0f, 1.0f);
	m_walk_sound = sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
	m_test_sfx = sound_manager->Create2DData("Dun_Dun", true, false, false, false, 1.0f, 1.0f);
	m_3D_music_data = sound_manager->Create3DData("Main_Theme", "", false, false, false, 1.0f, 1.0f);
	//m_leaf_sfx = sound_manager->Create2DData("Take_Leaf", false, false, false, false, 1.0f, 1.0f);

	m_jump_sound = sound_manager->Create2DData("Jump", false, false, false, false, 1.0f, 1.0f);
	//m_bounce_sound = sound_manager->Create2DData("Bounce", false, false, false, false, 1.0f, 1.0f);
	m_bubble_burst_sound = sound_manager->Create2DData("Bubble_Burst", false, false, false, false, 1.0f, 1.0f);
	m_bubble_blow_sound = sound_manager->Create2DData("Blow_Bubble", false, false, false, false, 1.0f, 1.0f);

	m_bounce_1 = sound_manager->Create2DData("Bounce_1", false, false, false, false, 1.0f, 1.0f);
	m_bounce_2 = sound_manager->Create2DData("Bounce_2", false, false, false, false, 1.0f, 1.0f);
	m_bounce_3 = sound_manager->Create2DData("Bounce_3", false, false, false, false, 1.0f, 1.0f);
	m_bounce_4 = sound_manager->Create2DData("Bounce_4", false, false, false, false, 1.0f, 1.0f);

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
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle", 0, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle2", 0, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Run", 0, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk", 0, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_End", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_Loop", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Blow_Start", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_End", 0, 0.1f, "Top_Jump_End"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_Loop", 0, 0.1f, "Top_Jump_Loop"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Jump_Start", 0, 0.1f, "Top_Jump_Start"));
	m_animation_manager->AddAnimation(AnimationDef("Base_PickUpLeaf_State", 0, 0.1f, "Top_PickUpLeaf_State"));
	m_animation_manager->AddAnimation(AnimationDef("Top_Idle", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Idle2", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Run", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Top_Walk", 1, 0.1f));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk_On_Bubble", 0, 0.1f, "Top_Walk_On_Bubble"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle_On_Bubble", 0, 0.1f, "Top_Idle_On_Bubble"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle_On_Bubble2", 0, 0.1f, "Top_Idle_On_Bubble2"));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk_On_Bubble2", 0, 0.1f, "Top_Walk_On_Bubble2"));
	m_animation_manager->AddAnimation(AnimationDef("Base_PickUpLeaf_State", 0, 0.1f, "Top_PickUpLeaf_State"));
	
	m_player_state_manager = new PlayerStateManager;

	PlayerState::Init(m_owner->GetComponentMessenger(), m_animation_manager, this, m_player_state_manager, sound_manager);
	BubblePropertiesDef free_blue_def;
	BubblePropertiesDef stand_on_blue_def;
	BubblePropertiesDef inside_blue_def;
	BubblePropertiesDef free_pink_def;
	BubblePropertiesDef stand_on_pink_def;
	BubblePropertiesDef inside_pink_def;
	free_blue_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Damping", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Gravity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Max_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Restitution", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	free_blue_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	
	stand_on_blue_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Damping", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Gravity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Max_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Restitution", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	stand_on_blue_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");

	inside_blue_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Damping", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Gravity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Max_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Restitution", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");
	inside_blue_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Velocity", INIFILE_PLAYER_VARIABLES, "BLUE_BUBBLE");

	// Pink definitions
	free_pink_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Damping", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	free_pink_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	free_pink_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Gravity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	free_pink_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Max_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	free_pink_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Restitution", INIFILE_PLAYER_VARIABLES, "BPINK_BUBBLE");
	free_pink_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	free_pink_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Free_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	
	stand_on_pink_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Damping", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Gravity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Max_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Restitution", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	stand_on_pink_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Stand_On_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");

	inside_pink_def.damping = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Damping", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.friction = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.gravity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Gravity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.max_velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Max_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.restitution = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Restitution", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.rolling_friction = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Rolling_Friction", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");
	inside_pink_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Inside_Velocity", INIFILE_PLAYER_VARIABLES, "PINK_BUBBLE");

	PlayerState::SetBubbleProperties(free_blue_def, stand_on_blue_def, inside_blue_def, free_pink_def, stand_on_pink_def, inside_pink_def);
	m_player_state_manager->AddPlayerState(new PlayerIdle);
	m_player_state_manager->AddPlayerState(new PlayerStateMove);
	m_player_state_manager->AddPlayerState(new PlayerBlowBubble(m_physics_engine));
	m_player_state_manager->AddPlayerState(new PlayerJump);
	m_player_state_manager->AddPlayerState(new PlayerFalling);
	m_player_state_manager->AddPlayerState(new PlayerLand);
	m_player_state_manager->AddPlayerState(new PlayerBounce);
	m_player_state_manager->AddPlayerState(new PlayerOnBubble(message_system));
	m_player_state_manager->AddPlayerState(new PlayerInsideBubble(physics_engine, message_system));
	m_player_state_manager->AddPlayerState(new PlayerHoldObject(physics_engine, message_system));
	m_player_state_manager->AddPlayerState(new PlayerLeafCollect(message_system));
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
bool PlayerInputComponent::IsInsideBubble(){
	return m_player_state_manager->IsInsideBubble();
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
			m_impulse_direction = *static_cast<Ogre::Vector3*>(msg);
			break;
		case MSG_BUBBLE_CONTROLLER_TIMER_RUN:
			//m_run_timer = *static_cast<bool*>(msg);
			break;
		case MSG_BUBBLE_CONTROLLER_ACTIVATE:
			{
				if (m_owner->GetType() == GAME_OBJECT_BLUE_BUBBLE){
					if (m_ready){
						btRigidBody* body = NULL;
						m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
						if (body){
							body->setLinearFactor(btVector3(1,1,1));
							m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_ACTIVATE, this);
						}
					}
				}
			}
			break;
		case MSG_BUBBLE_CONTROLLER_READY:
			{
				m_ready = true;
				Ogre::SceneNode* node = NULL;
				m_messenger->Notify(MSG_NODE_GET_NODE, &node);
				if (node){
					m_original_scale = node->getScale().y;
					m_max_scale = m_original_scale * 1.2f;
				}
			}
			break;
		case MSG_BUBBLE_CONTROLLER_PROPERTIES_SET:
			{
				BubblePropertiesDef& def = *static_cast<BubblePropertiesDef*>(msg);
				m_bubble_body->setFriction(def.friction);
				m_bubble_body->setRollingFriction(def.rolling_friction);
				m_bubble_body->setRestitution(def.restitution);
				m_bubble_body->setDamping(def.damping, btScalar(0.0f));
				m_bubble_body->setGravity(btVector3(0.0f, -def.gravity, 0.0f));
				m_velocity = def.velocity;
				m_max_velocity = def.max_velocity;
			}
			break;
	default:
		break;
	}
}

void BubbleController::Shut(){
	//SoundData2D pop_sound = m_owner->GetGameObjectManager()->GetSoundManager()->Create2DData("Bubble_Burst", false, false, false, false, 1.0, 1.0);
	//m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &pop_sound);

	BubbleEvent evt;
	evt.m_type = EVT_BUBBLE_REMOVE;
	evt.bubble = m_owner;
	m_message_system->Notify(&evt);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_ACTIVATE, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_TIMER_RUN, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_READY, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_PROPERTIES_SET, this);
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void BubbleController::Init(PhysicsEngine* physics_engine, MessageSystem* message_system, float velocity, float max_velocity, float scale){
	m_physics_engine = physics_engine;
	m_message_system = message_system;
	m_velocity = velocity;
	m_max_velocity = max_velocity;
	m_distance = VariableManager::GetSingletonPtr()->GetAsFloat("PinkBubbleDistance");
	m_max_distance = m_distance;
	m_original_scale = scale;
	m_max_scale = scale * 1.1f;
	m_scale_increment = 1.4f;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &m_bubble_body, "body");
	physics_engine->AddObjectSimulationStep(this);
}

void BubbleController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_TIMER_RUN, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_ACTIVATE, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_READY, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_PROPERTIES_SET, this);
}

void BubbleController::Update(float dt){
	if (m_apply_impulse){
		Ogre::Vector3 impulse = (m_impulse_direction * m_velocity) * dt;
		m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse, "body");
		m_apply_impulse = false;
		if (m_owner->GetType() == GAME_OBJECT_PINK_BUBBLE){
			m_distance -= impulse.squaredLength();
		}
	}
	if (m_owner->GetType() == GAME_OBJECT_PINK_BUBBLE){
		float percent = m_max_distance * 0.5f;
		if (m_distance < percent){
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				float scale_inc = (m_scale_increment * (percent / m_distance)) * dt;
				if (m_scale_state == 0) {    // increase size
					Ogre::Vector3 scale = node->getScale() + scale_inc;
					node->setScale(scale);
					if (node->getScale().y > m_max_scale){
						node->setScale(Ogre::Vector3(m_max_scale));
						m_scale_state = 1;
					}
				}
				else if (m_scale_state == 1){    // decrease size
					Ogre::Vector3 scale = node->getScale() - scale_inc;
					node->setScale(scale);
					if (node->getScale().y < m_original_scale){
						node->setScale(Ogre::Vector3(m_original_scale));
						m_scale_state = 0;
					}
				}
			}
		}
		if (m_distance <= 0.0f){
			SoundData2D pop_sound = m_owner->GetGameObjectManager()->GetSoundManager()->Create2DData("Bubble_Burst", false, false, false, false, 1.0, 1.0);
			m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &pop_sound);
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