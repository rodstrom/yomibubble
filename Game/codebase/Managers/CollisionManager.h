#ifndef _N_COLLISION_MANAGER_H_
#define _N_COLLISION_MANAGER_H_

class MessageSystem;
class GameObject;
namespace Collision {
	bool ContactCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1);
}
class CollisionManager
{
public:
	static CollisionManager* GetSingletonPtr();
	static void CleanSingleton();

	void ProcessCollision(const btCollisionObject* ob_a, const btCollisionObject* ob_b);
	void ProcessRaycast(const btCollisionObject* ob_a, const btCollisionObject* ob_b);
	void UpdateColliisonChache();
	void SetMessageSystem(MessageSystem* message_system) { m_message_system = message_system; }
	
private:
	CollisionManager(void);
	~CollisionManager(void);

	static CollisionManager* m_instance;

	void Init();
	inline std::pair<int,int> MakeIntPair(int, int);

	void PlayerTott(GameObject* player, GameObject* tott);
	void TottPlayer(GameObject* tott, GameObject* player) { PlayerTott(player, tott); }
	void BlueBubbleBlueBubble(GameObject* blue_bubble_a, GameObject* blue_bubble_b);
	void PinkBubblePinkBubble(GameObject* pink_bubble_a, GameObject* pink_bubble_b) { BlueBubbleBlueBubble(pink_bubble_b, pink_bubble_a); }
	void BlueBubblePinkBubble(GameObject* blue_bubble, GameObject* pink_bubble) { BlueBubbleBlueBubble(pink_bubble, blue_bubble); }
	void PinkBubbleBlueBubble(GameObject* pink_bubble, GameObject* blue_bubble) { BlueBubbleBlueBubble(blue_bubble, pink_bubble); }
	void PlayerBlueBubble(GameObject* player, GameObject* blue_bubble);
	void BlueBubblePlayer(GameObject* blue_bubble, GameObject* player) { PlayerBlueBubble(player, blue_bubble); }
	void PlayerPinkBubble(GameObject* player, GameObject* pink_bubble) {}
	void PinkBubblePlayer(GameObject* pink_bubble, GameObject* player) { PlayerPinkBubble(player, pink_bubble); }
	void PlayerPlane(GameObject* player, GameObject* plane);
	void PlanePlayer(GameObject* plane, GameObject* player) { PlayerPlane(player, plane); }
	void LeafPlayer(GameObject* leaf, GameObject* player);
	void PlayerLeaf(GameObject* player, GameObject* leaf) { LeafPlayer(leaf, player); }
	void PlayerTrigger(GameObject* player, GameObject* trigger);

	void PlayerQuestItem(GameObject* player, GameObject* quest_item);
	void QuestItemPlayer(GameObject* quest_item, GameObject* player) { PlayerQuestItem(player, quest_item); }
	void TottQuestItem(GameObject* tott, GameObject* quest_item);
	void QuestItemTott(GameObject* quest_item, GameObject* tott) { TottQuestItem(tott, quest_item); }
	void PlayerSpeechBubble(GameObject* player, GameObject* speech_bubble);
	void SpeechBubblePlayer(GameObject* speech_bubble, GameObject* player) { PlayerSpeechBubble(player, speech_bubble); }

	void CameraTerrain(GameObject* camera, GameObject* terrain);
	void TerrainCamera(GameObject* terrain, GameObject* camera) { CameraTerrain(camera, terrain); }

	void PlayerTerrain(GameObject* player, GameObject* terrain);
	void TerrainPlayer(GameObject* terrain, GameObject* player){ PlayerTerrain(player, terrain); }

	void GatePlayer(GameObject* gate, GameObject* player);
	void PlayerGate(GameObject* player, GameObject* gate) { GatePlayer(gate, player); }

	
	typedef void (CollisionManager::*DoubleDispatch)(GameObject*, GameObject*);
	typedef std::map<std::pair<int, int>, DoubleDispatch> HitMap;
	typedef std::list<std::pair<void*,void*>> BeginEndChache;
	HitMap m_collision;
	HitMap m_raycast_map;
	MessageSystem* m_message_system;
};

#endif // _N_COLLISION_MANAGER_H_