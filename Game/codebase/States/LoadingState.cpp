#include "stdafx.h"
#include "LoadingState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"
#include "..\FadeInFadeOut.h"
#include "..\Managers\VariableManager.h"

LoadingState::LoadingState(void){} 
LoadingState::~LoadingState(void){}

void LoadingState::Enter(){
	m_viewport = FindByName("PlayState")->GetViewport();
	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system, NULL);

	OverlayDef menuBackground;
	
	if (m_level == "try") { 
		menuBackground.overlay_name = "LoadingDay";
		menuBackground.cont_name = "Loading/Background2"; 
	}
	else if (m_level == "Dayarea") { 
		menuBackground.overlay_name = "LoadingNight";
		menuBackground.cont_name = "Loading/Background3"; 
	}
	else if (m_level == "NightArea") { //so this is like the "end graphics"
		menuBackground.overlay_name = "LoadingNight";
		menuBackground.cont_name = "Loading/Background3"; 
	}
	else{
		menuBackground.overlay_name = "LoadingFirst";
		menuBackground.cont_name = "Loading/Background1"; 
	}
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);
}

void LoadingState::Exit(){
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
}

bool LoadingState::Update(float dt){
	m_game_object_manager->Update(dt);
	m_render_window->update();
	return true;
}