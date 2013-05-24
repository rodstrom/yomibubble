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
	void ChangeState();
};

#endif //_INTRO_LOGO_STATE_H

