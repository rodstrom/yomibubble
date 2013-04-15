#include "stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\GameObjectManager.h"
#include "..\Audio\SoundManager.h"

PlayState::PlayState(void) : m_physics_engine(NULL), m_game_object_manager(NULL){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_camera = m_scene_manager->createCamera("MainCamera");
	m_camera->setPosition(Ogre::Vector3(0,0,50));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(5);
	m_viewport = m_render_window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	
	m_scene_manager->createSceneNode("testnode");

	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();
	
//	m_scene_manager->addListener(m_sound_manager->GetSoundManager()->getListener());

	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager);
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(0,0,0), NULL);

	Ogre::Light* light = m_scene_manager->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 200, 200, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	m_plane = m_scene_manager->createEntity("LightPlaneEntity", "plane");
	m_scene_manager->getRootSceneNode()->createChildSceneNode()->attachObject(m_plane);
	m_plane->setMaterialName("Examples/BeachStones");
	BtOgre::StaticMeshToShapeConverter converter1(m_plane);
	m_plane_shape = converter1.createTrimesh();
	m_ground_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
	m_plane_body = new btRigidBody(0, m_ground_motion_state, m_plane_shape, btVector3(0,0,0));
	m_physics_engine->AddRigidBody(m_plane_body);

	m_physics_engine->ShowDebugDraw(false);
	m_scene_manager->setSkyDome(true, "Examples/CloudySky");
}

void PlayState::Exit(){
	m_physics_engine->RemoveRigidBody(m_plane_body);

	m_plane_body->getMotionState();
	delete m_plane_body;
	delete m_ground_motion_state;
	delete m_plane_shape->getMeshInterface();
	delete m_plane_shape;

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



bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt){
	m_game_object_manager->Update(evt.timeSinceLastFrame);
	m_sound_manager->Update(m_camera, evt.timeSinceLastFrame);

	/*float cameraMovementSpeed = 50.0f;
	Ogre::Vector3 translateCamera(0,0,0);
	if (m_input_manager->IsButtonDown(BTN_W))
		translateCamera += Ogre::Vector3(0,0,-1);
	if (m_input_manager->IsButtonDown(BTN_S))
		translateCamera += Ogre::Vector3(0,0,1);
	if (m_input_manager->IsButtonDown(BTN_A))
		translateCamera += Ogre::Vector3(-1,0,0);
	if (m_input_manager->IsButtonDown(BTN_D))
		translateCamera += Ogre::Vector3(1,0,0);
	m_camera->moveRelative(translateCamera*evt.timeSinceLastFrame*cameraMovementSpeed);
	float rotX = m_input_manager->GetMousePosition().rel_x * evt.timeSinceLastFrame * -1;
	float rotY = m_input_manager->GetMousePosition().rel_y * evt.timeSinceLastFrame * -1;
	m_camera->yaw(Ogre::Radian(rotX));
	m_camera->pitch(Ogre::Radian(rotY));*/


	m_physics_engine->Step(evt.timeSinceLastFrame, 10);
	m_game_object_manager->LateUpdate(evt.timeSinceLastFrame);
	return true;
}
