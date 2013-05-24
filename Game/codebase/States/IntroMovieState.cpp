#include "stdafx.h"
#include "IntroMovieState.h"
#include "..\..\libs\OgreVideo\include\theoraplayer\theoraplayer\TheoraVideoManager.h"


IntroMovieState::IntroMovieState(void){}


IntroMovieState::~IntroMovieState(void){}


bool IntroMovieState::Update(float dt){
	m_video_mgr->update(dt);

	if(m_video_frame)
	{
		// transfer the frame pixels to your display device, texure, graphical context or whatever you use.
		m_clip->popFrame(); // be sure to pop the frame from the frame queue when you're done
	}

	return true;
}

void IntroMovieState::Enter(){
	m_video_mgr = new TheoraVideoManager();
	m_clip = m_video_mgr->createVideoClip("konqi.ogg");
	m_video_frame = m_clip->getNextFrame();
}

void IntroMovieState::Exit(){
	delete m_video_mgr;
	m_video_mgr = NULL;
}

void IntroMovieState::ChangeState(){
}