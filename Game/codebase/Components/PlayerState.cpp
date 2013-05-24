#include "stdafx.h"
#include "PlayerState.h"
#include "..\Managers\AnimationManager.h"
#include "ComponentMessenger.h"
#include "PlayerInputComponent.h"
#include "..\Managers\PlayerStateManager.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "..\Managers\VariableManager.h"
#include "..\PhysicsPrereq.h"
#include "..\BtOgreGP.h"

ComponentMessenger* PlayerState::s_messenger = NULL;
AnimationManager* PlayerState::s_animation = NULL;
PlayerInputComponent* PlayerState::s_input_component = NULL;
PlayerStateManager* PlayerState::s_manager = NULL;
SoundManager* PlayerState::s_sound_manager = NULL;

PlayerIdle::PlayerIdle(void){ 
	m_type = PLAYER_STATE_IDLE; 
}

void PlayerIdle::Enter(){
	s_animation->PlayAnimation("Base_Idle");
	//s_animation->PlayAnimation("Top_Idle");
	std::cout << "Enter Idle State\n";
}

void PlayerIdle::Exit(){

}

void PlayerIdle::PlayTopIdle(){
	//s_animation->PlayAnimation("Top_Idle");
}

void PlayerIdle::Update(float dt){
	if (!s_manager->IsBlowingBubbles()){
		s_animation->PlayAnimation("Top_Idle");
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_LEFT_MOUSE) || s_input_component->GetInputManager()->IsButtonPressed(BTN_RIGHT_MOUSE)){
			s_manager->BlowBubble(true);
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
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}
PlayerStateMove::PlayerStateMove(void){ 
	m_type = PLAYER_STATE_MOVE;
	m_walk_sound = s_sound_manager->Create2DData("Yomi_Walk", false, false, false, false, 1.0f, 1.0f);
}

void PlayerStateMove::Enter(){
	s_animation->PlayAnimation("Base_Walk");
	s_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
	std::cout << "Enter Move State\n";
}

void PlayerStateMove::Exit(){
	s_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
	//s_animation->PlayAnimation(m_current_base);
	//s_animation->PlayAnimation(m_current_top);
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
			s_manager->BlowBubble(true);
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
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

PlayerBlowBubble::PlayerBlowBubble(void) : m_bubble(NULL), m_current_scale(0.0f) { 
	m_type = PLAYER_STATE_BLOW_BUBBLE; 
	m_bubble_blow_sound = s_sound_manager->Create2DData("Blow_Bubble", false, false, false, false, 1.0f, 1.0f);
	m_min_bubble_size = 0.805f;
	m_max_bubble_size = 1.907f;
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
	}
	else if (s_input_component->GetInputManager()->IsButtonDown(BTN_LEFT_MOUSE)){
		m_bubble = s_input_component->GetOwner()->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_BLUE_BUBBLE, pos, &bubble_def);
		m_bubble_type = BUBBLE_TYPE_BLUE;
	}
	s_messenger->Notify(MSG_SFX2D_PLAY, &m_bubble_blow_sound);
	std::cout << "Enter BlowBubble State\n";
	s_animation->PlayAnimation("Blow_Start", false);
	s_animation->QueueAnimation("Blow_Loop");
}

void PlayerBlowBubble::Exit(){
	s_animation->PlayAnimation("Blow_End", false);
	int index = 1;
	s_messenger->Notify(MSG_ANIMATION_SET_WAIT, &index);
	s_messenger->Notify(MSG_SFX2D_STOP, &m_bubble_blow_sound);
	s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
}

void PlayerBlowBubble::Update(float dt){
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
			this->CreateTriggerForBubble();
			Ogre::Vector3 gravity(0,-5.8f,0);
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
			if (m_bubble != NULL)
			{
				if (child_node && bubble_node && player_node){
					Ogre::Vector3 pos = child_node->_getDerivedPosition();
					Ogre::Vector3 dir = pos - player_node->getPosition();
					dir.normalise();
					float scale_size = (bubble_node->getScale().length() * 0.5f);
					pos += (dir*scale_size);
					s_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");		// btrig is the ID for the TriggerCompoent
					m_bubble->GetComponentMessenger()->Notify(MSG_INCREASE_SCALE_BY_VALUE, &scale_inc);
				}
			}
		}
}

void PlayerBlowBubble::CreateTriggerForBubble(){
	Ogre::SceneNode* node = NULL;
	s_messenger->Notify(MSG_NODE_GET_NODE, &node);
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
		m_bubble->CreateComponent(COMPONENT_SYNCED_TRIGGER, pos, &trdef);
	}
}

PlayerJump::PlayerJump(void){ 
	m_type = PLAYER_STATE_JUMP;
}

void PlayerJump::Enter(){
	s_animation->PlayAnimation("Jump_Start", false);
	s_animation->QueueAnimation("Jump_Loop");
	bool jump = true;
	s_messenger->Notify(MSG_CHARACTER_CONTROLLER_JUMP, &jump);
	std::cout << "Enter Jump State\n";
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
	s_animation->PlayAnimation("Jump_Loop");
	std::cout << "Enter Fall State\n";
	s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
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
	s_messenger->Notify(MSG_ANIMATION_CLEAR_QUEUE, NULL);
	s_animation->PlayAnimation("Jump_End", false);
	std::function<void()> func = [this] { Proceed(); };
	s_messenger->Notify(MSG_ANIMATION_CALLBACK, &func);
	std::cout << "Enter Land state\n";
}

void PlayerLand::Exit(){

}

void PlayerLand::Update(float dt){
	Ogre::Vector3 dir = s_input_component->GetDirection();
	if (dir != Ogre::Vector3::ZERO){
		s_messenger->Notify(MSG_ANIMATION_CLEAR_CALLBACK, NULL);
		int p = 1;
		s_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
		s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_MOVE));
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

void PlayerOnBubble::Enter(){
	m_transition = true;
	s_animation->PlayAnimation("Jump_End", false);
	s_animation->QueueAnimation("Base_Idle");
	s_animation->QueueAnimation("Top_Idle");
	m_bubble = s_input_component->GetBubble();
	std::cout << "Enter OnBubble State\n";
	m_current_base = "Base_Idle";
	m_current_top = "Top_Idle";
}

void PlayerOnBubble::Exit(){

}

void PlayerOnBubble::Update(float dt){
	if (!m_transition){
		Ogre::Vector3 dir = s_input_component->GetDirection();
		Ogre::String move_base = Ogre::StringUtil::BLANK;
		Ogre::String move_top = Ogre::StringUtil::BLANK;
		if (dir == Ogre::Vector3::ZERO){
			move_base = "Base_Idle";
			move_top = "Top_Idle";
		}
		else {
			move_base = "Base_Walk";
			move_top = "Top_Walk";
		}
			s_animation->PlayAnimation(move_base);
			s_animation->PlayAnimation(move_top);

		if (move_top != m_current_top){
			m_current_top = move_top;
			m_current_base = move_base;

		}

		Ogre::SceneNode* bubble_node = NULL;
		m_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
		btRigidBody* player_body = NULL;
		s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
		if (bubble_node){
			Ogre::Vector3 bubble_scale = bubble_node->getScale();
			btRigidBody* bubble_body = NULL;
			m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
			if (player_body && bubble_body){
				player_body->setLinearVelocity(btVector3(0,0,0));
				btVector3 pos = BtOgre::Convert::toBullet(bubble_node->getPosition());
				pos.setY(pos.y() + bubble_scale.y * 0.5f + 0.01f);
				player_body->getWorldTransform().setOrigin(pos);
			}
		}

		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
			bool jump = true;
			Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
			s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
			s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
			s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
		}
		if (bubble_node){
			s_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
			dir.normalise();
			m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		}
	}
	else {
		btRigidBody* body = NULL;
		btRigidBody* bubble_body = NULL;
		s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (body && bubble_body){
			Ogre::Vector3 bubble_pos = BtOgre::Convert::toOgre(bubble_body->getWorldTransform().getOrigin());
			Ogre::Vector3 player_pos = BtOgre::Convert::toOgre(body->getWorldTransform().getOrigin());
			float distance = bubble_pos.squaredDistance(player_pos);
			if (distance < 0.01f){
				int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				s_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
				m_transition = false;
			}
			else {
				float speed = 5.0f * dt;
				Ogre::Vector3 dir = bubble_pos - player_pos;
				dir.normalise();
				player_pos += (dir * speed);
				s_messenger->Notify(MSG_SET_OBJECT_POSITION, &player_pos);
				body->setLinearVelocity(btVector3(0,0,0));
			}
		}
	}
}

void PlayerInsideBubble::Enter(){
	m_transition = true;
	m_bubble = s_input_component->GetBubble();
	std::cout << "Enter InsideBubble State\n";
}

void PlayerInsideBubble::Exit(){

}

void PlayerInsideBubble::Update(float dt){
	if (!m_transition){
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
			if (player_body && bubble_body){
				player_body->setLinearVelocity(btVector3(0,0,0));
				btScalar y_pos = bubble_body->getWorldTransform().getOrigin().y() - player_body->getWorldTransform().getOrigin().y();
				btVector3 pos = BtOgre::Convert::toBullet(bubble_node->getPosition());
				player_body->getWorldTransform().setOrigin(pos);
			}
		}
		if (s_input_component->GetInputManager()->IsButtonPressed(BTN_A)){
			int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
			s_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
			bool jump = true;
			Ogre::Vector3 gravity(0.0f, -9.8f, 0.0f);
			s_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
			s_messenger->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity, "body");
			s_manager->SetPlayerState(s_manager->GetPlayerState(PLAYER_STATE_JUMP));
		}
		if (bubble_node){
			s_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &dir);
			dir.normalise();
			m_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
		}
	}
	else {
		btRigidBody* body = NULL;
		btRigidBody* bubble_body = NULL;
		s_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
		m_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &bubble_body);
		if (body && bubble_body){
			Ogre::Vector3 bubble_pos = BtOgre::Convert::toOgre(bubble_body->getWorldTransform().getOrigin());
			Ogre::Vector3 player_pos = BtOgre::Convert::toOgre(body->getWorldTransform().getOrigin());
			float distance = bubble_pos.squaredDistance(player_pos);
			if (distance < 0.005f){
				m_transition = false;
			}
			else {
				float speed = 5.0f * dt;
				Ogre::Vector3 dir = bubble_pos - player_pos;
				dir.normalise();
				player_pos += dir * speed;
				s_messenger->Notify(MSG_SET_OBJECT_POSITION, &player_pos);
				body->setLinearVelocity(btVector3(0,0,0));
			}
		}
	}
}

void PlayerBounce::Enter(){
	s_animation->PlayAnimation("Jump_Start", false);
	s_animation->QueueAnimation("Jump_Loop");
	std::cout << "Enter Bounce State\n";
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