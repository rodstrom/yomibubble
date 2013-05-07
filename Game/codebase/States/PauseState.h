#ifndef _PAUSE_STATE_H_
#define _PAUSE_STATE_H_

#include "State.h"

class PauseState : public State
{
public:
	PauseState(void);
	~PauseState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ResumeGame();
	void ToMainMenu();

	//DECLARE_STATE_CLASS(PauseState);

private:
	GameObjectManager*			m_game_object_manager;
	PhysicsEngine*				m_physics_engine;
	SoundManager*				m_sound_manager;
	bool						m_quit;

};



#endif // _PAUSE_STATE_H_
