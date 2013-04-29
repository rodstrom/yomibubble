#include "stdafx.h"
#include "PlayerInputComponent.h"
#include "..\Managers\InputManager.h"
#include "ComponentMessenger.h"
#include "..\BtOgreGP.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "..\PhysicsEngine.h"

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
	case MSG_PLAYER_INPUT_STATE_GET:
		*static_cast<int*>(msg) = m_player_state;
		break;
	default:
		break;
	}
}

void PlayerInputComponent::Shut(){
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Unregister(MSG_PLAYER_INPUT_SET_STATE, this);
	m_messenger->Unregister(MSG_PLAYER_INPUT_STATE_GET, this);
}

void PlayerInputComponent::Init(InputManager* input_manager, SoundManager* sound_manager){
	m_input_manager = input_manager;

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
	m_messenger->Register(MSG_PLAYER_INPUT_STATE_GET, this);
	m_messenger->Register(MSG_PLAYER_INPUT_SET_BUBBLE, this);
	m_messenger->Register(MSG_PLAYER_INPUT_SET_STATE, this);
}

void PlayerInputComponent::Normal(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	if (dir != Ogre::Vector3::ZERO){
		/*m_messenger->Notify(MSG_SFX2D_PLAY, &m_walk_sound);
		AnimationMsg msg;
		msg.id="RunBase"; //SliceHorizontal är en top anim
		msg.index=0;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
		//m_messenger->Notify(MSG_ANIMATION_BLEND, &msg);
		AnimationMsg msg2;
		msg.id="SliceHorizontal"; //SliceHorizontal är en top anim
		msg.index=1;
		m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);*/

	}
	else
	{
		//m_messenger->Notify(MSG_SFX2D_STOP, &m_walk_sound);
	}

	if (!m_is_creating_bubble){
		Ogre::SceneNode* node = NULL;
		if (m_input_manager->IsButtonPressed(BTN_LEFT_MOUSE) || m_input_manager->IsButtonPressed(BTN_RIGHT_MOUSE)){
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &node);
			btRigidBody* body = NULL;
			m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "btrig");
			if (node && body){
				Ogre::Vector3 pos = node->_getDerivedPosition();
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
		const float SCALE = 0.001f * dt;
		Ogre::Vector3 scale_inc(SCALE);
		m_current_scale += SCALE;
		if (m_bubble_type == BUBBLE_TYPE_BLUE && m_input_manager->IsButtonReleased(BTN_LEFT_MOUSE)){
			Ogre::Vector3 gravity(0,-9.8f,0);
			m_current_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
			m_current_bubble->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GRAVITY_SET, &gravity);
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble = NULL;
		}
		else if (m_bubble_type == BUBBLE_TYPE_PINK && m_input_manager->IsButtonReleased(BTN_RIGHT_MOUSE)){
			m_current_scale = 0.0f;
			m_is_creating_bubble = false;
			m_current_bubble->RemoveComponent(COMPONENT_POINT2POINT_CONSTRAINT);
			m_current_bubble = NULL;
		}

		if (m_input_manager->IsButtonDown(BTN_LEFT_MOUSE) || m_input_manager->IsButtonDown(BTN_RIGHT_MOUSE)){
			Ogre::SceneNode* player_node = NULL;
			Ogre::SceneNode* child_node = NULL;
			Ogre::SceneNode* bubble_node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &player_node);
			m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &child_node);
			m_current_bubble->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &bubble_node);
			if (child_node && bubble_node && player_node){
				Ogre::Vector3 pos = child_node->_getDerivedPosition();
				Ogre::Vector3 dir = pos - player_node->getPosition();
				dir.normalise();
				float scale_size = (bubble_node->getScale().length() * 50.0f);
				pos += (dir*scale_size);
				m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &pos, "btrig");		// btrig is the ID for the TriggerCompoent
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


	Ogre::Vector3 acc = Ogre::Vector3::ZERO;
	Acceleration(dir, acc, dt);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &acc);
}

void PlayerInputComponent::OnBubble(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
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
			btVector3 pos = BtOgre::Convert::toBullet(bubble_node->getPosition());
			pos.setY(pos.y() + 2.0f);
			player_body->getWorldTransform().setOrigin(pos);
		}
	}
	if (m_input_manager->IsButtonPressed(BTN_START)){
		m_player_state = PLAYER_STATE_NORMAL;
		bool jump = true;
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, &jump);
		return;
	}

	float speed = 10.0f * dt;
	bool follow_cam = false;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, &follow_cam);
	if (follow_cam){
		Ogre::SceneNode* cam_node = NULL;
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &cam_node);
		if (cam_node && bubble_node){
			Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
			goal_dir += dir.z * cam_node->getOrientation().zAxis();
			goal_dir += dir.x * cam_node->getOrientation().xAxis();
			goal_dir.y = 0.0f;
			goal_dir.normalise();
			m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &goal_dir);
		}
	}
	else{
		m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
	}
}

void PlayerInputComponent::InsideBubble(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
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
	if (m_input_manager->IsButtonPressed(BTN_START)){
		m_player_state = PLAYER_STATE_NORMAL;
		int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		bool jump = true;
		m_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, &coll, "body");
		m_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &jump);
		m_messenger->Notify(MSG_CHARACTER_CONROLLER_JUMP, &jump);
		return;
	}

	float speed = 10.0f * dt;
	bool follow_cam = false;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, &follow_cam);
	if (follow_cam){
		Ogre::SceneNode* cam_node = NULL;
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &cam_node);
		if (cam_node && bubble_node){
			Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
			goal_dir += dir.z * cam_node->getOrientation().zAxis();
			goal_dir += dir.x * cam_node->getOrientation().xAxis();
			goal_dir.y = 0.0f;
			goal_dir.normalise();
			std::cout << "X: " << goal_dir.x << " Y: " << goal_dir.y << " Z: " << goal_dir.z << std::endl;
			m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &goal_dir);
		}
	}
	else{
		m_current_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE, &dir);
	}
}

void PlayerInputComponent::Bouncing(float dt){
	Ogre::Vector3 dir = Ogre::Vector3::ZERO;
	dir.x = m_input_manager->GetMovementAxis().x;
	dir.z = m_input_manager->GetMovementAxis().z;

	Ogre::Vector3 acc = Ogre::Vector3::ZERO;
	Acceleration(dir, acc, dt);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &acc);
}

void PlayerInputComponent::Acceleration(Ogre::Vector3& dir, Ogre::Vector3& acc, float dt){
	bool is_jumping = false;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET, &is_jumping);
	
	float deacc = m_deacc;
	if (is_jumping){
		deacc = m_deacc;
	}

	if (dir.x != 0.0f){
		if (dir.x < 0.0f){
			m_acc_x = std::max(m_acc_x - (m_velocity*dt), -m_max_velocity);
		}
		else if (dir.x > 0.0f){
			m_acc_x = std::min(m_acc_x + (m_velocity*dt), m_max_velocity);
		}
	}
	else{
		if (m_acc_x < 0.0f){
			m_acc_x = std::min(m_acc_x + (m_deacc*dt), 0.0f);
		}
		else if (m_acc_x > 0.0f){
			m_acc_x = std::max(m_acc_x - (m_deacc*dt), 0.0f);
		}
	}

	if (dir.z != 0.0f){
		if (dir.z < 0.0f){
			m_acc_z = std::max(m_acc_z - (m_velocity*dt), -m_max_velocity);
		}
		else if (dir.z > 0.0f){
			m_acc_z = std::min(m_acc_z + (m_velocity*dt), m_max_velocity);
		}
	}
	else {
		if (m_acc_z < 0.0f){
			m_acc_z = std::min(m_acc_z + (m_deacc*dt), 0.0f);
		}
		else if (m_acc_z > 0.0f){
			m_acc_z = std::max(m_acc_z - (m_deacc*dt), 0.0f);
		}
	}
	acc.x += m_acc_x;
	acc.z += m_acc_z;
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


void BubbleController::ApplyImpulse(const btVector3& dir){
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
	if (body){
		body->applyCentralImpulse(btVector3(dir.x(), 0.0f, dir.z()));
	}
}

void BubbleController::Update(float dt){
	if (m_apply_impulse){
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
	if (body){
	}
		m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &m_impulse);
	}
}

void BubbleController::SimulationStep(btScalar time_step){
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
	if (body){
		const btScalar max_speed = (btScalar)m_max_velocity;
		btVector3 vel = body->getLinearVelocity();
		btScalar speed = vel.length();
		if (speed >= max_speed){
			vel *= max_speed/speed;
			body->setLinearVelocity(vel);
		}
	}
}