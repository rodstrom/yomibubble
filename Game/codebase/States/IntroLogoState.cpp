#include "stdafx.h"
#include "IntroLogoState.h"
#include "..\FadeInFadeOut.h"
#include "..\Managers\VariableManager.h"

IntroLogoState::IntroLogoState(void){
}


IntroLogoState::~IntroLogoState(void){
}


void IntroLogoState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_scene_manager->createCamera("IntroCamera");
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	m_viewport->setBackgroundColour(Ogre::ColourValue::White);

	std::function<void()> func = [this] { m_run_timer = true; m_allow_input = true; };
	m_fade->SetFadeInCallBack(func);
	func = [this] { ChangeToMenuState(); };
	m_fade->SetFadeOutCallBack(func);
	m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));
	
	m_allow_input = false;
	m_run_timer = false;
	m_timer = 0.0f;
	m_target_time = 5.0f;
	
	m_logo = Ogre::OverlayManager::getSingleton().getByName("IntroLogo");
	m_logo->show();
}

bool IntroLogoState::Update(float dt){
	if (m_run_timer){
		m_timer += dt;
		if (m_timer >= m_target_time){
			m_run_timer = false;
			m_allow_input = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
		}
	}

	if (m_allow_input){
		if (m_input_manager->IsButtonPressed(BTN_A) || m_input_manager->IsButtonPressed(BTN_START)){
			m_run_timer = false;
			m_allow_input = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
		}
	}
	return true;
}

void IntroLogoState::ChangeToMenuState(){
	ChangeState(FindByName("MenuState"));
}

void IntroLogoState::Exit(){
	m_logo->hide();
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
}
