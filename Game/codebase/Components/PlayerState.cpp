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

ComponentMessenger* PlayerState::s_messenger = NULL;
AnimationManager* PlayerState::s_animation = NULL;
PlayerInputComponent* PlayerState::s_input_component = NULL;
PlayerStateManager* PlayerState::s_manager = NULL;
SoundManager* PlayerState::s_sound_manager = NULL;

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
	if (!s_manager->IsBlowingBubbles()){
		s_animation->PlayAnimation(m_current_idle_top);
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE)){
			if (!s_manager->IsHoldingObject()){
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
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (!on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X)){
		s_manager->HoldObject(true);
	}
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}
PlayerStateMove::PlayerStateMove(void){ 
	m_type = PLAYER_STATE_MOVE;
	m_walk_sound = s_sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
}

void PlayerStateMove::Enter(){
	s_animation->PlayAnimation("Base_Walk");
	s_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
	s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("Stick"));
}

void PlayerStateMove::Exit(){
	s_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
}
void PlayerStateMove::PlayTopAnimation(){

}

void PlayerStateMove::Update(float dt){
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
	if (!s_manager->IsBlowingBubbles()){
		s_animation->PlayAnimation(move_top);
	}
	

	if (!s_manager->IsBlowingBubbles()){
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE)){
			if (!s_manager->IsHoldingObject()){
				s_manager->BlowBubble(true);
			}
		}
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (!on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	if (dir == Ogre::Vector3::ZERO){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));	// if the character is not moving, change to idle
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_X)){
		s_manager->HoldObject(true);
	}
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

PlayerBlowBubble::PlayerBlowBubble(void) : m_bubble(NULL), m_current_scale(0.0f) {
	m_type = PLAYER_STATE_BLOW_BUBBLE;
	m_bubble_blow_sound = s_sound_manager->Create2DData("Blow_Bubble", false, false, false, false, 1.0f, 1.0f);
	m_min_bubble_size = VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Min_Size");
	m_max_bubble_size = VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Max_Size");
	m_bubble_gravity = VariableManager::GetSingletonPtr()->GetAsFloat("BlueBubbleGravity");
}

void PlayerBlowBubble::Enter(){
	btRigidBody* body = NULL;
	Ogre::SceneNode* node = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "btrig");
	s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
	Ogre::Vector3 pos = node->_getDerivedPosition();
	BubbleDef bubble_def;
	bubble_def.connection_body = body;
	bubble_def.start_scale = 0.002f;
	bubble_def.friction = 1.0f;
	bubble_def.restitution = 0.2f;
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
	s_animation->PlayAnimation("Top_Blow_Start", false);
}

void PlayerBlowBubble::Exit(){
	s_animation->PlayAnimation("Top_Blow_End", false);
	//int index = 1;
	//s_messenger->Notify(MSG_ANIMATION_SET_WAIT, &index);
	s_messenger->Notify(MSG_SFX2D_STOP, &m_bubble_blow_sound);
	s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
}

void PlayerBlowBubble::Update(float dt){
	s_animation->PlayAnimation("Top_Blow_Loop", true, AnimationBlender::BlendThenAnimate);

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
				m_bubble = NULL;
				//insert pop sound here
				SoundData2D pop_sound = s_sound_manager->Create2DData("Bubble_Burst", false, false, false, false, 1.0, 1.0);
				s_messenger->Notify(MSG_SFX2D_PLAY, &pop_sound);
				m_current_scale = 0.0f;
				s_manager->BlowBubble(false);
			}
		}
		if (m_bubble_type == BUBBLE_TYPE_BLUE && s_input_component->GetInputManager()->IsButtonReleased(BTN_LEFT_MOUSE) && m_bubble != NULL){
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
			s_manager->BlowBubble(false);
			m_bubble = NULL;
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && s_input_component->GetInputManager()->IsButtonReleased(BTN_RIGHT_MOUSE) && m_bubble != NULL){
			if (m_current_scale < m_min_bubble_size){
				m_current_scale = m_min_bubble_size;
				Ogre::Vector3 scale_incr(m_current_scale);
				m_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_incr);
			}
			btRigidBody* bubble_body = NULL;
			m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body, "body");
			bubble_body->setLinearVelocity(btVector3(0,0,0));
			bubble_body->setLinearFactor(btVector3(1,0,1));
			this->CreateTriggerForBubble();
			m_current_scale = 0.0f;
			s_manager->BlowBubble(false);
			m_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
			m_bubble = NULL;
		}

		if (s_input_component->GetInputManager()->IsButtonDown(BTN_LEFT_MOUSE) && m_bubble != NULL || s_input_component->GetInputManager()->IsButtonDown(BTN_RIGHT_MOUSE) && m_bubble != NULL){
			Ogre::SceneNode* player_node = NULL;
			Ogre::SceneNode* child_node = NULL;
			Ogre::SceneNode* bubble_node = NULL;
			s_messenger->Notify(MSG_NODE_GET_NODE, &player_node);
			s_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &child_node);
			m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
			if (m_bubble != NULL) {
				if (child_node && bubble_node && player_node){
					//Ogre::Vector2 child_pos(child_node->_getDerivedPosition().x, child_node->_getDerivedPosition().z);
					//Ogre::Vector2 player_pos(player_node->_getDerivedPosition().x, player_node->_getDerivedPosition().z);
					//Ogre::Vector2 dir = child_pos - player_pos;
					Ogre::Vector3 child_pos(child_node->_getDerivedPosition());
					Ogre::Vector3 player_pos(player_node->_getDerivedPosition());
					Ogre::Vector3 dir = child_pos - player_pos;
					dir.normalise();
					float scale_size = (bubble_node->getScale().length() * 0.3f);
					child_pos += (dir*scale_size);
					float y_pos = child_node->_getDerivedPosition().y;
					//Ogre::Vector3 new_dir(child_pos.x, y_pos, child_pos.y);
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
}

void PlayerJump::Enter(){
	s_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("Jump"));
	int p = 1;
	s_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
	s_animation->PlayAnimation("Base_Jump_Start", false);
	s_animation->PlayAnimation("Base_Jump_Loop", true);
	bool jump = true;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
}

void PlayerJump::Exit(){

}

void PlayerJump::Update(float dt){
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
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_LAND));
	}
	Ogre::Vector3 dir = s_input_component->GetDirection();
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

void PlayerLand::Enter(){
	//s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
	s_animation->PlayAnimation("Base_Jump_End", false);
	//std::function<void()> func = [this] { Proceed(); };
	//s_messenger->Notify(MSG_ANIMATION_CALLBACK, &func);
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
	Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir == Ogre::Vector3::ZERO){
		int p = 1;
		s_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_IDLE));
	}
}

PlayerOnBubble::PlayerOnBubble(MessageSystem* message_system) : m_on_bubble_y_offset(0.0f), m_message_system(message_system),
	m_timer(0.0f), m_target_time(0.0f) { 
	m_on_bubble_y_offset = VariableManager::GetSingletonPtr()->GetAsFloat("OnBubbleY");
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
			s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
		}
	}
}

void PlayerOnBubble::ChangeTargetTime(float low, float high){
	m_target_time = static_cast<float>(Ogre::Math::RangeRandom(Ogre::Real(low), Ogre::Real(high)));
	m_timer = 0.0f;
}

PlayerInsideBubble::PlayerInsideBubble(MessageSystem* message_system) : m_on_bubble_y_offset(0.0f), m_message_system(message_system) { 
	m_type = PLAYER_STATE_INSIDE_BUBBLE; 
	m_on_bubble_y_offset = VariableManager::GetSingletonPtr()->GetAsFloat("InsideBubbleY");
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
		y_scale = y_scale - m_on_bubble_y_offset;
		Generic6DofDef def;
		def.body_a = bubble_body;
		def.body_b = player_body;
		def.pivot_b.setY(m_on_bubble_y_offset);
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
}

void PlayerInsideBubble::Exit(){
	if (m_bubble->GetType() == GAME_OBJECT_PINK_BUBBLE){
		bool start = false;
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
	}
	m_bubble = NULL;
}

void PlayerInsideBubble::Update(float dt){
	Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir == Ogre::Vector3::ZERO){
		s_animation->PlayAnimation("Base_Idle");
		s_animation->PlayAnimation("Top_Idle");
	}
	else {
		s_animation->PlayAnimation("Base_Walk");
		s_animation->PlayAnimation("Top_Walk");
	}
	Ogre::SceneNode* bubble_node = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
	if (bubble_node){
		btRigidBody* player_body = NULL;
		btRigidBody* bubble_body = NULL;
		s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
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
	}
	if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE)){
		this->ChangeBubbleType();
	}
	if (bubble_node && m_bubble){
		s_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
		dir.normalise();
		m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		DirDT dirdt(dir, dt);
		s_messenger->Notify(MSG_CHARACTER_CONTROLLER_APPLY_ROTATION, &dirdt);
	}
}

void PlayerInsideBubble::ChangeBubbleType(){
	Ogre::String mat_name = Ogre::StringUtil::BLANK;
	btRigidBody* body = NULL;
	m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	if (body){
		if (m_bubble->GetType() == GAME_OBJECT_BLUE_BUBBLE){
			Ogre::Vector3 gravity(0,0,0);
			m_bubble->SetType(GAME_OBJECT_PINK_BUBBLE);
			mat_name = "PinkBubble";
			m_bubble->GetComponentMessenger()->Notify(MSG_MESH_SET_MATERIAL_NAME, &mat_name);
			m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			body->clearForces();
			body->setLinearFactor(btVector3(1,0,1));
			bool start = true;
			m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_TIMER_RUN, &start);
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

void PlayerBounce::Enter(){
	s_animation->PlayAnimation("Base_Jump_Start", false);
	s_animation->PlayAnimation("Base_Jump_Loop");
}

void PlayerBounce::Exit(){

}

void PlayerBounce::Update(float dt){
	btRigidBody* body = NULL;
	s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	float y_vel = body->getWorldTransform().getOrigin().y();
	if (y_vel > 0.0f){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_FALLING));
	}
	bool on_ground = s_input_component->IsOnGround();
	if (on_ground){
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_LAND));
	}
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
	from.y += 0.5f;
	IgnoreBodyCast ray(player_body);

	m_physics_engine->GetDynamicWorld()->rayTest(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), ray);
	if (m_physics_engine->GetDebugDraw()){
		m_physics_engine->GetDebugDraw()->drawLine(BtOgre::Convert::toBullet(from), BtOgre::Convert::toBullet(to), btVector3(0,0,0));
	}
	if (ray.hasHit()){
		CollisionDef* coll_def = static_cast<CollisionDef*>(ray.m_collisionObject->getUserPointer());
		if (coll_def->flag == COLLISION_FLAG_GAME_OBJECT){
			GameObject* ob = static_cast<GameObject*>(coll_def->data);
			if (ob->GetType() == GAME_OBJECT_BLUE_BUBBLE){
				//std::cout << "collision with blue bubble\n";
				m_object = ob;
			}
		}
	}
}

void PlayerHoldObject::Exit(){
	m_object = NULL;
}

void PlayerHoldObject::Update(float dt){
	if (m_object){
		s_manager->HoldObject(false);
	}
	else {
		s_manager->HoldObject(false);
	}
}
