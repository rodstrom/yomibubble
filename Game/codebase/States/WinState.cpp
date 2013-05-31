#include "stdafx.h"
#include "WinState.h"
#include "..\FadeInFadeOut.h"
#include "..\Managers\VariableManager.h"

WinState::WinState(void){}
WinState::~WinState(void){}

void WinState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_scene_manager->createCamera("IntroCamera");
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "WinStateSceneManager");
	m_sound_manager = new SoundManager;
	m_sound_manager->Init(m_scene_manager);
	m_sound_manager->LoadAudio();
	
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	m_viewport->setBackgroundColour(Ogre::ColourValue::White);

	std::function<void()> func = [this] { m_run_timer = true; m_allow_input = true; };
	
	func = [this] { ChangeToMenuState(); };
	m_fade->SetFadeInCallBack(func);
	m_fade->SetFadeOutCallBack(func);
	m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));

	m_run_timer = true;
	m_allow_input = false;
	m_timer = 0.0f;
	m_target_time = 5.0f;

	m_bg = Ogre::OverlayManager::getSingleton().getByName("win");
	m_text = Ogre::OverlayManager::getSingleton().getByName("continue_text");
	
	m_bg->show();
	m_text->show();
}

bool WinState::Update(float dt){
	m_sound_manager->Update(m_scene_manager, dt);
	
	if (m_run_timer){
		m_timer += dt;
		m_sound_manager->Play2DMusic("Ending_theme");
		if (m_timer >= m_target_time){
			m_run_timer = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
		}
		

	}
	if(m_input_manager->IsButtonPressed(BTN_A))
			m_allow_input = true;

	if (m_allow_input){
		if (m_input_manager->IsButtonPressed(BTN_A) || m_input_manager->IsButtonPressed(BTN_START)){
			m_run_timer = false;
			m_allow_input = false;
			m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
		}
	}
	return true;
}

void WinState::Exit(){
	m_bg->hide();
	m_text->hide();
	m_render_window->removeAllViewports();
	m_sound_manager->Stop2DSound("Ending_theme");
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
}

void WinState::ChangeToMenuState(){
	
	ChangeState(FindByName("MenuState"));
	
}