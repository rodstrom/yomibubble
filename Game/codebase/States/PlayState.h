#ifndef _PLAY_STATE_H_
#define _PLAY_STATE_H_

#include "State.h"
#include "..\PhysicsEngine.h"
#include "..\Artifex\Loader\ArtifexLoader.h"
#include "..\Functor.h"
#include <functional>

struct IEvent;
class LevelManager;
class GameObjectManager;
class PhysicsEngine;
class PlayState : public State
{
public:
	PlayState(void);
	~PlayState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	bool Pause() { return m_pause; }

	void SecondLoading();
	void ChangeLevel(IEvent*);

private:
	PhysicsEngine*				m_physics_engine;
	GameObjectManager*			m_game_object_manager;
	LevelManager*				m_level_manager;
	bool						m_pause;
	bool						m_running;
	bool						m_change_level;
};

#endif // _PLAY_STATE_H_