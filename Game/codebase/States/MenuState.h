#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include "State.h"


class MenuState : public State
{
public:
	MenuState(void);
	~MenuState(void);

	bool Update(float dt);
	void Enter();
	void Exit();

	//DECLARE_STATE_CLASS(MenuState);

private:
	GameObjectManager*			m_game_object_manager;
	PhysicsEngine*				m_physics_engine;
	SoundManager*				m_sound_manager;

};



#endif // _MENU_STATE_H_
