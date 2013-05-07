#include "stdafx.h"
#include "PauseState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"

PauseState::PauseState(void) : m_quit(false) {} 
PauseState::~PauseState(void){}

void PauseState::Enter(){
	/*m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	m_camera = m_scene_manager->createCamera("PauseCamera");
	m_camera->setNearClipDistance(0.1);
	m_viewport = m_render_window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();

	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();*/

	m_viewport = FindByName("PlayState")->GetViewport();

	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager);

	OverlayDef menuBackground;
	menuBackground.overlay_name = "Pause";
	menuBackground.cont_name = "Pause/Background";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	//menuBackground.overlay_name = "Pause";
	//menuBackground.cont_name = "Pause/BackgroundBubbles";
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	ButtonDef buttonDef;
	buttonDef.overlay_name = "Pause";
	buttonDef.cont_name = "Pause/Start";
	buttonDef.mat_start_hover = "Menu/StartHover";
	buttonDef.mat_start = "Menu/Start";
	buttonDef.func = [this] { ResumeGame(); };
	m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	buttonDef.overlay_name = "Pause";
	buttonDef.cont_name = "Pause/Quit";
	buttonDef.mat_start_hover = "Menu/QuitHover";
	buttonDef.mat_start = "Menu/Quit";
	buttonDef.func = [this] { ToMainMenu(); };
	m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
}

void PauseState::Exit(){
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	//m_physics_engine->CloseDebugDraw();
	//m_physics_engine->Shut();
	//delete m_physics_engine;
	//m_physics_engine = NULL;
	//delete m_sound_manager;
	//m_sound_manager = NULL;
	//m_render_window->removeAllViewports();
	//Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	//m_scene_manager = NULL;
}

bool PauseState::Update(float dt){
	//m_sound_manager->Update(m_camera, m_scene_manager, dt);
	m_game_object_manager->Update(dt);
	//m_physics_engine->Step(dt);
	//m_game_object_manager->LateUpdate(dt);
	//if (m_input_manager->IsButtonPressed(BTN_BACK))
		//return false;

	return !m_quit;
}

void PauseState::ResumeGame()
{
	PopState();
}

void PauseState::ToMainMenu(){
	//need to flush all states first
	PopState();
	ChangeState(FindByName("MenuState"));
}