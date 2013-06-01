#ifndef _TOTT_AI_STATES_H_
#define _TOTT_AI_STATES_H_

#include "AIPrereq.h"

class TottAIStateMove : public AIState{
public:
	TottAIStateMove(const TottMoveAIDef& def);
	virtual ~TottAIStateMove(void){}
	void Enter();
	void Exit();
	bool Update(float dt);
protected:
	Ogre::String m_animation;
	Ogre::Vector3 m_current_position;
	Ogre::Vector2 m_target_position;
};

class TottAIStateWait : public AIState {
public:
	TottAIStateWait(const TottWaitAIDef& def);
	virtual ~TottAIStateWait(void){}
	void Enter();
	void Exit();
	bool Update(float dt);
protected:
	Ogre::String m_animation;
	float m_target_time;
	float m_timer;
};


#endif // _TOTT_AI_STATES_H_