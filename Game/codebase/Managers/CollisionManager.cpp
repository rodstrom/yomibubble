#include "stdafx.h"
#include "CollisionManager.h"
#include "..\Components\GameObjectPrereq.h"
#include "..\Components\GameObject.h"
#include "..\PhysicsEngine.h"
#include "..\Components\PhysicsComponents.h"
#include "..\Components\PlayerInputComponent.h"

CollisionManager::CollisionManager(PhysicsEngine* physics_engine) : m_physics_engine(physics_engine){}
CollisionManager::~CollisionManager(void){}

void CollisionManager::Init(){
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_TOTT)] = &CollisionManager::PlayerTott;
	m_collision[MakeIntPair(GAME_OBJECT_TOTT, GAME_OBJECT_PLAYER)] = &CollisionManager::TottPlayer;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::BlueBubbleBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::PinkBubblePinkBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::PinkBubbleBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::BlueBubblePinkBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::PlayerBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::BlueBubblePlayer;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::PlayerPinkBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::PinkBubblePlayer;
}

void CollisionManager::Shut(){

}

void CollisionManager::ProcessCollision(const btCollisionObject* ob_a, const btCollisionObject* ob_b){
	GameObject* go_a = static_cast<GameObject*>(ob_a->getUserPointer());
	GameObject* go_b = static_cast<GameObject*>(ob_b->getUserPointer());
	HitMap::iterator it = m_collision.find(MakeIntPair(go_a->GetId(), go_b->GetId()));
	if (it != m_collision.end()){
		(this->*it->second)(go_a, go_b);
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
		Point2PointConstraintComponent* constraint = new Point2PointConstraintComponent;
		if (!comp_a){
			blue_bubble_a->AddComponent(constraint);
		}
		else{
			blue_bubble_b->AddComponent(constraint);
		}
		btVector3 pivot_a = rc_b->GetRigidbody()->getWorldTransform().getOrigin() - rc_a->GetRigidbody()->getWorldTransform().getOrigin();
		btVector3 pivot_b = rc_a->GetRigidbody()->getWorldTransform().getOrigin() - rc_b->GetRigidbody()->getWorldTransform().getOrigin();
		constraint->Init(m_physics_engine, rc_a->GetRigidbody(), rc_b->GetRigidbody(), pivot_a, pivot_b);
	}
}

void CollisionManager::PlayerBlueBubble(GameObject* player, GameObject* blue_bubble){
	PlayerInputComponent* pic = static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT));
	if (pic->GetPlayerState() == PLAYER_STATE_NORMAL){
		CharacterController* cc = static_cast<CharacterController*>(player->GetComponent(COMPONENT_CHARACTER_CONTROLLER));
		if (cc->IsFalling()){
			float test = cc->GetController()->getGhostObject()->getInterpolationLinearVelocity().y();
			int player_state = PLAYER_STATE_ON_BUBBLE;
			float gravity = 0.0f;
			player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &blue_bubble);
			player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
			player->GetComponentMessenger()->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
		}
	}
}