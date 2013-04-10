#ifndef _N_GAME_OBJECT_MANAGER_H_
#define _N_GAME_OBJECT_MANAGER_H_

#include "..\Components\GameObjectPrereq.h"
#include "..\Components\GameObjectFactory.h"

class InputManager;
class PhysicsEngine;
class GameObjectManager : public IGameObjectFactory
{
public:
	GameObjectManager(void);
	~GameObjectManager(void);
	void Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport);
	void Update(float dt);
	void LateUpdate(float dt);		// Update performed after physics simulation.	
	void RemoveGameObject(GameObject* gameobject);
	GameObject* CreateGameObject(int type, const Ogre::Vector3& position, void* data);
	void Shut();

private:
	GameObject* CreatePlayer(const Ogre::Vector3& position, void* data);
	GameObject* CreateBlueBubble(const Ogre::Vector3& position, void* data);
	GameObject* CreatePinkBubble(const Ogre::Vector3& position, void* data);
	void AddGameObject(GameObject* gameobject);

	typedef GameObject* (GameObjectManager::*CreateObjectFptr)(const Ogre::Vector3&, void* data);
	
	CreateObjectFptr		m_create_fptr[GAME_OBJECT_SIZE];
	std::list<GameObject*>	m_game_objects;
	std::list<GameObject*>	m_updateable_game_objects;
	std::list<GameObject*>	m_late_update_objects;
	PhysicsEngine*			m_physics_engine;
	Ogre::SceneManager*		m_scene_manager;
	InputManager*			m_input_manager;
	Ogre::Viewport*			m_viewport;
};

#endif // _N_GAME_OBJECT_MANAGER_H_