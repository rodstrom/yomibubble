#ifndef _INTRO_MOVIE_STATE_H
#define _INTRO_MOVIE_STATE_H

//#include "..\..\libs\OgreVideo\include\theoraplayer\theoraplayer\TheoraVideoManager.h"

#include "State.h"

class IntroMovieState : public State{
public:
	IntroMovieState(void);
	virtual ~IntroMovieState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ChangeState();

protected:
	//TheoraVideoManager*		m_video_mgr;
	//TheoraVideoClip*		m_clip;
	//TheoraVideoFrame*		m_video_frame;
};

#endif //_INTRO_MOVIE_STATE_H