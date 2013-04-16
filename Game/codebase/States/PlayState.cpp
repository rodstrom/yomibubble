#include "stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Audio\SoundManager.h"
#include "..\Managers\GameObjectManager.h"

PlayState::PlayState(void) : m_physics_engine(NULL), m_game_object_manager(NULL){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_camera = m_scene_manager->createCamera("MainCamera");
	//m_camera->setPosition(Ogre::Vector3(500,500,500));
	//m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(5);
	m_viewport = m_render_window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	//m_cam_node = m_scene_manager->getRootSceneNode()->createChildSceneNode("camNode");
	
	m_game_object_manager = new GameObjectManager;
	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager);

	
	//Ogre::String overlay = "testOverlay";
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &overlay);
	


	/*Ogre::Light* light = m_scene_manager->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));*/
	//m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	
	// Create plane mesh
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 500, 500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	
	m_physics_engine->ShowDebugDraw(false);
	//m_scene_manager->setSkyDome(true, "Examples/CloudySky");
	CharControllerDef player_def(COLLIDER_CAPSULE, 0.35f, 1000.0f, 5.0f);
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(1000,1001,1000), &player_def);
	//m_cam_node->attachObject(m_camera);
	//Ogre::SceneNode* node = m_scene_manager->getSceneNode("camNode");
	mArtifexLoader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, NULL, m_camera, "../../resources/terrain/");
#ifdef _DEBUG
	mArtifexLoader->loadZone("demozone", false, false, false, true, false, false, true, true, true, true);
#else
	mArtifexLoader->loadZone("demozone", true, true, true, true, true, false, true, true, true, true);
#endif
	PlaneDef plane_def("plane", "Examples/BeachStones");
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(1000,1000,1000), &plane_def);

	CharControllerDef tott_def(COLLIDER_CAPSULE, 0.35f, 500.0f, 5.0f);
	m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(1001,1001,1001), &tott_def);
	//m_physics_engine->CreateTerrainCollision(*mArtifexLoader->mTerrainInfo);
}

void PlayState::Exit(){

	/*m_plane_body->getMotionState();
	delete m_plane_body;
	delete m_ground_motion_state;
	delete m_plane_shape->getMeshInterface();
	delete m_plane_shape;*/

	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	m_scene_manager = NULL;
}

bool PlayState::Update(float dt){
	m_sound_manager->Update(m_camera, dt);
	m_game_object_manager->Update(dt);
	m_physics_engine->Step(dt);
	m_game_object_manager->LateUpdate(dt);
	if (m_input_manager->IsButtonPressed(BTN_BACK)){
		return false;
	}
	if (m_input_manager->IsButtonDown(BTN_A)){
		m_physics_engine->ShowDebugDraw(true);
	}
	else{
		m_physics_engine->ShowDebugDraw(false);
	}
	m_game_object_manager->LateUpdate(dt);
	return true;
}

