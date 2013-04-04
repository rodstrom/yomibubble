#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include "State.h"

class MenuState : public State
{
public:
	MenuState(void);
	~MenuState(void);

	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	void Enter();
	void Exit();

	//DECLARE_STATE_CLASS(MenuState);

private:
};



#endif // _MENU_STATE_H_
