#ifndef _LOADING_STATE_H_
#define _LOADING_STATE_H_

#include "State.h"

#include "..\Components\GameObject.h"

class LoadingState : public State
{
public:
	LoadingState(void);
	~LoadingState(void);

	bool Update(float dt);
	void Enter();
	void Exit();

	//DECLARE_STATE_CLASS(LoadingState);

private:
	GameObjectManager*			m_game_object_manager;
};



#endif // _LOADING_STATE_H_
