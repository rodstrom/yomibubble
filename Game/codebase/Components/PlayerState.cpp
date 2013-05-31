#include "stdafx.h"
#include "PlayerState.h"
#include "..\Managers\AnimationManager.h"
#include "ComponentMessenger.h"
#include "PlayerInputComponent.h"
#include "..\Managers\PlayerStateManager.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"
#include "..\MessageSystem.h"
#include "..\Managers\GameObjectManager.h"
#include "..\Managers\VariableManager.h"
#include "..\PhysicsPrereq.h"
#include "..\BtOgreGP.h"
#include "..\AnimationBlender.h"
#include "..\PhysicsEngine.h"
#include "..\RaycastCollision.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "..\ExtraMath.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

ComponentMessenger* PlayerState::s_messenger = NULL;
AnimationManager* PlayerState::s_animation = NULL;
PlayerInputComponent* PlayerState::s_input_component = NULL;
PlayerStateManager* PlayerState::s_manager = NULL;
SoundManager* PlayerState::s_sound_manager = NULL;
BubblePropertiesDef PlayerState::s_free_blue_bubble_def;
BubblePropertiesDef PlayerState::s_stand_on_blue_bubble_def;
BubblePropertiesDef PlayerState::s_inside_blue_bubble_def;
BubblePropertiesDef PlayerState::s_free_pink_bubble_def;
BubblePropertiesDef PlayerState::s_stand_on_pink_bubble_def;
BubblePropertiesDef PlayerState::s_inside_pink_bubble_def;

PlayerIdle::PlayerIdle(void){ 
	m_type = PLAYER_STATE_IDLE; 
}

void PlayerIdle::Enter(){
	m_timer = 0.0f;
	m_target_time = (float)Ogre::Math::RangeRandom(Ogre::Real(1.0f), Ogre::Real(3.0f));
	m_current_idle_base = "Base_Idle";
	m_current_idle_top = "Top_Idle";
	s_animation->PlayAnimation(m_current_idle_base);
}

void PlayerIdle::Exit(){
	
}

void PlayerIdle::Update(float dt){
	if (!s_manager->IsBlowingBubbles() && !s_manager->IsHoldingObject()){
		s_animation->PlayAnimation(m_current_idle_top);
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || (s_input_component->CanBlowPink() && s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE))){
			if (!s_manager->IsHoldingObject() && !s_manager->IsInsideBubble()){
				s_manager->BlowBubble(true);
			}
		}
	}
	s_animation->PlayAnimation(m_current_idle_base);
	m_timer += dt;
	if (m_timer >= m_target_time){
		if (m_current_idle_base == "Base_Idle"){
			m_current_idle_base = "Base_Idle2";
			m_current_idle_top = "Top_Idle2";
			m_timer = 0.0f;
			m_target_time = 6.6f;
		}
		else {
			m_current_idle_base = "Base_Idle";
			m_current_idle_top = "Top_Idle";
			m_timer = 0.0f;
			m_target_time = (float)Ogre::Math::RangeRandom(Ogre::Real(2.0f), Ogre::Real(4.0f));
		}
	}

	Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir != Ogre::Vector3::ZERO){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_MOVE));	// if the character is moving, change to move state
	}

	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
		if (s_manager->IsBlowingBubbles()){
			s_manager->BlowBubble(false);
		}
		else if (s_manager->IsInsideBubble()){
			if (s_manager->GetInsideBubbleState()->GetBubbleObject()->GetType() == GAME_OBJECT_PINK_BUBBLE){
				s_manager->GoInsideBubble(false);
				bool on_ground = true;
				s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &on_ground);
			}
		}
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (s_manager->IsInsideBubble()){
		if (s_manager->GetInsideBubbleState()->GetBubbleObject()->GetType() == GAME_OBJECT_PINK_BUBBLE){
			on_ground = true;
		}
	}
	if (!on_ground){
		if (s_manager->IsBlowingBubbles()){
			s_manager->BlowBubble(false);
		}
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X) && !s_manager->IsBlowingBubbles() && !s_manager->IsHoldingObject() && !s_manager->IsInsideBubble()){
		s_input_component->GetInputManager()->InjectReleasedButton(BTN_X);
		s_manager->HoldObject(true);
	}
	else if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X) && s_manager->IsInsideBubble()){
		s_manager->GoInsideBubble(false);
	}
	if (!s_manager->IsInsideBubble()){
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
	}
}
PlayerStateMove::PlayerStateMove(void){ 
	m_type = PLAYER_STATE_MOVE;
	m_walk_sound = s_sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
}

void PlayerStateMove::Enter(){
	s_animation->PlayAnimation("Base_Walk");
	s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("Stick"));
}

void PlayerStateMove::Exit(){
	s_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
}
void PlayerStateMove::PlayTopAnimation(){

}

void PlayerStateMove::Update(float dt){
	s_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
	Ogre::Vector3 dir = s_input_component->GetDirection();
	float speed = dir.squaredLength();
	Ogre::String move_bot = Ogre::StringUtil::BLANK;
	Ogre::String move_top = Ogre::StringUtil::BLANK;
	if (speed > 0.6f){
		move_top = "Top_Run";
		move_bot = "Base_Run";
	}
	else {
		move_top = "Top_Walk";
		move_bot = "Base_Walk";
	}

	s_animation->PlayAnimation(move_bot);
	if (!s_manager->IsBlowingBubbles() && !s_manager->IsHoldingObject()){
		s_animation->PlayAnimation(move_top);
	}

	if (!s_manager->IsBlowingBubbles() && !s_manager->IsHoldingObject() && !s_manager->IsInsideBubble()){
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || (s_input_component->CanBlowPink() && s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE))){
			if (!s_manager->IsHoldingObject() && !s_manager->IsInsideBubble()){
				s_manager->BlowBubble(true);
			}
		}
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
		if (s_manager->IsBlowingBubbles()){
			s_manager->BlowBubble(false);
		}
		else if (s_manager->IsInsideBubble()){
			if (s_manager->GetInsideBubbleState()->GetBubbleObject()->GetType() == GAME_OBJECT_PINK_BUBBLE){
				s_manager->GoInsideBubble(false);
				bool on_ground = true;
				s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &on_ground);
			}
		}
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (s_manager->IsInsideBubble()){
		if (s_manager->GetInsideBubbleState()->GetBubbleObject()->GetType() == GAME_OBJECT_PINK_BUBBLE){
			on_ground = true;
		}
	}
	if (!on_ground){
		if (s_manager->IsBlowingBubbles()){
			s_manager->BlowBubble(false);
		}
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	if (dir == Ogre::Vector3::ZERO){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));	// if the character is not moving, change to idle
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X) && !s_manager->IsBlowingBubbles() && !s_manager->IsHoldingObject() && !s_manager->IsInsideBubble()){
		s_input_component->GetInputManager()->InjectReleasedButton(BTN_X);
		s_manager->HoldObject(true);
	}
	else if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X) && s_manager->IsInsideBubble()){
		s_manager->GoInsideBubble(false);
	}
	if (!s_manager->IsInsideBubble()){
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
	}
}

PlayerBlowBubble::PlayerBlowBubble(PhysicsEngine* physics_engine) : m_bubble(NULL), m_current_scale(0.0f), m_physics_engine(physics_engine) {
	m_type = PLAYER_STATE_BLOW_BUBBLE;
	m_bubble_blow_sound = s_sound_manager->Create2DData("Blow_Bubble", false, false, false, false, 1.0f, 1.0f);
	m_min_bubble_size = VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Min_Size");
	m_max_bubble_size = VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Max_Size");
	m_bubble_gravity = VariableManager::GetSingletonPtr()->GetAsFloat("BlueBubbleGravity");
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &m_player_body, "body");
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &m_trigger_body, "btrig");
	s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &m_child_node);
	s_messenger->Notify(MSG_NODE_GET_NODE, &m_player_node);
}

void PlayerBlowBubble::Enter(){
	m_hit_wall = false;
	Ogre::Vector3 from = m_player_node->getPosition();
	Ogre::Vector3 to = m_child_node->_getDerivedPosition();
	from.y += 0.5f;		// player offset in Y from pivot
	IgnoreBodyCast ray(m_player_body);
	m_physics_engine->GetDynamicWorld()->rayTest(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), ray);


	btRigidBody* body = NULL;
	Ogre::SceneNode* node = NULL;
	Ogre::SceneNode* child_node = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "btrig");
	s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
	Ogre::Vector3 pos = node->_getDerivedPosition();
	BubbleDef bubble_def;
	bubble_def.connection_body = body;
	bubble_def.start_scale = 0.002f;
	bubble_def.friction = 0.5f;
	bubble_def.restitution = 0.2f;
	if (ray.hasHit()){
		Ogre::Vector3 lerp = Lerp(from, to, ray.m_closestHitFraction - 0.1f);	// 0.1f is a slight offset to put the target position closer to the player
		pos = lerp;
		m_hit_wall = true;
	}
	if (s_input_component->GetInputManager()->IsButtonDown(BTN_RIGHT_MOUSE)){
		m_bubble = s_input_component->GetOwner()->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_PINK_BUBBLE, pos, &bubble_def);
		m_bubble_type = BUBBLE_TYPE_PINK;
		s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("PinkBubble"));
	}
	else if (s_input_component->GetInputManager()->IsButtonDown(BTN_LEFT_MOUSE)){
		m_bubble = s_input_component->GetOwner()->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_BLUE_BUBBLE, pos, &bubble_def);
		m_bubble_type = BUBBLE_TYPE_BLUE;
		s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("BlueBubble"));
	}

	s_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");
	s_messenger->Notify(MSG_SFX2D_PLAY, &m_bubble_blow_sound);
	//s_animation->PlayAnimation("Top_Blow_Start");
}

void PlayerBlowBubble::Exit(){
	if (m_bubble_type == BUBBLE_TYPE_BLUE){
		if (m_current_scale < m_min_bubble_size){
			m_current_scale = m_min_bubble_size;
			Ogre::Vector3 scale_incr(m_current_scale);
			m_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_incr);
		}
		btRigidBody* bubble_body = NULL;
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
		bubble_body->setLinearVelocity(btVector3(0,0,0));
		bubble_body->setLinearFactor(btVector3(0,1,0));
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_READY, NULL);
		this->CreateTriggerForBubble();
		Ogre::Vector3 gravity(0,-m_bubble_gravity,0);
		m_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
		m_current_scale = 0.0f;
		m_bubble = NULL;
	}
	else if (m_bubble_type == BUBBLE_TYPE_PINK){
		if (m_current_scale < m_min_bubble_size){
			m_current_scale = m_min_bubble_size;
			Ogre::Vector3 scale_incr(m_current_scale);
			m_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_incr);
		}
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_READY, NULL);
		btRigidBody* bubble_body = NULL;
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
		bubble_body->setLinearVelocity(btVector3(0,0,0));
		bubble_body->setLinearFactor(btVector3(1,0,1));
		this->CreateTriggerForBubble();
		m_current_scale = 0.0f;
		m_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
		m_bubble = NULL;
	}

	s_messenger->Notify(MSG_SFX2D_STOP, &m_bubble_blow_sound);
	s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
}

void PlayerBlowBubble::Update(float dt){
	s_animation->PlayAnimation("Top_Blow_Loop");

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
			if (m_bubble != NULL)
			{
				s_input_component->GetOwner()->GetGameObjectManager()->RemoveGameObject(m_bubble);
				//m_bubble = NULL;
				//insert pop sound here
				SoundData2D pop_sound = s_sound_manager->Create2DData("Bubble_Burst", false, false, false, false, 1.0, 1.0);
				s_messenger->Notify(MSG_SFX2D_PLAY, &pop_sound);
				m_current_scale = 0.0f;
				s_manager->BlowBubble(false);
			}
		}
		if (m_bubble_type == BUBBLE_TYPE_BLUE && s_input_component->GetInputManager()->IsButtonReleased(BTN_LEFT_MOUSE) && m_bubble != NULL){
			s_manager->BlowBubble(false);
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && s_input_component->GetInputManager()->IsButtonReleased(BTN_RIGHT_MOUSE) && m_bubble != NULL){
			s_manager->BlowBubble(false);
		}

		if (s_input_component->GetInputManager()->IsButtonDown(BTN_LEFT_MOUSE) && m_bubble != NULL || s_input_component->GetInputManager()->IsButtonDown(BTN_RIGHT_MOUSE) && m_bubble != NULL){
			Ogre::SceneNode* bubble_node = NULL;
			m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
			if (m_bubble != NULL) {
				if (bubble_node){
					Ogre::Vector3 child_pos(m_child_node->_getDerivedPosition());
					Ogre::Vector3 player_pos(m_player_node->_getDerivedPosition());
					Ogre::Vector3 dir = child_pos - player_pos;
					dir.normalise();
					float scale_size = (bubble_node->getScale().length() * 0.3f);
					if (m_hit_wall){
						btRigidBody* bubble_body = NULL;
						m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
						Ogre::Vector3 from = m_player_node->getPosition();
						Ogre::Vector3 to = m_child_node->_getDerivedPosition();
						from.y += 0.5f;		// player offset in Y from pivot
						IgnoreBodyAndBubbleCast ray(m_player_body, bubble_body);
						m_physics_engine->GetDynamicWorld()->rayTest(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), ray);
						if (ray.hasHit()){
							Ogre::Vector3 lerp = Lerp(from, to, ray.m_closestHitFraction - 0.1f);	// 0.1f is a slight offset to put the target position closer to the player
							child_pos = lerp;
						}
					}
					child_pos += (dir*scale_size);
					Ogre::Vector3 new_dir(child_pos);
					s_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &new_dir, "btrig");		// btrig is the ID for the TriggerCompoent
					m_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_inc);
				}
			}
		}
}

void PlayerBlowBubble::CreateTriggerForBubble(){
	Ogre::SceneNode* node = NULL;
	btRigidBody* body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
	s_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node && body){
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
		m_bubble->CreateComponent(COMPONENT_SYNCED_TRIGGER, pos, &trdef);
		if (m_bubble_type == BUBBLE_TYPE_PINK){
			body->setLinearFactor(btVector3(1,0,1));
		}
	}
}

PlayerJump::PlayerJump(void){ 
	m_type = PLAYER_STATE_JUMP;
	m_jump_sfx_1 = s_sound_manager->Create2DData("Yomi_Jump_1", false, false, false, false, 1.0f, 1.0f);
	m_jump_sfx_2 = s_sound_manager->Create2DData("Yomi_Jump_2", false, false, false, false, 1.0f, 1.0f);
	m_jump_sfx_3 = s_sound_manager->Create2DData("Yomi_Jump_3", false, false, false, false, 1.0f, 1.0f);
}

void PlayerJump::Enter(){
	s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("Jump"));
	//int p = 1;
	//s_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
	s_animation->PlayAnimation("Base_Jump_Start", false, AnimationBlender::BlendSwitch);
	//s_animation->PlayAnimation("Base_Jump_Loop", true, AnimationBlender::BlendSwitch);
	bool jump = true;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);

	//randomize IF a jump sound should play: if true, randomize a specific jump sound
	srand (time(NULL));
	int play_jump_sound = rand() % 10 + 1;
	if (play_jump_sound < 7){
		int which_jump_sound = rand() % 3 + 1;
		switch(which_jump_sound){
		case 1:
			s_messenger->Notify(MSG_SFX2D_PLAY, &m_jump_sfx_1);
			break;
		case 2:
			s_messenger->Notify(MSG_SFX2D_PLAY, &m_jump_sfx_2);
			break;
		case 3:
			s_messenger->Notify(MSG_SFX2D_PLAY, &m_jump_sfx_3);
			break;
		default:
			break;
		}
	}
	else{}

}

void PlayerJump::Exit(){

}

void PlayerJump::Update(float dt){
	s_animation->PlayAnimation("Base_Jump_Loop");
	btRigidBody* body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	float y_vel = body->getLinearVelocity().y();
	if (y_vel < 0.0f){
		bool jump = false;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	else if (s_input_component->GetInputManager()->IsButtonReleased(BTN_A)){
		bool jump = false;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_LAND));
	}
	if (s_manager->IsInsideBubble() && s_input_component->GetInputManager()->IsButtonPressed(BTN_X)){
		s_manager->GoInsideBubble(false);
	}

	Ogre::Vector3 dir = s_input_component->GetDirection();
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

void PlayerFalling::Enter(){
	s_animation->PlayAnimation("Base_Jump_Loop");
}

void PlayerFalling::Exit(){

}

void PlayerFalling::Update(float dt){
	bool on_ground = s_input_component->IsOnGround();
	if (on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
	}
	if (s_manager->IsInsideBubble() && s_input_component->GetInputManager()->IsButtonPressed(BTN_X)){
		s_manager->GoInsideBubble(false);
	}
	Ogre::Vector3 dir = s_input_component->GetDirection();
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

PlayerLand::PlayerLand(){
	m_type = PLAYER_STATE_LAND;
	m_land_sfx_1 = s_sound_manager->Create2DData("Yomi_Land_1", false, false, false, false, 1.0f, 1.0f);
	m_land_sfx_2 = s_sound_manager->Create2DData("Yomi_Land_2", false, false, false, false, 1.0f, 1.0f);
}

void PlayerLand::Enter(){
	//s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
	//s_animation->PlayAnimation("Base_Jump_End");
	//std::function<void()> func = [this] { Proceed(); };
	//s_messenger->Notify(MSG_ANIMATION_CALLBACK, &func);

	srand (time(NULL));
	int play_land_sound = rand() % 10 + 1;
	if (play_land_sound < 6){
		int which_land_sound = rand() % 2 + 1;
		switch(which_land_sound){
		case 1:
			s_messenger->Notify(MSG_SFX2D_PLAY, &m_land_sfx_1);
			break;
		case 2:
			s_messenger->Notify(MSG_SFX2D_PLAY, &m_land_sfx_2);
			break;
		default:
			break;
		}
	}
	else{}
}

void PlayerLand::Exit(){

}

void PlayerLand::Update(float dt){
	Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir != Ogre::Vector3::ZERO){
		//s_messenger->Notify(MSG_ANIMATION_CLEAR_CALLBACK, NULL);
		int p = 0;
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_MOVE));
	}
	else {
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
	}
	AnimIsDoneMsg anim_msg(0, false);
	s_messenger->Notify(MSG_ANIMATION_IS_DONE, &anim_msg);
	if (anim_msg.is_done){
		int p = 0;
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
	}
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

void PlayerLand::Proceed(){
	/*Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir == Ogre::Vector3::ZERO){
		int p = 1;
		s_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
	}*/
}

PlayerOnBubble::PlayerOnBubble(MessageSystem* message_system) : m_on_bubble_y_offset(0.0f), m_message_system(message_system),
	m_timer(0.0f), m_target_time(0.0f) { 
	m_on_bubble_y_offset = 0.05f;
	m_type = PLAYER_STATE_ON_BUBBLE; 
	m_message_system->Register(EVT_BUBBLE_REMOVE, this, &PlayerOnBubble::BubbleRemoved);
}

PlayerOnBubble::~PlayerOnBubble(void){
	m_message_system->Unregister(EVT_BUBBLE_REMOVE, this);
}

void PlayerOnBubble::Enter(){
	m_timer = 0.0f;
	m_target_time = (float)Ogre::Math::RangeRandom(Ogre::Real(1.0f), Ogre::Real(3.0f));
	m_bubble = s_input_component->GetBubble();
	Ogre::SceneNode* bubble_node = NULL;
	btRigidBody* player_body = NULL;
	btRigidBody* bubble_body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	if (player_body && bubble_body && bubble_node){
		float y_scale = bubble_node->getScale().y;
		y_scale *= -0.5f;
		Generic6DofDef def;
		def.body_a = bubble_body;
		def.body_b = player_body;
		def.pivot_b.setY(y_scale + m_on_bubble_y_offset);
		btScalar mass = 0.0f;
		btVector3 inertia(0,0,0);
		s_input_component->GetOwner()->CreateComponent(COMPONENT_GENERIC_6DOF_COMPONENT, Ogre::Vector3(0,0,0), &def);
		bool limit = false;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
	}
	if (m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
		bool start = true;
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
	}
	m_current_idle = "Base_Idle_On_Bubble";
	m_current_walk = "Base_Walk_On_Bubble";
	s_animation->PlayAnimation("Base_Jump_End", false);
}

void PlayerOnBubble::Exit(){
	if (m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
		bool start = false;
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
	}
	m_bubble = NULL;
}

void PlayerOnBubble::Update(float dt){
	btRigidBody* trigger_body = NULL;
	btRigidBody* player_body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &trigger_body, "btrig");
	btRigidBody* bubble_body = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
	Ogre::Vector3 dir = s_input_component->GetDirection();
	Ogre::String move_base = Ogre::StringUtil::BLANK;
	Ogre::String move_top = Ogre::StringUtil::BLANK;
	bool is_moving = false;
	if (dir == Ogre::Vector3::ZERO) {
		move_base = m_current_idle;
	}
	else {
		is_moving = true;
		move_base = m_current_walk;
	}
	s_animation->PlayAnimation(move_base);
	m_timer += dt;
	if (m_timer >= m_target_time){
		if (is_moving){
			if (m_current_walk == "Base_Walk_On_Bubble"){
				m_current_walk = "Base_Walk_On_Bubble2";
				m_timer = 0.0f;
				m_target_time = 3.2f;
			}
			else {
				m_current_walk = "Base_Walk_On_Bubble";
				this->ChangeTargetTime(3.0f, 6.0f);
			}
		}
		else {
			if (m_current_idle == "Base_Idle_On_Bubble"){
				m_current_idle = "Base_Idle_On_Bubble2";
				m_timer = 0.0f;
				m_target_time = 3.2f;
			}
			else {
				m_current_idle = "Base_Idle_On_Bubble";
				this->ChangeTargetTime(3.0f, 6.0f);
			}
		}
	}

	Ogre::SceneNode* bubble_node = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);

	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
		bool jump = true;
		Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
		s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
		bool limit = true;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
	}
	if (bubble_node && m_bubble){
		s_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
		dir.normalise();
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		DirDT dirdt(dir, dt);
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_APPLY_ROTATION, &dirdt);
	}
}

void PlayerOnBubble::BubbleRemoved(IEvent* evt){
	if (evt->m_type == EVT_BUBBLE_REMOVE){
		GameObject* bubble = static_cast<BubbleEvent*>(evt)->bubble;
		if (bubble == m_bubble){
			s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
			int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
			s_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
			Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
			s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
			s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
			bool limit = true;
			s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
			s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
		}
	}
}

void PlayerOnBubble::ChangeTargetTime(float low, float high){
	m_target_time = static_cast<float>(Ogre::Math::RangeRandom(Ogre::Real(low), Ogre::Real(high)));
	m_timer = 0.0f;
}

PlayerInsideBubble::PlayerInsideBubble(PhysicsEngine* physics_engine, MessageSystem* message_system) : m_on_bubble_y_offset(0.0f), m_physics_engine(physics_engine), m_message_system(message_system) { 
	m_type = PLAYER_STATE_INSIDE_BUBBLE; 
	m_on_bubble_y_offset = 0.5f;
	m_bubble_gravity = VariableManager::GetSingletonPtr()->GetAsFloat("BlueBubbleGravity");
	m_message_system->Register(EVT_BUBBLE_REMOVE, this, &PlayerInsideBubble::BubbleRemoved);
}

PlayerInsideBubble::~PlayerInsideBubble(void){
	m_message_system->Unregister(EVT_BUBBLE_REMOVE, this);
}

void PlayerInsideBubble::Enter(){
	m_bubble = s_input_component->GetBubble();

	Ogre::SceneNode* bubble_node = NULL;
	btRigidBody* player_body = NULL;
	btRigidBody* bubble_body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	if (player_body && bubble_body && bubble_node){
		float y_scale = bubble_node->getScale().y;
		y_scale *= 0.5f;
		float step_height = y_scale + 0.15f;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_STEP_HEIGHT, &step_height);
		y_scale = y_scale - m_on_bubble_y_offset;
		Generic6DofDef def;
		def.body_a = bubble_body;
		def.body_b = player_body;
		def.pivot_b.setY(m_on_bubble_y_offset);
		s_input_component->GetOwner()->CreateComponent(COMPONENT_GENERIC_6DOF_COMPONENT, Ogre::Vector3(0,0,0), &def);
		bool limit = false;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
	}
	/*if (m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
		bool start = true;
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
	}*/
}

void PlayerInsideBubble::Exit(){
	btRigidBody* bubble_body = NULL;	// configuration to avoid that the player falls through the world when exiting a bubble
	btRigidBody* player_body = NULL;
	Ogre::SceneNode* bubble_node = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	float step_height = (bubble_node->getScale().y * 0.5f) + 0.25f;
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	btVector3 from(player_body->getWorldTransform().getOrigin());
	btVector3 to(btVector3(from.x(), from.y() - step_height, from.z()));
	IgnoreBodyAndBubbleCast ray(player_body, bubble_body);	// we will ignore the player body and the bubble when ray testing
	m_physics_engine->GetDynamicWorld()->rayTest(from, to, ray);
	if (ray.hasHit() && !s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){  // the ray has hit something on the ground, push the bubble in the direction the player is facing
		Ogre::SceneNode* child_node = NULL;
		Ogre::SceneNode* player_node = NULL;
		s_messenger->Notify(MSG_NODE_GET_NODE, &player_node);
		s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &child_node);
		Ogre::Vector3 child_pos(child_node->_getDerivedPosition());
		Ogre::Vector3 player_pos(player_node->_getDerivedPosition());
		Ogre::Vector3 dir = child_pos - player_pos;
		dir.normalise();
		dir.y = 1.0f;	// always push the bubble upwards (will not move pink but blue)
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
		dir *= 4.0f;
		bubble_body->applyCentralImpulse(BtOgre::Convert::toBullet(dir));
	}
	// if the bubble is not hitting the ground simple drop the player down
	int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
	s_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
	Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
	s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
	s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
	bool limit = true;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
	step_height = 0.15f;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_STEP_HEIGHT, &step_height);
	m_bubble = NULL;
}

void PlayerInsideBubble::Update(float dt){
	Ogre::Vector3 dir = s_input_component->GetDirection();
	Ogre::SceneNode* bubble_node = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	/*if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A) && m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
		int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		s_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
		bool jump = true;
		Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
		s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
		bool limit = true;
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, &limit);
	}*/

	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE)){
		if (s_input_component->CanBlowPink()){
			this->ChangeBubbleType();
		}
	}
	if (bubble_node && m_bubble && s_manager->GetCurrentType() != PLAYER_STATE_LEAF_COLLECT){
		s_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
		dir.normalise();
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		DirDT dirdt(dir, dt);
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_APPLY_ROTATION, &dirdt);
	}
	Ogre::Vector3 zerodir = Ogre::Vector3::ZERO;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &zerodir);
}

void PlayerInsideBubble::ChangeBubbleType(){
	Ogre::String mat_name = Ogre::StringUtil::BLANK;
	btRigidBody* body = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	if (body){
		if (m_bubble->GetType() == GAME_OBJECT_BLUE_BUBBLE){
			btVector3 vel = body->getLinearVelocity();
			Ogre::Vector3 gravity(0,0,0);
			m_bubble->SetType(GAME_OBJECT_PINK_BUBBLE);
			mat_name = "PinkBubble";
			m_bubble->GetComponentMessenger()->Notify(MSG_MESH_SET_MATERIAL_NAME, &mat_name);
			m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			body->setLinearVelocity(btVector3(vel.x(), 0.0f, vel.z()));
			body->setLinearFactor(btVector3(1,0,1));
			bool start = true;
			m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
			if (s_manager->GetCurrentType() == PLAYER_STATE_FALLING || s_manager->GetCurrentType() == PLAYER_STATE_JUMP){
				s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
			}
		}
		else if (m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
			Ogre::Vector3 gravity(0,-m_bubble_gravity,0);
			m_bubble->SetType(GAME_OBJECT_BLUE_BUBBLE);
			mat_name = "BlueBubble";
			m_bubble->GetComponentMessenger()->Notify(MSG_MESH_SET_MATERIAL_NAME, &mat_name);
			m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			body->setLinearFactor(btVector3(1,1,1));
			bool start = false;
			m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
		}
	}
}

void PlayerInsideBubble::BubbleRemoved(IEvent* evt){
	if (evt->m_type == EVT_BUBBLE_REMOVE){
		GameObject* bubble = static_cast<BubbleEvent*>(evt)->bubble;
		if (bubble == m_bubble){
			s_manager->GoInsideBubble(false);
		}
	}
}

void PlayerBounce::Enter(){
	s_animation->PlayAnimation("Base_Jump_Start", false, AnimationBlender::BlendSwitch);
}

void PlayerBounce::Exit(){

}

void PlayerBounce::Update(float dt){
	Ogre::Vector3 dir = s_input_component->GetDirection();
	s_animation->PlayAnimation("Base_Jump_Loop");
	btRigidBody* body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	float y_vel = body->getLinearVelocity().y();
	if (y_vel < 0.0f){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_LAND));
	}
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

PlayerHoldObject::PlayerHoldObject(PhysicsEngine* physics_engine, MessageSystem* message_system) : m_object(NULL), m_physics_engine(physics_engine), m_message_system(message_system) { 
	m_type = PLAYER_STATE_HOLD_OBJECT; 
	m_bubble_gravity = VariableManager::GetSingletonPtr()->GetAsFloat("BlueBubbleGravity");
	m_message_system->Register(EVT_QUEST_ITEM_REMOVE, this, &PlayerHoldObject::ItemRemoved);
}

PlayerHoldObject::~PlayerHoldObject(void){
	m_message_system->Unregister(EVT_QUEST_ITEM_REMOVE, this);
}

void PlayerHoldObject::Enter(){
	btRigidBody* player_body = NULL;
	Ogre::SceneNode* player_node = NULL;
	Ogre::SceneNode* child_node = NULL;
	s_messenger->Notify(MSG_NODE_GET_NODE, &player_node);
	s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &child_node);
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
	Ogre::Vector3 from = player_node->getPosition();
	Ogre::Vector3 to = child_node->_getDerivedPosition();
	from.y += 0.5f;		// player offset in Y from pivot
	IgnoreBodyCast ray(player_body);

	m_physics_engine->GetDynamicWorld()->rayTest(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), ray);
	if (m_physics_engine->GetDebugDraw()){
		m_physics_engine->GetDebugDraw()->drawLine(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), btVector3(0,0,0));
	}
	if (ray.hasHit()){
		CollisionDef* coll_def = static_cast<CollisionDef*>(ray.m_collisionObject->getUserPointer());
		if (coll_def->flag == COLLISION_FLAG_GAME_OBJECT){
			GameObject* ob = static_cast<GameObject*>(coll_def->data);
			if (ob->GetType() == GAME_OBJECT_QUEST_ITEM){
				m_object = ob;
				btRigidBody* ob_body = NULL;
				btRigidBody* player_trigger = NULL;
				s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_trigger, "btrig");
				if (ob->GetType() == GAME_OBJECT_BLUE_BUBBLE){
					m_object->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &ob_body, "body");
				}
				else {
					m_object->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &ob_body);
				}
				
				if (player_trigger && ob_body){
					if (ob->GetType() == GAME_OBJECT_BLUE_BUBBLE){
						ob->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_READY, NULL);
						ob->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);
					}
					ob_body->setGravity(btVector3(0,0,0));
					Generic6DofDef def;
					def.body_a = player_trigger;
					def.body_b = ob_body;
					s_input_component->GetOwner()->CreateComponent(COMPONENT_GENERIC_6DOF_COMPONENT, Ogre::Vector3(0,0,0), &def);
				}
			}
			else if (ob->GetType() == GAME_OBJECT_BLUE_BUBBLE || ob->GetType() == GAME_OBJECT_PINK_BUBBLE){
				Ogre::Vector3 gravity(0,0,0);
				s_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &ob);
				if (ob->GetType() == GAME_OBJECT_BLUE_BUBBLE){
					ob->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);
				}
				s_messenger->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);
				s_manager->HoldObject(false);
				s_manager->GoInsideBubble(true);
			}
		}
	}
}

void PlayerHoldObject::Exit(){
	s_input_component->GetOwner()->RemoveComponent(COMPONENT_GENERIC_6DOF_COMPONENT);
	if (m_object){
		Ogre::Vector3 gravity(Ogre::Real(0),Ogre::Real(-9.8),Ogre::Real(0));
		m_object->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
	}
	m_object = NULL;
}

void PlayerHoldObject::Update(float dt){
	if (m_object){
		s_animation->PlayAnimation("Top_Blow_Loop");
		btRigidBody* body = NULL;
		Ogre::SceneNode* node = NULL;
		s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
		if (node){
			Ogre::Vector3 pos = node->_getDerivedPosition();
			s_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");
		}
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X)){
			s_manager->HoldObject(false);
		}
	}
	else {
		s_manager->HoldObject(false);
	}
}

void PlayerHoldObject::ItemRemoved(IEvent* evt){
	if (evt->m_type == EVT_QUEST_ITEM_REMOVE){
		s_manager->HoldObject(false);
	}
}

PlayerLeafCollect::PlayerLeafCollect(MessageSystem* message_system) : m_message_system(message_system), m_leaf_object(NULL){
	m_type = PLAYER_STATE_LEAF_COLLECT;
	m_message_system->Register(EVT_LEAF_PICKUP,this, &PlayerLeafCollect::GetLeaf);
	s_messenger->Notify(MSG_NODE_GET_NODE, &m_player_node);
}

PlayerLeafCollect::~PlayerLeafCollect(void){
	m_message_system->Unregister(EVT_LEAF_PICKUP, this);
}

void PlayerLeafCollect::Enter(){
	m_is_dancing = false;
	bool on_ground = s_input_component->IsOnGround();
	if (on_ground){
		m_is_dancing = true;
		s_animation->PlayAnimation("Base_PickUpLeaf_State");
		m_leaf_object->GetComponentMessenger()->Notify(MSG_BOBBING_START_MOVING, &m_player_node);
	}
}

void PlayerLeafCollect::Exit(){
	m_leaf_node = NULL;
	m_leaf_object = NULL;
}

void PlayerLeafCollect::Update(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	if (!m_is_dancing){
		bool on_ground = s_input_component->IsOnGround();
		if (on_ground){
			m_is_dancing = true;
			s_animation->PlayAnimation("Base_PickUpLeaf_State");
			m_leaf_object->GetComponentMessenger()->Notify(MSG_BOBBING_START_MOVING, &m_player_node);
		}
	}
	else {
		float distance = m_leaf_node->getPosition().distance(m_player_node->getPosition());
		if (distance <= 0.06f){
			m_leaf_object->RemoveGameObject(m_leaf_object);
			s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
		}
	}
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

void PlayerLeafCollect::GetLeaf(IEvent* evt){
	if (evt->m_type == EVT_LEAF_PICKUP){
		m_leaf_object = static_cast<LeafEvent*>(evt)->leaf;
		m_leaf_node = static_cast<LeafEvent*>(evt)->leaf_node;
	}
}
