#ifndef _N_GAME_OBJECT_MANAGER_H_
#define _N_GAME_OBJECT_MANAGER_H_

#include "..\Components\GameObjectPrereq.h"
#include <string>

class MessageSystem;
class GameObject;
class InputManager;
class PhysicsEngine;
class SoundManager;
class GameObjectManager
{
public:
	GameObjectManager(void);
	~GameObjectManager(void);
	void Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager, MessageSystem* message_system);
	void Update(float dt);
	void RemoveGameObject(GameObject* gameobject);
	GameObject* CreateGameObject(int type, const Ogre::Vector3& position, void* data);
	void Shut();	// Closes the Game Object Manager, removes all objects and relations to method pointers. Only call this when you want to delete the instance.
	void ClearAllGameObjects();		// Removes all game objects from the instance, call when you want to change level.

	Ogre::SceneManager* GetSceneManager() const { return m_scene_manager; }
	InputManager* GetInputManager() const { return m_input_manager; }
	PhysicsEngine* GetPhysicsEngine() const { return m_physics_engine; }
	SoundManager* GetSoundManager() const { return m_sound_manager; }
	MessageSystem* GetMessageSystem() const { return m_message_system; }

private:
	void AddGameObject(GameObject* gameobject);
	GameObject* CreatePlayer(const Ogre::Vector3& position, void* data);
	GameObject* CreateBlueBubble(const Ogre::Vector3& position, void* data);
	GameObject* CreatePinkBubble(const Ogre::Vector3& position, void* data);
	GameObject* CreateButton(const Ogre::Vector3& position, void* data);
	GameObject* Create2DOverlay(const Ogre::Vector3& position, void* data);
	GameObject* CreateTott(const Ogre::Vector3& position, void* data);
	GameObject* CreatePlane(const Ogre::Vector3& position, void* data);
	GameObject* CreateLeaf(const Ogre::Vector3& position, void* data);
	GameObject* CreateGUI(const Ogre::Vector3& position, void* data);
	GameObject* CreateTestTrigger(const Ogre::Vector3& position, void* data);
	GameObject* CreateCompanion(const Ogre::Vector3& position, void* data);
	GameObject* CreateTerrain(const Ogre::Vector3& position, void* data);
	GameObject* CreateGate(const Ogre::Vector3& position, void* data);
	GameObject* CreateLoadingScreen(const Ogre::Vector3& position, void* data);

	typedef GameObject* (GameObjectManager::*CreateObjectFptr)(const Ogre::Vector3&, void* data);
	
	CreateObjectFptr		m_create_fptr[GAME_OBJECT_SIZE];
	std::list<GameObject*>	m_game_objects;
	std::list<GameObject*>	m_updateable_game_objects;
	PhysicsEngine*			m_physics_engine;
	Ogre::SceneManager*		m_scene_manager;
	InputManager*			m_input_manager;
	Ogre::Viewport*			m_viewport;
	SoundManager*			m_sound_manager;
	MessageSystem*			m_message_system;
	Ogre::SceneNode*		m_node;
};

#endif // _N_GAME_OBJECT_MANAGER_H_