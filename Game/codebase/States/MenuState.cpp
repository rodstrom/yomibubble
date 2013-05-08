#include "stdafx.h"
#include "MenuState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"

MenuState::MenuState(void) : m_quit(false), m_current_selected_button(0) {} 
MenuState::~MenuState(void){}

void MenuState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	//m_sound_manager->Init(m_scene_manager);

	m_camera = m_scene_manager->createCamera("MenuCamera");
	m_camera->setNearClipDistance(0.1);
	m_viewport = m_render_window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();

	//m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	//m_sound_manager->LoadAudio();
	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system);

	OverlayDef menuBackground;
	menuBackground.overlay_name = "Menu";
	menuBackground.cont_name = "Menu/Background";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	menuBackground.overlay_name = "Menu";
	menuBackground.cont_name = "Menu/BackgroundBubbles";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	ButtonDef buttonDef;
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Start";
	buttonDef.mat_start_hover = "Menu/StartHover";
	buttonDef.mat_start = "Menu/Start";
	buttonDef.func = [this] { ChangeStateToPlayState(); };
	m_buttons[0] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Options";
	buttonDef.mat_start_hover = "Menu/OptionsHover";
	buttonDef.mat_start = "Menu/Options";
	buttonDef.func = [this] { ChangeStateToPlayState(); };
	m_buttons[1] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Credits";
	buttonDef.mat_start_hover = "Menu/CreditsHover";
	buttonDef.mat_start = "Menu/Credits";
	buttonDef.func = [this] { ChangeStateToPlayState(); };
	m_buttons[2] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Quit";
	buttonDef.mat_start_hover = "Menu/QuitHover";
	buttonDef.mat_start = "Menu/Quit";
	buttonDef.func = [this] { ChangeStateToExit(); };
	m_buttons[3] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	//m_scene_manager->setSkyDome(true, "Examples/CloudySky");
}

void MenuState::Exit(){
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	//delete m_sound_manager;
	//m_sound_manager = NULL;
	//m_sound_manager->Exit();
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	m_scene_manager = NULL;
}

bool MenuState::Update(float dt){
	//m_sound_manager->Update(m_camera, m_scene_manager, dt);
	m_game_object_manager->Update(dt);
	m_physics_engine->Step(dt);
	//m_game_object_manager->LateUpdate(dt);
	if (m_input_manager->IsButtonPressed(BTN_BACK))
		return false;

	if (m_input_manager->IsButtonPressed(BTN_ARROW_DOWN)) 
		m_current_selected_button++;
	if (m_input_manager->IsButtonPressed(BTN_ARROW_UP)) 
		m_current_selected_button--;
	
	if (m_current_selected_button < 0) 
		m_current_selected_button = 3;
	if (m_current_selected_button > 3) 
		m_current_selected_button = 0;
	
	for (int i = 0; i < 4; i++){
		if(i != m_current_selected_button) {
			m_buttons[i]->GetComponentMessenger()->Notify(MSG_OVERLAY_HOVER_EXIT, NULL);
		}
		else {
			m_buttons[i]->GetComponentMessenger()->Notify(MSG_OVERLAY_HOVER_ENTER, NULL);
		}
	}

	if(m_input_manager->IsButtonPressed(BTN_A))
	{
		m_buttons[m_current_selected_button]->GetComponentMessenger()->Notify(MSG_OVERLAY_CALLBACK, NULL);
		m_buttons[m_current_selected_button]->GetComponentMessenger()->Notify(MSG_OVERLAY_HIDE, NULL);
	}

	return !m_quit;
}

void MenuState::ChangeStateToPlayState()
{
	ChangeState(FindByName("PlayState"));
}

void MenuState::ChangeStateToOptions(){}

void MenuState::ChangeStateToCredits(){}

void MenuState::ChangeStateToExit(){
	m_quit = true;
}