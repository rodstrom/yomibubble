#include "stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"
#include "..\PhysicsPrereq.h"
#include "..\Managers\LevelManager.h"
#include <sstream>
#include "..\Managers\VariableManager.h"
#include "..\MessageSystem.h"
#include "LoadingState.h"
#include "..\FadeInFadeOut.h"
#include "..\CompositorListeners.h"

PlayState::PlayState(void) : m_physics_engine(NULL), m_game_object_manager(NULL), m_level_manager(NULL), m_pause(false), m_running(true), m_change_level(false){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_message_system->Register(EVT_CHANGE_LEVEL, this, &PlayState::ChangeLevel);
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	m_sound_manager = new SoundManager;
	m_sound_manager->Init(m_scene_manager, true);
	m_sound_manager->LoadAudio();
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_camera = m_scene_manager->createCamera("MainCamera");
	m_camera->setPosition(0,0,0);

	m_camera->setFarClipDistance(5000.0f);
	m_camera->setNearClipDistance(1.0f);
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system, NULL);

#ifdef NDEBUG
	if (VariableManager::GetSingletonPtr()->GetAsBool("Show", INIFILE_VISUAL_CONFIG, "Shadows", true)){
		m_scene_manager->setShadowUseInfiniteFarPlane(false);
		m_scene_manager->setShadowTextureSelfShadow(false);
		m_scene_manager->setShadowCasterRenderBackFaces(false);
		m_scene_manager->setShadowTextureCount(1);
		m_scene_manager->setShadowTextureSize(VariableManager::GetSingletonPtr()->GetAsFloat("TextureSize", INIFILE_VISUAL_CONFIG, "Shadows", 2048));
		m_scene_manager->setShadowColour(Ogre::ColourValue(0.5f,0.5f,0.6f,1.0f));
		m_scene_manager->setShadowFarDistance(VariableManager::GetSingletonPtr()->GetAsFloat("FarDistance", INIFILE_VISUAL_CONFIG, "Shadows", 30.0f));
		m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	}
	if (VariableManager::GetSingletonPtr()->GetAsBool("Show", INIFILE_VISUAL_CONFIG, "Bloom", true)){
		 Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "Bloom");
		 Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", true);
	}
#endif

	std::function<void()> func = [this] { ChangeToWinState(); };
	m_level_manager = new LevelManager(m_game_object_manager, m_scene_manager, m_physics_engine, func);
	LevelDef level1;
	level1.filepath = "try";
	level1.next_level = "Dayarea";
	level1.loading_screen_name = "LoadingDay";
	LevelDef level2;
	level2.filepath = "Dayarea";
	level2.next_level = "NightArea";
	level2.loading_screen_name = "LoadingNight";
	LevelDef level3;
	level3.filepath = "NightArea";
	level3.next_level = Ogre::StringUtil::BLANK;
	level3.loading_screen_name = "LoadingNight";
	m_level_manager->AddLevel(level1);
	m_level_manager->AddLevel(level2);
	m_level_manager->AddLevel(level3);
	m_level_manager->LoadLevel(VariableManager::GetSingletonPtr()->GetAsString("StartLevel"));
	func = [this] { m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer")); m_parent->PauseUpdate(false); m_parent->HideLoadingScreen(); };
	m_fade->SetFadeOutCallBack(func);
	m_fade->SetFadeInCallBack(NULL);
	m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
}

void PlayState::SecondLoading(){
}

void PlayState::Exit(){
	m_message_system->Unregister(EVT_CHANGE_LEVEL, this);
	delete m_level_manager;
	m_level_manager = NULL;
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	m_sound_manager->Exit();
	delete m_sound_manager;
	m_sound_manager = NULL;
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
}

bool PlayState::Update(float dt){
	m_sound_manager->Update(m_scene_manager, dt);
	m_game_object_manager->Update(dt);
	m_physics_engine->Step(dt);
	if(m_input_manager->IsButtonPressed(BTN_ARROW_UP)){
		ChangeLevel(NULL);
	}
	if (m_change_level){
    
		//State* loading = FindByName("LoadingState");
		//m_parent->Init(loading);

		//static_cast<LoadingState*>(loading)->SetLevel(m_level_manager->GetCurrentLevel());
		//loading->Enter();
		//loading->Update(1.0f);
		//m_camera->setPosition(0,0,0);
		//m_level_manager->ChangeLevel();
		//m_change_level = false;

		//loading->Exit();
	}


		if (m_input_manager->IsButtonPressed(BTN_BACK)){
			m_pause = true;
			PushState(FindByName("PauseState"));
			m_input_manager->InjectReleasedButton(BTN_BACK);	//bugfix
			//return false;
		}
		else {
			m_pause = false;
		}
	//}
	return m_running;
}

void PlayState::ChangeLevel(IEvent*) {
	std::function<void()> func = [this] { 
		m_parent->ShowLoadingScreen(m_level_manager->GetLoadingScreen()); 
		m_parent->PauseUpdate(true);
		_ReadyNextLevel();
	};
	m_fade->SetFadeOutCallBack(func);
	m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
}

void PlayState::_ReadyNextLevel(){
	std::function<void()> func = [this] { _LoadNextLevel(); };
	m_fade->SetFadeInCallBack(func);
	m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));
}

void PlayState::_LoadNextLevel(){
	m_camera->setPosition(0,0,0);
	if (m_level_manager->ChangeLevel()){
		std::function<void()> func = [this] { m_parent->PauseUpdate(false); m_parent->HideLoadingScreen(); m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer")); };
		m_fade->SetFadeOutCallBack(func);
		m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
	}
	else {
		std::function<void()> func = [this] { m_parent->PauseUpdate(false); m_parent->HideLoadingScreen(); ChangeToWinState(); };
		m_fade->SetFadeOutCallBack(func);
		m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));
	}
}

void PlayState::ChangeToWinState(){
	ChangeState(FindByName("WinState"));
}