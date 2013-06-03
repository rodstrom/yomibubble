#ifndef _INTRO_MOVIE_STATE_H
#define _INTRO_MOVIE_STATE_H

#include "State.h"
#include "..\FadeInFadeOut.h"

class IntroMovieState : public State{
public:
	IntroMovieState(void);
	virtual ~IntroMovieState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ChangeState();
	void Picture2();

protected:
	Ogre::Overlay*			m_background;
	Ogre::Overlay*			m_picture2;
	bool					m_allow_input;
	bool					m_run_timer;
	float					m_timer;
	float					m_target_time;
	FadeInFadeOut*			m_fader;

};

#endif //_INTRO_MOVIE_STATE_H