#include "stdafx.h"
#include "CollisionManager.h"
#include "..\Components\GameObjectPrereq.h"
#include "..\Components\GameObject.h"
#include "GameObjectManager.h"
#include "..\PhysicsEngine.h"
#include "..\Components\PhysicsComponents.h"
#include "..\Components\PlayerInputComponent.h"
#include <memory>
#include "..\Components\CameraComponents.h"
#include "OgreAxisAlignedBox.h"
#include "..\MessageSystem.h"
#include "..\Components\VisualComponents.h"

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
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_LEAF)] = &CollisionManager::PlayerLeaf;
	//m_collision[MakeIntPair(GAME_OBJECT_CAMERA, GAME_OBJECT_TERRAIN)] = &CollisionManager::CameraTerrain;
	//m_collision[MakeIntPair(GAME_OBJECT_TERRAIN, GAME_OBJECT_CAMERA)] = &CollisionManager::TerrainCamera;
	m_collision[MakeIntPair(GAME_OBJECT_GATE, GAME_OBJECT_PLAYER)] = &CollisionManager::GatePlayer;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_GATE)] = &CollisionManager::PlayerGate;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_QUEST_ITEM)] = &CollisionManager::PlayerQuestItem;
	m_collision[MakeIntPair(GAME_OBJECT_QUEST_ITEM, GAME_OBJECT_PLAYER)] = &CollisionManager::QuestItemPlayer;
	m_collision[MakeIntPair(GAME_OBJECT_TOTT, GAME_OBJECT_QUEST_ITEM)] = &CollisionManager::TottQuestItem;
	m_collision[MakeIntPair(GAME_OBJECT_QUEST_ITEM, GAME_OBJECT_TOTT)] = &CollisionManager::QuestItemTott;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_SPEECH_BUBBLE)] = &CollisionManager::PlayerSpeechBubble;
	m_collision[MakeIntPair(GAME_OBJECT_SPEECH_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::SpeechBubblePlayer;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_TOTT)] = &CollisionManager::BlueBubbleTott;
	m_collision[MakeIntPair(GAME_OBJECT_TOTT, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::TottBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_BLUE_BUBBLE)] = &CollisionManager::PlayerBlueBubble;
	m_collision[MakeIntPair(GAME_OBJECT_BLUE_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::BlueBubblePlayer;
	m_collision[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_LEVEL_CHANGE)] = &CollisionManager::PlayerLevelChange;
	m_collision[MakeIntPair(GAME_OBJECT_LEVEL_CHANGE, GAME_OBJECT_PLAYER)] = &CollisionManager::LevelChangePlayer;

	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_PINK_BUBBLE)] = &CollisionManager::PlayerPinkBubble;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PINK_BUBBLE, GAME_OBJECT_PLAYER)] = &CollisionManager::PinkBubblePlayer;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_PLANE)] = &CollisionManager::PlayerPlane;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLANE, GAME_OBJECT_PLAYER)] = &CollisionManager::PlanePlayer;
	m_raycast_map[MakeIntPair(GAME_OBJECT_PLAYER, GAME_OBJECT_TERRAIN)] = &CollisionManager::PlayerTerrain;
	m_raycast_map[MakeIntPair(GAME_OBJECT_TERRAIN, GAME_OBJECT_PLAYER)] = &CollisionManager::TerrainPlayer;
}

void CollisionManager::ProcessCollision(const btCollisionObject* ob_a, const btCollisionObject* ob_b){
	CollisionDef* cd_a = static_cast<CollisionDef*>(ob_a->getUserPointer());
	CollisionDef* cd_b = static_cast<CollisionDef*>(ob_b->getUserPointer());
	if ((cd_a->flag == COLLISION_FLAG_GAME_OBJECT) &&
		(cd_b->flag == COLLISION_FLAG_GAME_OBJECT)){
			GameObject* go_a = static_cast<GameObject*>(cd_a->data);
			GameObject* go_b = static_cast<GameObject*>(cd_b->data);
			int type_a = go_a->GetType();
			int type_b = go_b->GetType();
			HitMap::iterator it = m_collision.find(MakeIntPair(go_a->GetType(), go_b->GetType()));
		if (it != m_collision.end()){
			(this->*it->second)(go_a, go_b);
		}
	}
}

void CollisionManager::ProcessRaycast(const btCollisionObject* ob_a, const btCollisionObject* ob_b){
	/*CollisionDef* cd_a = static_cast<CollisionDef*>(ob_a->getUserPointer());
	CollisionDef* cd_b = static_cast<CollisionDef*>(ob_b->getUserPointer());
	if (((cd_a->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT) &&
		((cd_b->flag & COLLISION_FLAG_GAME_OBJECT) == COLLISION_FLAG_GAME_OBJECT)){
			GameObject* go_a = static_cast<GameObject*>(cd_a->data);
			GameObject* go_b = static_cast<GameObject*>(cd_b->data);
			HitMap::iterator it = m_raycast_map.find(MakeIntPair(go_a->GetType(), go_b->GetType()));
		if (it != m_raycast_map.end()){
			(this->*it->second)(go_a, go_b);
		}
	}*/
}

inline std::pair<int,int> CollisionManager::MakeIntPair(int a, int b){
	return std::pair<int, int>(a,b);
}

void CollisionManager::PlayerTott(GameObject* player, GameObject* tott){
	//std::cout << "Hi, I am a tott\n";
}

void CollisionManager::BlueBubbleBlueBubble(GameObject* blue_bubble_a, GameObject* blue_bubble_b){
	blue_bubble_a->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);	// make sure both of them gets activated
	blue_bubble_b->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);
}

void CollisionManager::PlayerBlueBubble(GameObject* player, GameObject* blue_bubble){
	blue_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);
}

void CollisionManager::PlayerPlane(GameObject* player, GameObject* plane){

}

void CollisionManager::LeafPlayer(GameObject* leaf, GameObject* player){
	int player_state = PLAYER_STATE_MOVE;
	player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_STATE_GET, &player_state);

	if (player_state != PLAYER_STATE_ON_BUBBLE && player_state != PLAYER_STATE_INSIDE_BUBBLE){
		btRigidBody* player_body = NULL;
		player->GetComponentMessenger()->Notify(MSG_RIGIDBODY_GET_BODY, &player_body, "body");
		if (player_body){
			float y_vel = player_body->getLinearVelocity().y();
			player_body->setLinearVelocity(btVector3(0.0f, y_vel, 0.0f));
		}
		Ogre::SceneNode* leaf_node = NULL;
		leaf->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &leaf_node);
		LeafEvent evt;
		evt.leaf = leaf;
		evt.leaf_node = leaf_node;
		m_message_system->Notify(&evt);
		int player_state = PLAYER_STATE_LEAF_COLLECT;
		player->GetComponentMessenger()->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
		player->GetComponentMessenger()->Notify(MSG_LEAF_PICKUP, NULL); 
		player->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT))->m_leaf_sfx);
		player->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT))->m_leaf_giggle_sfx);
	}
	else {
		LeafEvent evt;
		evt.leaf = leaf;
		evt.leaf_node = NULL;
		m_message_system->Notify(&evt);
		player->GetComponentMessenger()->Notify(MSG_LEAF_PICKUP, NULL); 
		player->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT))->m_leaf_sfx);
		player->GetComponentMessenger()->Notify(MSG_SFX2D_PLAY, &static_cast<PlayerInputComponent*>(player->GetComponent(COMPONENT_PLAYER_INPUT))->m_leaf_giggle_sfx);
		leaf->RemoveGameObject(leaf);
	}
	leaf->RemoveComponent(COMPONENT_TRIGGER);
};

void CollisionManager::PlayerTrigger(GameObject* player, GameObject* trigger){
	//std::cout << "Trigger collision\n";
}

void CollisionManager::PlayerTerrain(GameObject* player, GameObject* terrain){

}

void CollisionManager::CameraTerrain(GameObject* camera, GameObject* terrain){
	//wizardry goes here
	//send info to component (invert camera target?)
	//std::cout << "Camera : Terrain collided\n";
};

void CollisionManager::GatePlayer(GameObject* gate, GameObject* player){
	/*bool is_gate_open = false;
	gate->GetComponentMessenger()->Notify(MSG_GATE_OPEN_GET, &is_gate_open);
	if (is_gate_open){
		IEvent evt;
		evt.m_type = EVT_CHANGE_LEVEL;
		m_message_system->Notify(&evt);
	}*/
}

void CollisionManager::PlayerQuestItem(GameObject* player, GameObject* quest_item){
	//std::cout << "Player vs QuestItem\n";
};

void CollisionManager::TottQuestItem(GameObject* tott, GameObject* quest_item){
	std::cout << "Tott vs QuestItem\n";
	TOTT_STATE ts = HAPPY;
	tott->GetComponentMessenger()->Notify(MSG_TOTT_STATE_CHANGE, &ts);
	//typ ljudeffekt
	ParticleDef particleDef;
	particleDef.particle_name = "Particle/Smoke";
	quest_item->GetGameObjectManager()->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(static_cast<NodeComponent*>(quest_item->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition().x, static_cast<NodeComponent*>(quest_item->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition().y + 2, static_cast<NodeComponent*>(quest_item->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition().z), &particleDef);
	quest_item->GetGameObjectManager()->RemoveGameObject(quest_item);
	quest_item->GetGameObjectManager()->RemoveGameObject(quest_item->GetGameObjectManager()->GetGameObject( static_cast<TottController*>(tott->GetComponent(COMPONENT_CHARACTER_CONTROLLER))->GetSpeechBubble())); //gotta fix a generic name here, stored somewhere
};

void CollisionManager::PlayerSpeechBubble(GameObject* player, GameObject* speech_bubble){
	//	std::cout << "Player vs SpeechBubble\n";
	//static_cast<SpeechBubbleComponent*>(speech_bubble->GetComponent(COMPONENT_SPEECH_BUBBLE))->m_player_collide = true;
	/*
	if (static_cast<SpeechBubbleComponent*>(speech_bubble->GetComponent(COMPONENT_SPEECH_BUBBLE))->m_player_collide == false){
		AnimationMsg msg;
		msg.blend = false;
		msg.full_body = true;
		msg.id = "Excited";
		msg.index = 0;
		msg.loop = true;
		msg.wait = false;

		speech_bubble->GetGameObjectManager()->GetGameObject("TestTott")->GetComponentMessenger()->Notify(MSG_ANIMATION_PLAY, &msg);
	}
	*/
	speech_bubble->GetComponentMessenger()->Notify(MSG_SP_BUBBLE_SHOW, NULL);
	TOTT_STATE ts = CURIOUS;
	static_cast<SpeechBubbleComponent*>(speech_bubble->GetComponent(COMPONENT_SPEECH_BUBBLE))->m_tott->GetComponentMessenger()->Notify(MSG_TOTT_STATE_CHANGE, &ts);
};

void CollisionManager::TottBlueBubble(GameObject* tott, GameObject* blue_bubble){
	blue_bubble->GetComponentMessenger()->Notify(MSG_BUBBLE_CONTROLLER_ACTIVATE, NULL);
}

void CollisionManager::PlayerLevelChange(GameObject* player, GameObject* level_change){
	IEvent evt;
	evt.m_type = EVT_CHANGE_LEVEL;
	m_message_system->Notify(&evt);
}