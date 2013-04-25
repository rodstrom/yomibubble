#include "stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"
//#include "..\Components\SkyXPrereq.h"
#include <sstream>

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
	m_camera->setNearClipDistance(0.1);
	m_viewport = m_render_window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	//m_cam_node = m_scene_manager->getRootSceneNode()->createChildSceneNode("camNode");
	
	m_game_object_manager = new GameObjectManager;
	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager);
	
	ParticleDef particleDef;
	particleDef.particle_name = "Particle/Smoke";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(180,78,225), &particleDef);	
	
	//Ogre::Light* light = m_scene_manager->createLight("light1");
	//light->setType(Ogre::Light::LT_DIRECTIONAL);
	//light->setDirection(Ogre::Vector3(1,-1,0));
	//m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	
	// Create plane mesh
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 50, 50, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	float x = 180.0f;
	float y = 90.0f;
	float z = 230.0f;
	

	//m_cam_node->attachObject(m_camera);
	//Ogre::SceneNode* node = m_scene_manager->getSceneNode("camNode");

	//mArtifexLoader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, NULL, m_camera, "../../resources/terrain/");
	//mArtifexLoader->loadZone("try");

	PlaneDef plane_def;//("plane", "Examples/BeachStones");
	plane_def.material_name = "Examples/BeachStones";
	plane_def.plane_name = "plane";
	plane_def.friction = 1.0f;
	plane_def.restitution = 0.8f;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(x,y - 2.0f,z), &plane_def);
	//0.35f, 1000.0f, 500.0f, 10.0f, 
	CharControllerDef player_def;
	player_def.friction = 1.0f;
	player_def.velocity = 10.0f;
	player_def.jump_power = 200.0f;
	player_def.restitution = 0.0f;
	player_def.step_height = 0.35f;
	player_def.turn_speed = 1000.0f;
	player_def.max_jump_height = 10.0f;
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(x,y+1.0f,z), &player_def);
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(x,y-10,z), &player_def);
	
	CharControllerDef tott_def;
	tott_def.friction = 1.0f;
	tott_def.velocity = 500.0f;
	tott_def.jump_power = 200.0f;
	tott_def.restitution = 0.0f;
	tott_def.step_height = 0.35f;
	tott_def.turn_speed = 1000.0f;
	tott_def.max_jump_height = 10.0f;

	m_physics_engine->ShowDebugDraw(true);
	m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y+1.0f,z+3.0f), &tott_def);
	//m_physics_engine->CreateTerrainCollision(mArtifexLoader->mTerrain);
	//m_scene_manager->setSkyDome(true, "Examples/CloudySky");
	
	
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
	m_sound_manager->Update(m_camera, m_scene_manager, dt);
	m_game_object_manager->Update(dt);
	m_physics_engine->Step(dt);
	m_game_object_manager->LateUpdate(dt);
	if (m_input_manager->IsButtonPressed(BTN_BACK)){
		return false;
	}
	if (m_input_manager->IsButtonDown(BTN_ARROW_UP)){
		m_physics_engine->ShowDebugDraw(true);
	}
	else{
		m_physics_engine->ShowDebugDraw(false);
	}
	/*
	if (m_input_manager->IsButtonDown(BTN_S)){
		Ogre::String test = "Anders";
		*/
	//}
	return true;
}

void PlayState::Test(){
	std::cout << "Working" << std::endl;
	//SkyXSettings(Ogre::Vector3(8.85f, 7.5f, 20.5f),  -0.08f, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0022f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.52f, 0.44f), -0.991f, 3, 4), false, true, 300, false, Ogre::Radian(270), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0.35, 0.2, 0.92, 0.1), Ogre::Vector4(0.4, 0.7, 0, 0), Ogre::Vector2(0.8,1));
}


/*void PlayState::setPreset(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt, const bool& lc){
	m_sky_x->setTimeMultiplier(tm.timeMultiplier);
	m_basic_controller->setTime(t.time);
	m_basic_controller->setMoonPhase(mp.moonPhase);
	m_sky_x->getAtmosphereManager()->setOptions(atmOpt.atmosphereOpt);

	// Layered clouds
	if (lc.layeredClouds)
	{
		// Create layer cloud
		if (m_sky_x->getCloudsManager()->getCloudLayers().empty())
		{
			m_sky_x->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options ));
		}
	}
	else
	{
		// Remove layer cloud
		if (!m_sky_x->getCloudsManager()->getCloudLayers().empty())
		{
			m_sky_x->getCloudsManager()->removeAll();
		}
	}

	m_sky_x->getVCloudsManager()->setWindSpeed(preset.vcWindSpeed);
	m_sky_x->getVCloudsManager()->setAutoupdate(preset.vcAutoupdate);

	SkyX::VClouds::VClouds* vclouds = m_sky_x->getVCloudsManager()->getVClouds();

	vclouds->setWindDirection(preset.vcWindDir);
	vclouds->setAmbientColor(preset.vcAmbientColor);
	vclouds->setLightResponse(preset.vcLightResponse);
	vclouds->setAmbientFactors(preset.vcAmbientFactors);
	vclouds->setWheater(preset.vcWheater.x, preset.vcWheater.y, false);

	if (preset.volumetricClouds)
	{
		// Create VClouds
		if (!m_sky_x->getVCloudsManager()->isCreated())
		{
			// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
			m_sky_x->getVCloudsManager()->create(m_sky_x->getMeshManager()->getSkydomeRadius(m_camera));
		}
	}
	else
	{
		// Remove VClouds
		if (m_sky_x->getVCloudsManager()->isCreated())
		{
			m_sky_x->getVCloudsManager()->remove();
		}
	}

	vclouds->getLightningManager()->setEnabled(preset.vcLightnings);
	vclouds->getLightningManager()->setAverageLightningApparitionTime(preset.vcLightningsAT);
	vclouds->getLightningManager()->setLightningColor(preset.vcLightningsColor);
	vclouds->getLightningManager()->setLightningTimeMultiplier(preset.vcLightningsTM);

	//mTextArea->setCaption(buildInfoStr());

	// Reset camera position/orientation
	//mRenderingCamera->setPosition(0,0,0);
	//mRenderingCamera->setDirection(0,0,1);

	m_sky_x->update(0);

}*/