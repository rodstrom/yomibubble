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
#include <iostream>
#include "..\Managers\AnimationManager.h"
#include "..\AnimPrereq.h"
#include "..\Managers\PlayerStateManager.h"
#include "PlayerState.h"

void PlayerInputComponent::Update(float dt){
	m_direction.x = m_input_manager->GetMovementAxis().x;
	m_direction.z = m_input_manager->GetMovementAxis().z;
	m_player_state_manager->Update(dt);
	//(this->*m_states[m_player_state])(dt);
	
	if (m_level == "try"){ m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music); }
	else if (m_level == "Dayarea") { m_messenger->Notify(MSG_SFX2D_PLAY, &m_day_music); }
	else if (m_level == "NightArea") { m_messenger->Notify(MSG_SFX2D_PLAY, &m_night_music); }
	else { m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music); }
	
	//m_messenger->Notify(MSG_SFX2D_PLAY, &m_start_music);
}

inline void PlayerInputComponent::Jump(float dt){
	/*if (m_input_manager->IsButtonPressed(BTN_A)){
		bool is_jumping = true;
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &is_jumping);
	}
	else if (m_input_manager->IsButtonReleased(BTN_A)){
		bool is_jumping = false;
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &is_jumping);
	}*/
}

void PlayerInputComponent::SetCustomVariables(float min_bubble_size, float max_bubble_size, float on_bubble_mod, float in_bubble_mod, Ogre::String level){
	m_min_bubble_size = min_bubble_size;
	m_max_bubble_size = max_bubble_size;
	m_on_bubble_speed_mod = on_bubble_mod;
	m_in_bubble_speed_mod = in_bubble_mod;
	m_level = level;
};

void PlayerInputComponent::OntoBubbleTransition(float dt){
	/*if (m_current_bubble){
		btRigidBody* body = NULL;
		btRigidBody* bubble_body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
		m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (body && bubble_body){
			Ogre::Vector3 bubble_pos = BtOgre::Convert::toOgre(bubble_body->getWorldTransform().getOrigin());
			Ogre::Vector3 player_pos = BtOgre::Convert::toOgre(body->getWorldTransform().getOrigin());
			float distance = bubble_pos.squaredDistance(player_pos);
			if (distance < 0.01f){
				int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				m_player_state = PLAYER_STATE_ON_BUBBLE;
				m_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
			}
			else {
				float speed = 5.0f * dt;
				Ogre::Vector3 dir = bubble_pos - player_pos;
				//dir.y = 0.0f;
				dir.normalise();
				player_pos += (dir * speed);
				m_messenger->Notify(MSG_SET_OBJECT_POSITION, &player_pos);
				body->setLinearVelocity(btVector3(0,0,0));
			}
		}
	}*/
}

void PlayerInputComponent::IntoBubbleTransition(float dt){
	/*if (m_current_bubble){
		btRigidBody* body = NULL;
		btRigidBody* bubble_body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
		m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (body && bubble_body){
			Ogre::Vector3 bubble_pos = BtOgre::Convert::toOgre(bubble_body->getWorldTransform().getOrigin());
			Ogre::Vector3 player_pos = BtOgre::Convert::toOgre(body->getWorldTransform().getOrigin());
			float distance = bubble_pos.squaredDistance(player_pos);
			if (distance < 0.005f){
				m_player_state = PLAYER_STATE_INSIDE_BUBBLE;
			}
			else {
				float speed = 5.0f * dt;
				Ogre::Vector3 dir = bubble_pos - player_pos;
				dir.normalise();
				player_pos += dir * speed;
				m_messenger->Notify(MSG_SET_OBJECT_POSITION, &player_pos);
				body->setLinearVelocity(btVector3(0,0,0));
			}
		}
	}*/
}

inline void PlayerInputComponent::BlowBubble(float dt){
	/*if (!m_is_creating_bubble){
		m_messenger->Notify(MSG_SFX2D_STOP, &m_bubble_blow_sound);
		Ogre::SceneNode* node = NULL;
		if (m_input_manager->IsButtonPressed(BTN_LEFT_MOUSE) || m_input_manager->IsButtonPressed(BTN_RIGHT_MOUSE)){// && m_current_bubble != NULL){
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			btRigidBody* body = NULL;
			m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "btrig");
			if (node && body){
				Ogre::Vector3 pos = node->_getDerivedPosition();
				//pos.y += 4;
				BubbleDef bubble_def;
				bubble_def.connection_body = body;
				bubble_def.start_scale = 0.002f;
				bubble_def.friction = 1.0f;
				bubble_def.restitution = 0.2f;
				if (m_input_manager->IsButtonPressed(BTN_LEFT_MOUSE)){
					m_current_bubble = m_owner->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_BLUE_BUBBLE, pos, &bubble_def);
					m_bubble_type = BUBBLE_TYPE_BLUE;
				}
				else if (m_input_manager->IsButtonPressed(BTN_RIGHT_MOUSE)){
					m_current_bubble = m_owner->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_PINK_BUBBLE, pos, &bubble_def);
					m_bubble_type = BUBBLE_TYPE_PINK;
				}
				m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");
				m_is_creating_bubble = true;
			}
		}
	}
	else{
		m_messenger->Notify(MSG_SFX2D_PLAY, &m_bubble_blow_sound);

		const float SCALE = 0.91f * dt;
		Ogre::Vector3 scale_inc;//(SCALE);
		if (m_current_scale < m_min_bubble_size){
			m_current_scale += SCALE * 2;
			scale_inc = Ogre::Vector3((SCALE * 2));
		}
		else if (m_current_scale >= m_min_bubble_size
			&& m_current_scale < m_max_bubble_size){
			m_current_scale += SCALE;
			scale_inc = Ogre::Vector3(SCALE);
		}
		if (m_current_scale > m_max_bubble_size){
			if (m_current_bubble != NULL)
			{
				m_owner->GetGameObjectManager()->RemoveGameObject(m_current_bubble);
				m_current_bubble = NULL;
				//insert pop sound here
				m_current_scale = 0.0f;
				m_is_creating_bubble = false;
			}
		}
		if (m_bubble_type == BUBBLE_TYPE_BLUE && m_input_manager->IsButtonReleased(BTN_LEFT_MOUSE) && m_current_bubble != NULL){
			if (m_current_scale < m_min_bubble_size){
				m_current_scale = m_min_bubble_size;
				Ogre::Vector3 scale_incr(m_current_scale);
				m_current_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_incr);
			}
			this->CreateTriggerForBubble();
			Ogre::Vector3 gravity(0,-5.8f,0);
			m_current_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
			m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble = NULL;
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && m_input_manager->IsButtonReleased(BTN_RIGHT_MOUSE) && m_current_bubble != NULL){
			if (m_current_scale < m_min_bubble_size){
				m_current_scale = m_min_bubble_size;
				Ogre::Vector3 scale_incr(m_current_scale);
				m_current_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_incr);
			}
			this->CreateTriggerForBubble();
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
			m_current_bubble = NULL;
		}

		if (m_input_manager->IsButtonDown(BTN_LEFT_MOUSE) && m_current_bubble != NULL || m_input_manager->IsButtonDown(BTN_RIGHT_MOUSE) && m_current_bubble != NULL){
			Ogre::SceneNode* player_node = NULL;
			Ogre::SceneNode* child_node = NULL;
			Ogre::SceneNode* bubble_node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &player_node);
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &child_node);
			m_current_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
			if (m_current_bubble != NULL)
			{
				if (child_node && bubble_node && player_node){
					Ogre::Vector3 pos = child_node->_getDerivedPosition();
					Ogre::Vector3 dir = pos - player_node->getPosition();
					dir.normalise();
					float scale_size = (bubble_node->getScale().length() * 0.5f);
					pos += (dir*scale_size);
					m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");		// btrig is the ID for the TriggerCompoent
					m_current_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_inc);
				}
			}
		}
	}*/
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

	if (m_level == "try"){ m_messenger->Notify(MSG_SFX2D_STOP, &m_start_music); }
	else if (m_level == "Dayarea") { m_messenger->Notify(MSG_SFX2D_STOP, &m_day_music); }
}

void PlayerInputComponent::Init(InputManager* input_manager, SoundManager* sound_manager, PhysicsEngine* physics_engine){
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
	m_animation_manager->AddAnimation(AnimationDef("Base_Idle", 0, false));
	m_animation_manager->AddAnimation(AnimationDef("Base_Run", 0, false));
	m_animation_manager->AddAnimation(AnimationDef("Base_Walk", 0, false));
	m_animation_manager->AddAnimation(AnimationDef("Blow_End", 1, false));
	m_animation_manager->AddAnimation(AnimationDef("Blow_Loop", 1, false));
	m_animation_manager->AddAnimation(AnimationDef("Blow_Start", 1, false));
	m_animation_manager->AddAnimation(AnimationDef("Jump_End", 0, true));
	m_animation_manager->AddAnimation(AnimationDef("Jump_Loop", 0, true));
	m_animation_manager->AddAnimation(AnimationDef("Jump_Start", 0, true));
	m_animation_manager->AddAnimation(AnimationDef("PickUpLeaf_State", 0, true));
	m_animation_manager->AddAnimation(AnimationDef("Top_Idle", 1, false));
	m_animation_manager->AddAnimation(AnimationDef("Top_Run", 1, false));
	m_animation_manager->AddAnimation(AnimationDef("Top_Walk", 1, false));
	m_player_state_manager = new PlayerStateManager;

	PlayerState::Init(m_owner->GetComponentMessenger(), m_animation_manager, this, m_player_state_manager, sound_manager);
	m_player_state_manager->AddPlayerState(new PlayerIdle);
	m_player_state_manager->AddPlayerState(new PlayerStateMove);
	m_player_state_manager->AddPlayerState(new PlayerBlowBubble);
	m_player_state_manager->AddPlayerState(new PlayerJump);
	m_player_state_manager->AddPlayerState(new PlayerFalling);
	m_player_state_manager->AddPlayerState(new PlayerLand);
	m_player_state_manager->AddPlayerState(new PlayerBounce);
	m_player_state_manager->AddPlayerState(new PlayerOnBubble);
	m_player_state_manager->AddPlayerState(new PlayerInsideBubble);
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
	/*Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	m_direction.x = m_input_manager->GetMovementAxis().x;
	m_direction.z = m_input_manager->GetMovementAxis().z;

	if (dir != Ogre::Vector3::ZERO){
		m_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
		if (!m_on_ground){
			m_animation_manager->PlayAnimation("Jump_Loop");
			m_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
		}
		else {
			m_animation_manager->PlayAnimation("Base_Run");
			if (!m_is_creating_bubble){
				m_animation_manager->PlayAnimation("Top_Run");
			}
			else {
				m_animation_manager->PlayAnimation("Blow_Loop");
			}
		}
	}
	else
	{
		m_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
		if (!m_on_ground){
			m_animation_manager->PlayAnimation("Jump_Loop");
		}
		else {
			m_animation_manager->PlayAnimation("Base_Idle");
			if (!m_is_creating_bubble){
				m_animation_manager->PlayAnimation("Top_Idle");
			}
			else {
				m_animation_manager->PlayAnimation("Blow_Loop");
			}
		}
	}
	Jump(dt);
	BlowBubble(dt);

	m_camera_data_def.player_direction = Ogre::Vector3(dir.x, 0.0f, dir.z);
	m_messenger->Notify(MSG_DEFAULT_CAMERA_POS, &m_camera_data_def);

	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
	m_messenger->Notify(MSG_START_TIMER, NULL);*/
}

void PlayerInputComponent::OnBubble(float dt){
	/*Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	Ogre::SceneNode* bubble_node = NULL;
	m_current_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	btRigidBody* player_body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	if (bubble_node){
		Ogre::Vector3 bubble_scale = bubble_node->getScale();
		btRigidBody* bubble_body = NULL;
		m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (player_body && bubble_body){
			player_body->setLinearVelocity(btVector3(0,0,0));
			btVector3 pos = BtOgre::Convert::toBullet(bubble_node->getPosition());
			pos.setY(pos.y() + bubble_scale.y * 0.5f + 0.01f);
			player_body->getWorldTransform().setOrigin(pos);
		}
	}
	if (m_input_manager->IsButtonPressed(BTN_A)){
		m_player_state = PLAYER_STATE_NORMAL;
		Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
		bool jump = true;
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		m_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
		//m_owner->RemoveComponent(COMPONENT_HINGE_CONSTRAINT, true);
		if (player_body){
			player_body->setLinearFactor(btVector3(1,1,1));
		}
		return;
	}

	float speed = m_on_bubble_speed_mod * dt;
	bool follow_cam = true;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, &follow_cam);
	if (follow_cam){
		if (bubble_node){
			m_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
			dir.normalise();
			m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		}
	}
	else{
		m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
	}*/
}

void PlayerInputComponent::InsideBubble(float dt){
	/*Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	Ogre::SceneNode* bubble_node = NULL;
	m_current_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	if (bubble_node){
		btRigidBody* player_body = NULL;
		btRigidBody* bubble_body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
		m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (player_body && bubble_body){
			player_body->setLinearVelocity(btVector3(0,0,0));
			btScalar y_pos = bubble_body->getWorldTransform().getOrigin().y() - player_body->getWorldTransform().getOrigin().y();
			btVector3 pos = BtOgre::Convert::toBullet(bubble_node->getPosition());
			player_body->getWorldTransform().setOrigin(pos);
		}
	}
	if (m_input_manager->IsButtonPressed(BTN_A)){
		m_player_state = PLAYER_STATE_NORMAL;
		int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		bool jump = true;
		m_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
		return;
	}

	float speed = m_in_bubble_speed_mod * dt;
	bool follow_cam = false;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, &follow_cam);
	if (follow_cam){
		if (bubble_node){
			m_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
			dir.normalise();
			m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		}
	}
	else{
		m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
	}*/
}

void PlayerInputComponent::Bouncing(float dt){
	/*Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
	//m_messenger->Notify(MSG_SFX2D_PLAY, &m_bounce_sound);*/
}

void PlayerInputComponent::CreateTriggerForBubble(){
	/*Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		TriggerDef trdef;
		trdef.body_type = STATIC_BODY;
		trdef.collider_type = COLLIDER_BOX;
		trdef.mass = 1.0f;
		float size = m_current_scale * 0.5f;
		trdef.x = size;
		trdef.z = size;
		trdef.y = 0.01f;
		trdef.offset.y = size;
		trdef.collision_filter.filter = COL_BUBBLE_TRIG;
		trdef.collision_filter.mask = COL_PLAYER;
		Ogre::Vector3 pos = node->_getDerivedPosition();
		m_current_bubble->CreateComponent(COMPONENT_SYNCED_TRIGGER, pos, &trdef);
	}*/
}

void PlayerInputComponent::SimulationStep(btScalar time_step){
	/*if (m_player_state == PLAYER_STATE_INSIDE_BUBBLE || m_player_state == PLAYER_STATE_ON_BUBBLE){
		if (m_current_bubble){
			Ogre::Vector3 bubble_pos = Ogre::Vector3::ZERO;
			btRigidBody* player_body = NULL;
			m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
			m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_POSITION_GET, &bubble_pos, "body");
			m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &bubble_pos, "btrig");
			if (player_body){
				btVector3 vel = player_body->getLinearVelocity();
				player_body->setLinearVelocity(btVector3(vel.x(), 0.0f, vel.z()));
			}
		}
	}*/
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
	default:
		break;
	}
}

void BubbleController::Shut(){
	SoundData2D pop_sound = m_owner->GetGameObjectManager()->GetSoundManager()->Create2DData("Bubble_Burst", false, false, false, false, 1.0, 1.0);
	m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &pop_sound);

	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET, this);
	m_messenger->Unregister(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET, this);
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void BubbleController::Init(PhysicsEngine* physics_engine, float velocity, float max_velocity){
	m_physics_engine = physics_engine;
	m_velocity = velocity;
	m_max_velocity = max_velocity;
	physics_engine->AddObjectSimulationStep(this);
}

void BubbleController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET, this);
	m_messenger->Register(MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET, this);
}

/*void BubbleController::ApplyImpulse(const btVector3& dir){
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
	if (body){
		body->applyCentralImpulse(btVector3(dir.x(), 0.0f, dir.z()));
	}
}*/

void BubbleController::Update(float dt){
	/*
	m_time_counter += dt;

	if (m_time_counter >= m_time_counter){
		m_owner->GetGameObjectManager()->RemoveGameObject(m_owner);
		//add pop sound
	}
	*/
	if (m_apply_impulse){
		Ogre::Vector3 impulse = (m_impulse * 8.0f) * dt;
		m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse, "body");
		m_apply_impulse = false;
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