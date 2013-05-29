#ifndef _INTRO_LOGO_STATE_H
#define _INTRO_LOGO_STATE_H

#include "State.h"

class IntroLogoState : public State{
public:
	IntroLogoState(void);
	virtual ~IntroLogoState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ChangeToMenuState();
protected:
	Ogre::Overlay* m_logo;
	bool m_run_timer;
	bool m_allow_input;
	float m_timer;
	float m_target_time;
};

#endif //_INTRO_LOGO_STATE_H

