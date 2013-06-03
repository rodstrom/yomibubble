#ifndef _LEVEL_MANAGER_H_
#define _LEVEL_MANAGER_H_

#include "LevelPrereq.h"

class PhysicsEngine;
struct IEvent;
class GameObjectManager;
class LevelManager {
public:
	LevelManager(GameObjectManager* game_object_manager, Ogre::SceneManager* scene_manager, PhysicsEngine* physics_engine, std::function<void()> change_state);
	~LevelManager(void);
	void AddLevel(const LevelDef& level_def) { m_levels.push_back(level_def); }
	bool ChangeLevel();
	void LoadLevel(const Ogre::String& level_id);
	void LoadPlane();		// will load a plane and a player at position 0,0,0 and enable debug draw (For testing purposes)
	Ogre::String GetCurrentLevel() { return m_str_current_level; }
	const Ogre::String& GetLoadingScreen() { return m_levels[m_current_level].loading_screen_name; }

protected:
	void FixZFighting();	// Hack to fix Z fighting with shadows on the terrain
	int m_current_level;
	std::vector<LevelDef> m_levels;
	GameObjectManager* m_game_object_manager;
	Ogre::SceneManager* m_scene_manager;
	PhysicsEngine* m_physics_engine;
	Ogre::String m_str_current_level;
	std::function<void()> m_change_state;
};

#endif // _LEVEL_MANAGER_H_