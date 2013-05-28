#ifndef _N_GAME_OBJECT_MANAGER_H_
#define _N_GAME_OBJECT_MANAGER_H_

#include "..\Components\GameObjectPrereq.h"
#include <string>

class MessageSystem;
class GameObject;
class InputManager;
class PhysicsEngine;
class SoundManager;
class VariableManager;
class GameObjectManager
{
public:
	GameObjectManager(void);
	~GameObjectManager(void);
	void Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager, MessageSystem* message_system, VariableManager* variable_manager);
	void Update(float dt);
	void RemoveGameObject(GameObject* gameobject);
	GameObject* CreateGameObject(int type, const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	void Shut();	// Closes the Game Object Manager, removes all objects and relations to method pointers. Only call this when you want to delete the instance.
	void ClearAllGameObjects();		// Removes all game objects from the instance, call when you want to reuse the instance.

	GameObject* GetGameObject(const Ogre::String& id);

	Ogre::SceneManager* GetSceneManager() const { return m_scene_manager; }
	InputManager* GetInputManager() const { return m_input_manager; }
	PhysicsEngine* GetPhysicsEngine() const { return m_physics_engine; }
	SoundManager* GetSoundManager() const { return m_sound_manager; }
	MessageSystem* GetMessageSystem() const { return m_message_system; }

private:
	void ClearQueuedGameObjects();	// removes and deletes game objects saved in teh remove list. This is called if the remove list is not empty on each update cycle
	void RemoveBubble(GameObject* bubble);
	void CheckBubbleSize();
	void AddGameObject(GameObject* gameobject);
	GameObject* CreatePlayer(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateBlueBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreatePinkBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateButton(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* Create2DOverlay(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateTott(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreatePlane(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateLeaf(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateGUI(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateTestTrigger(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateCompanion(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateTerrain(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateGate(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateLoadingScreen(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateNextLevelTrigger(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreatePlayerCamera(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateParticleEffect(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateQuestItem(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateSpeechBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);
	GameObject* CreateRockSlide(const Ogre::Vector3& position, void* data, const Ogre::String& id = Ogre::StringUtil::BLANK);

	typedef GameObject* (GameObjectManager::*CreateObjectFptr)(const Ogre::Vector3&, void* data, const Ogre::String& id);

	CreateObjectFptr		m_create_fptr[GAME_OBJECT_SIZE];
	std::list<GameObject*>	m_remove_list;
	std::list<GameObject*>	m_game_objects;
	std::list<GameObject*>	m_updateable_game_objects;
	std::deque<GameObject*> m_bubbles;

	PhysicsEngine*			m_physics_engine;
	Ogre::SceneManager*		m_scene_manager;
	InputManager*			m_input_manager;
	Ogre::Viewport*			m_viewport;
	SoundManager*			m_sound_manager;
	MessageSystem*			m_message_system;
	Ogre::SceneNode*		m_node;
	VariableManager*		m_variable_manager;

	Ogre::SceneNode*		m_temp_node;

	int m_leaf_iterations;
	int m_particle_iterations;
	unsigned int m_max_bubbles;
};

#endif // _N_GAME_OBJECT_MANAGER_H_