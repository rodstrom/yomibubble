#include "stdafx.h"
#include "IntroMovieState.h"
#include "..\FadeInFadeOut.h"
#include "..\Managers\VariableManager.h"

IntroMovieState::IntroMovieState(void){}
IntroMovieState::~IntroMovieState(void){}

void IntroMovieState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_scene_manager->createCamera("IntroCamera");
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	m_viewport->setBackgroundColour(Ogre::ColourValue::White);	
	m_fader = new FadeInFadeOut("Overlays/Test", "IntroCutScene/Picture1");

	std::function<void()> funct = [this] { Picture2(); };
	m_fader->SetFadeInCallBack(funct);

	std::function<void()> func = [this] { m_run_timer = true; m_allow_input = true; };
	m_fade->SetFadeInCallBack(func);
	func = [this] { ChangeState(); };
	m_fade->SetFadeOutCallBack(func);
	m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));
	m_fader->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));

	m_allow_input = false;
	m_run_timer = false;
	m_timer = 0.0f;
	m_target_time = 5.0f;
	
	m_background = Ogre::OverlayManager::getSingleton().getByName("IntroBackground");
	m_background->show();
}


bool IntroMovieState::Update(float dt){
	if (m_run_timer){
		m_timer += dt;
		if (m_timer >= m_target_time){
			m_run_timer = false;
			m_allow_input = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
			m_fader->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
		}
	}

	if (m_allow_input){
		if (m_input_manager->IsButtonPressed(BTN_A) || m_input_manager->IsButtonPressed(BTN_START)){
			m_run_timer = false;
			m_allow_input = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
			m_fader->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));
		}
	}

	if(m_fader->IsFading()){
		m_fader->Update(dt);
	}
	return true;
}

void IntroMovieState::Exit(){
	m_background->hide();
	//m_picture2->hide();
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
}

void IntroMovieState::ChangeState(){
	std::cout << "test" << std::endl;
}

void IntroMovieState::Picture2(){
	std::cout << "test" << std::endl;
	std::function<void()> funct = [this] { /*Picture3();*/ };
	m_fader->SetFadeInCallBack(funct);
	m_picture2 = Ogre::OverlayManager::getSingleton().getByName("IntroPicture2");
	m_picture2->show();
	
}