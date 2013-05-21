#include "stdafx.h"
#include "LoadingState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"

LoadingState::LoadingState(void){} 
LoadingState::~LoadingState(void){}

void LoadingState::Enter(){
	//m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	//m_sound_manager->Init(m_scene_manager);

	//m_camera = m_scene_manager->createCamera("LoadingCamera");
	//m_camera->setNearClipDistance(0.1);
	//m_viewport = m_render_window->addViewport(m_camera);
	//m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_viewport = FindByName("PlayState")->GetViewport();
	//m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	//m_physics_engine = new PhysicsEngine;
	//m_physics_engine->Init();

	//m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	//m_sound_manager->LoadAudio();
	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system, NULL);

	OverlayDef menuBackground;

	if (m_level != "try"
		&& m_level != "Dayarea"
		&& m_level != "NightArea")
	{ 
		menuBackground.overlay_name = "LoadingFirst";
		menuBackground.cont_name = "LoadingScreenFirstArea"; 
	}

	
	
	if (m_level == "try") { 
		menuBackground.overlay_name = "LoadingDay";
		menuBackground.cont_name = "LoadingScreenDayArea"; 
	}
	else if (m_level == "Dayarea") { 
		menuBackground.overlay_name = "LoadingNight";
		menuBackground.cont_name = "LoadingScreenNightArea"; 
	}
	else if (m_level == "NightArea") { //so this is like the "end graphics"
		menuBackground.overlay_name = "LoadingNight";
		menuBackground.cont_name = "LoadingScreenNightArea"; 
	}
	//menuBackground.cont_name = "Loading/Background"; //+1,2,3
///	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);
}

void LoadingState::Exit(){
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	//m_physics_engine->CloseDebugDraw();
	//m_physics_engine->Shut();
	//delete m_physics_engine;
	//m_physics_engine = NULL;
	//delete m_sound_manager;
	//m_sound_manager = NULL;
	//m_sound_manager->Exit();
	//m_render_window->removeAllViewports();
	//Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	//m_scene_manager = NULL;
}

bool LoadingState::Update(float dt){
	//m_sound_manager->Update(m_camera, m_scene_manager, dt);
	m_game_object_manager->Update(dt);
	//m_physics_engine->Step(dt);
	m_render_window->update();
	//m_game_object_manager->LateUpdate(dt);
	return true;
}