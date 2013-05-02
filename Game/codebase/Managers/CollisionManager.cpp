#include "stdafx.h"
#include "CollisionManager.h"
#include "..\Components\GameObjectPrereq.h"
#include "..\Components\GameObject.h"
#include "GameObjectManager.h"
#include "..\PhysicsEngine.h"
#include "..\Components\PhysicsComponents.h"
#include "..\Components\PlayerInputComponent.h"
#include <memory>

CollisionManager* CollisionManager::m_instance = NULL;

CollisionManager* CollisionManager::GetSingletonPtr(){
	if (!m_instance){
		m_instance = new CollisionManager;
	}
	return m_instance;
}

void CollisionManager::CleanSingleton(){
	if (m_instance){
		delete m_instance;
		m_instance = NULL;
	}
}


bool Collision::ContactCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1){
	CollisionManager::GetSingletonPtr()->ProcessCollision(colObj0Wrap->getCollisionObject(), colObj1Wrap->getCollisionObject());
	return false;
}

CollisionManager::CollisionManager(void){ Init(); }
CollisionManager::~CollisionManager(void){}

void CollisionManager::Init(){
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_TOTT)] = &CollisionManager::PlayerTott;
	m_collision[MakeIntPair(GAME_OBJECT_TOTT, GAME_OBJECT_PLAYER)] = &CollisionManager::TottPlayer;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::BlueBubbleBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::PinkBubblePinkBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::PinkBubbleBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::BlueBubblePinkBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_TRIGGER_TEST)] = &CollisionManager::PlayerTrigger;
	m_collision[MakeIntPair(GAME_OBJECT_LEAF, GAME_OBJECT_PLAYER)] = &CollisionManager::LeafPlayer;

	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::PlayerBlueBubble;
	m_raycast_map[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::BlueBubblePlayer;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::PlayerPinkBubble;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::PinkBubblePlayer;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_PLANE)] = &CollisionManager::PlayerPlane;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLANE, GAME_OBJECT_PLAYER)] = &CollisionManager::PlanePlayer;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_TERRAIN)] = &CollisionManager::PlayerTerrain;
}

void CollisionManager::ProcessCollision(const btCollisionObject* ob_a, const btCollisionObject* ob_b){
	CollisionDef* cd_a = static_cast<CollisionDef*>(ob_a->getUserPointer());
	CollisionDef* cd_b = static_cast<CollisionDef*>(ob_b->getUserPointer());
	if (((cd_a->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT) &&
		((cd_b->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT)){
			GameObject* go_a = static_cast<GameObject*>(cd_a->data);
			GameObject* go_b = static_cast<GameObject*>(cd_b->data);
			HitMap::iterator it = m_collision.find(MakeIntPair(go_a->GetType(), go_b->GetType()));
		if (it != m_collision.end()){
			(this->*it->second)(go_a, go_b);
		}
	}
}

void CollisionManager::ProcessRaycast(const btCollisionObject* ob_a, const btCollisionObject* ob_b){
	CollisionDef* cd_a = static_cast<CollisionDef*>(ob_a->getUserPointer());
	CollisionDef* cd_b = static_cast<CollisionDef*>(ob_b->getUserPointer());
	if (((cd_a->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT) &&
		((cd_b->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT)){
			GameObject* go_a = static_cast<GameObject*>(cd_a->data);
			GameObject* go_b = static_cast<GameObject*>(cd_b->data);
			HitMap::iterator it = m_raycast_map.find(MakeIntPair(go_a->GetType(), go_b->GetType()));
		if (it != m_raycast_map.end()){
			(this->*it->second)(go_a, go_b);
		}
	}
}

inline std::pair<int,int> CollisionManager::MakeIntPair(int a, int b){
	return std::pair<int, int>(a,b);
}

void CollisionManager::PlayerTott(GameObject* player, GameObject* tott){
	std::cout << "Hi, I am a tott\n";
}

void CollisionManager::BlueBubbleBlueBubble(GameObject* blue_bubble_a, GameObject* blue_bubble_b){
	Component* comp_a = blue_bubble_a->GetComponent(COMPONENT_POINT2POINT_CONSTRAINT);
	Component* comp_b = blue_bubble_b->GetComponent(COMPONENT_POINT2POINT_CONSTRAINT);
	if (!comp_a && !comp_b){
		RigidbodyComponent* rc_a = static_cast<RigidbodyComponent*>(blue_bubble_a->GetComponent(COMPONENT_RIGIDBODY));
		RigidbodyComponent* rc_b = static_cast<RigidbodyComponent*>(blue_bubble_b->GetComponent(COMPONENT_RIGIDBODY));
		HingeConstraintComponent* constraint = new HingeConstraintComponent;
		if (!comp_a){
			blue_bubble_a->AddComponentToFront(constraint);
		}
		else{
			blue_bubble_b->AddComponentToFront(constraint);
		}
		btVector3 pivot_a = rc_b->GetRigidbody()->getWorldTransform().getOrigin() - rc_a->GetRigidbody()->getWorldTransform().getOrigin();
		btVector3 pivot_b = rc_a->GetRigidbody()->getWorldTransform().getOrigin() - rc_b->GetRigidbody()->getWorldTransform().getOrigin();
		PhysicsEngine* pe = blue_bubble_a->GetGameObjectManager()->GetPhysicsEngine();
		constraint->Init(pe, rc_a->GetRigidbody(), rc_b->GetRigidbody(), pivot_a, btVector3(0,0,0), pivot_a, pivot_a);
	}
}

void CollisionManager::PlayerBlueBubble(GameObject* player, GameObject* blue_bubble){
	PlayerInputComponent* pic = static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT));
	if (pic->GetPlayerState() == PLAYER_STATE_NORMAL || pic->GetPlayerState() == PLAYER_STATE_BOUNCING){
		CharacterController* cc = static_cast<CharacterController*>(player->GetComponent(COMPONENT_CHARACTER_CONTROLLER));
		float y_vel = cc->GetRigidbody()->getLinearVelocity().y();
		if (y_vel < 0.0f){
			if (y_vel < -4.0f && y_vel > -10.0f){   // bounce on bubble
				int player_state = PLAYER_STATE_BOUNCING;
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &blue_bubble);
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				Ogre::Vector3 impulse(0.0f, cc->GetRigidbody()->getLinearVelocity().y() * -2.2f, 0.0f);
				player->GetComponentMessenger()->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse);
			}
			else if (y_vel < -10.0f){  // go inside bubble
				int player_state = PLAYER_STATE_INSIDE_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				bool can_land = false;
				int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				player->GetComponentMessenger()->Notify(MSG_RIGIDBODY_COLLISION_FLAG_SET, &coll);
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &blue_bubble);
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);
			}
			else {   //Stand on bubble if lower than 4.0
				int player_state = PLAYER_STATE_ON_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &blue_bubble);
				player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);
			}
		}
	}
}

void CollisionManager::PlayerPlane(GameObject* player, GameObject* plane){
	/*bool on_ground = true;
	int current_state = PLAYER_STATE_INSIDE_BUBBLE;
	player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_STATE_GET, &current_state);
	if (current_state != PLAYER_STATE_INSIDE_BUBBLE){
		int player_state = PLAYER_STATE_NORMAL;
		player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
		player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &on_ground);
		CharacterController* cc = static_cast<CharacterController*>(player->GetComponent(COMPONENT_CHARACTER_CONTROLLER));
	}*/
}

void CollisionManager::LeafPlayer(GameObject* leaf, GameObject* player){
	player->GetComponentMessenger()->Notify(MSG_LEAF_PICKUP, NULL);
	player->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT))->m_leaf_sfx);
	leaf->GetGameObjectManager()->RemoveGameObject(leaf);
};

void CollisionManager::PlayerTrigger(GameObject* player, GameObject* trigger){
	std::cout << "trigger collisioin\n";
}

void CollisionManager::PlayerTerrain(GameObject* player, GameObject* terrain){
	/*bool on_ground = true;
	int current_state = PLAYER_STATE_INSIDE_BUBBLE;
	player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_STATE_GET, &current_state);
	if (current_state != PLAYER_STATE_INSIDE_BUBBLE){
		int player_state = PLAYER_STATE_NORMAL;
		player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
		player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, &on_ground);
		CharacterController* cc = static_cast<CharacterController*>(player->GetComponent(COMPONENT_CHARACTER_CONTROLLER));
	}*/
}