#ifndef _LEVEL_MANAGER_H_
#define _LEVEL_MANAGER_H_

#include "LevelPrereq.h"

class PhysicsEngine;
struct IEvent;
class MessageSystem;
class GameObjectManager;
class LevelManager {
public:
	LevelManager(GameObjectManager* game_object_manager, Ogre::SceneManager* scene_manager, PhysicsEngine* physics_engine);
	~LevelManager(void);
	void AddLevel(const LevelDef& level_def) { m_levels.push_back(level_def); }
	void ChangeLevel(IEvent*);
	void LoadLevel(const Ogre::String& level_id);
	Ogre::String GetCurrentLevel() { return m_str_current_level; }

protected:
	void FixZFighting();	// Hack to fix Z fighting with shadows on the terrain
	int m_current_level;
	std::vector<LevelDef> m_levels;
	GameObjectManager* m_game_object_manager;
	MessageSystem*	m_message_system;
	Ogre::SceneManager* m_scene_manager;
	PhysicsEngine* m_physics_engine;
	Ogre::String m_str_current_level;
};

#endif // _LEVEL_MANAGER_H_