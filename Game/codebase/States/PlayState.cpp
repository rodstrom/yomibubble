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
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_camera = m_scene_manager->createCamera("MainCamera");

	m_camera->setFarClipDistance(5000.0f);
	m_camera->setNearClipDistance(1.0f);
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	//Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "Bloom");
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", true);

	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system, NULL);
	//RUN SECONDLOADING
}

void PlayState::SecondLoading(){	
	// Create plane mesh
	//Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	//Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 50, 50, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	//m_scene_manager->setShadowUseInfiniteFarPlane(false);
	//m_scene_manager->setShadowTextureSelfShadow(false);
	//m_scene_manager->setShadowCasterRenderBackFaces(false);
	//m_scene_manager->setShadowTextureCount(1);
	//m_scene_manager->setShadowTextureSize(2048);
	//m_scene_manager->setShadowColour(Ogre::ColourValue(0.5f,0.5f,0.6f,1.0f));
	//m_scene_manager->setShadowFarDistance(30.0f);
	//m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

	m_level_manager = new LevelManager(m_game_object_manager, m_scene_manager, m_physics_engine);
	LevelDef level1;
	level1.filepath = "try";
	level1.next_level = "Dayarea";
	LevelDef level2;
	level2.filepath = "Dayarea";
	level2.next_level = "NightArea";
	LevelDef level3;
	level3.filepath = "NightArea";
	level3.next_level = Ogre::StringUtil::BLANK;
	m_level_manager->AddLevel(level1);
	m_level_manager->AddLevel(level2);
	m_level_manager->AddLevel(level3);
	m_level_manager->LoadLevel(VariableManager::GetSingletonPtr()->GetAsString("StartLevel"));

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
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	m_scene_manager = NULL;
}

bool PlayState::Update(float dt){
	if (m_change_level){
		
		State* loading = FindByName("LoadingState");
		m_parent->Init(loading);
		static_cast<LoadingState*>(loading)->SetLevel(m_level_manager->GetCurrentLevel());
		loading->Enter();
		loading->Update(1.0f);

		m_level_manager->ChangeLevel();
		m_change_level = false;

		
		loading->Exit();
	}
	m_game_object_manager->Update(dt);
	
	//if(m_pause){
	//	//CreatePauseScreen();
	//	PushState(FindByName("PauseState"));
	//}
	//else {
		m_sound_manager->Update(m_scene_manager, dt);
		m_physics_engine->Step(dt);

		/*if (m_input_manager->IsButtonDown(BTN_ARROW_UP)){
			m_physics_engine->ShowDebugDraw(true);
		}
		else{
			m_physics_engine->ShowDebugDraw(false);
		}*/

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
		if(m_fade->IsFading())
		{
			m_fade->Update(dt);
		}

	return m_running;
}

void PlayState::ChangeLevel(IEvent*) {
	m_change_level = true;
}