#ifndef _AI_PREREQ_H_
#define _AI_PREREQ_H_

enum EAIState {
	AI_STATE_MOVE,
	AI_STATE_WAIT,
	AI_STATE_SIZE
};

class ComponentMessenger;
class AIState{
public:
	AIState(void){}
	virtual ~AIState(void){}

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual bool Update(float dt) = 0;
	void Init(ComponentMessenger* messenger) { m_messenger = messenger; }
	int GetState() { return m_state; }

protected:
	int m_state;
	ComponentMessenger* m_messenger;
};

struct TottWaitAIDef{
	float target_time;
	Ogre::String animation;
};

struct TottMoveAIDef{
	Ogre::Vector2 target_position;
	Ogre::String animation;
};

#endif // _AI_PREREQ_H_