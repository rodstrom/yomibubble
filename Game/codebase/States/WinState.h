#ifndef _WIN_STATE_H_
#define _WIN_STATE_H_

#include "State.h"

class WinState : public State {
public:
	WinState(void);
	~WinState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ChangeToMenuState();

protected:
	bool m_run_timer;
	float m_target_time;
	float m_timer;
	bool m_allow_input;
	Ogre::Overlay* m_bg;
};

#endif // _WIN_STATE_H_