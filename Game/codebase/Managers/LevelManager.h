#ifndef _LEVEL_MANAGER_H_
#define _LEVEL_MANAGER_H_

#include "LevelPrereq.h"

class MessageSystem;
class GameObjectManager;
class LevelManager {
public:
	LevelManager(GameObjectManager* game_object_manager, MessageSystem* message_system);
	~LevelManager(void);
	void AddLevel(const LevelDef& level_def) { m_levels.push_back(level_def); }
	void ChangeLevel(IEvent*);
	void LoadLevel(const Ogre::String& level_id);

protected:
	void FixZFighting();	// Hack to fix Z fighting with shadows on the terrain
	int m_current_level;
	std::vector<LevelDef> m_levels;
	GameObjectManager* m_game_object_manager;
	MessageSystem*	m_message_system;
};

#endif // _LEVEL_MANAGER_H_