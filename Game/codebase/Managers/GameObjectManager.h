#ifndef _N_GAME_OBJECT_MANAGER_H_
#define _N_GAME_OBJECT_MANAGER_H_

#include "..\Components\GameObjectPrereq.h"

class GameObject;
class InputManager;
class PhysicsEngine;
class SoundManager;
class GameObjectManager
{
public:
	GameObjectManager(void);
	~GameObjectManager(void);
	void Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager);
	void Update(float dt);
	void LateUpdate(float dt);		// Update performed after physics simulation.	
	void RemoveGameObject(GameObject* gameobject);
	GameObject* CreateGameObject(int type, const Ogre::Vector3& position, void* data);
	void Shut();

private:
	void AddGameObject(GameObject* gameobject);
	GameObject* CreatePlayer(const Ogre::Vector3& position, void* data);
	GameObject* CreateBlueBubble(const Ogre::Vector3& position, void* data);
	GameObject* CreatePinkBubble(const Ogre::Vector3& position, void* data);
	GameObject* Create2DOverlay(const Ogre::Vector3& position, void* data);
	GameObject* CreateTott(const Ogre::Vector3& position, void* data);
	GameObject* CreatePlane(const Ogre::Vector3& position, void* data);
	GameObject* CreateLeaf(const Ogre::Vector3& position, void* data);

	typedef GameObject* (GameObjectManager::*CreateObjectFptr)(const Ogre::Vector3&, void* data);
	
	CreateObjectFptr		m_create_fptr[GAME_OBJECT_SIZE];
	std::list<GameObject*>	m_game_objects;
	std::list<GameObject*>	m_updateable_game_objects;
	std::list<GameObject*>	m_late_update_objects;
	PhysicsEngine*			m_physics_engine;
	Ogre::SceneManager*		m_scene_manager;
	InputManager*			m_input_manager;
	Ogre::Viewport*			m_viewport;
	SoundManager*			m_sound_manager;
	Ogre::SceneNode*		m_node;
};

#endif // _N_GAME_OBJECT_MANAGER_H_