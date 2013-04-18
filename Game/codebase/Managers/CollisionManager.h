#ifndef _N_COLLISION_MANAGER_H_
#define _N_COLLISION_MANAGER_H_

class PhysicsEngine;
class GameObject;
class CollisionManager
{
public:
	CollisionManager(PhysicsEngine* physics_engine);
	~CollisionManager(void);

	void ProcessCollision(const btCollisionObject* ob_a, const btCollisionObject* ob_b);
	void Init();
	void Shut();

private:
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

	typedef void (CollisionManager::*DoubleDispatch)(GameObject*, GameObject*);
	typedef std::map<std::pair<int, int>, DoubleDispatch> HitMap;
	HitMap m_collision;
	PhysicsEngine* m_physics_engine;
};

#endif // _N_COLLISION_MANAGER_H_