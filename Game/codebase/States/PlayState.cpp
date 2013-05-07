#include "stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"
#include "..\PhysicsPrereq.h"
#include "..\Managers\LevelManager.h"
#include <sstream>

PlayState::PlayState(void) : m_physics_engine(NULL), m_game_object_manager(NULL){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");

	//m_scene_manager->setAmbientLight(Ogre::ColourValue(0.2f,0.2f,0.2f,1.0f));
	//m_scene_manager->setDisplaySceneNodes(true);
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_camera = m_scene_manager->createCamera("MainCamera");
	//m_camera->setPosition(Ogre::Vector3(500,500,500));
	//m_camera->lookAt(Ogre::Vector3(0,0,0));
	//m_camera->setNearClipDistance(5.0f);
	m_camera->setFarClipDistance(5000.0f);
	m_viewport = m_render_window->addViewport(m_camera);
	//m_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	//m_cam_node = m_scene_manager->getRootSceneNode()->createChildSceneNode("camNode");

	m_game_object_manager = new GameObjectManager;
	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system);
	
	m_level_manager = new LevelManager(m_game_object_manager, m_message_system);

	// Create plane mesh
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 50, 50, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	float x = 180.0f;
	float y = 90.0f;
	float z = 230.0f;
	/*
	ParticleDef particleDef;
	particleDef.particle_name = "Particle/Smoke";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(178+5,72,240), &particleDef);	
	*/
	//m_cam_node->attachObject(m_camera);
	//Ogre::SceneNode* node = m_scene_manager->getSceneNode("camNode");
	/*mArtifexLoader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, NULL, m_camera, "../../resources/terrain/");
#ifdef _DEBUG
	mArtifexLoader->loadZone("try", false, false, false, false, true, false, true, true, true, true);
#else
	mArtifexLoader->loadZone("try", true, true, true, false, true, false, true, true, true, true);
#endif*/
	//PlaneDef plane_def("plane", "Examples/BeachStones");
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(x,y - 2.0f,z), &plane_def);

	//mArtifexLoader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, m_camera, m_scene_manager->getSceneNode(""), m_game_object_manager, m_sound_manager, "../../resources/terrain/");
	//mArtifexLoader->loadZone("try");
	/*
	PlaneDef plane_def;
	plane_def.material_name = "Examples/BeachStones";
	plane_def.plane_name = "plane";
	plane_def.friction = 1.0f;
	plane_def.restitution = 0.8f;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(x,y - 2.0f,z), &plane_def);
	*/
	//0.35f, 1000.0f, 500.0f, 10.0f, 
	/*TriggerDef trigger_def;
	trigger_def.body_type = DYNAMIC_BODY;
	trigger_def.type = COLLIDER_BOX;
	trigger_def.x = 0.5f;
	trigger_def.y = 0.5f;
	trigger_def.z = 0.5f;
	trigger_def.origin = Ogre::Vector3(0,-1,0);
	trigger_def.mass = 0.0f;*/

	
	CharacterControllerDef tott_def;
	tott_def.friction = 1.0f;
	tott_def.velocity = 500.0f;
	tott_def.jump_power = 200.0f;
	tott_def.restitution = 0.0f;
	tott_def.step_height = 2.35f;
	tott_def.turn_speed = 1000.0f;
	tott_def.max_jump_height = 10.0f;

	//m_physics_engine->ShowDebugDraw(true);
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y+1.0f,z+3.0f), &tott_def);
	//m_physics_engine->CreateTerrainCollision(mArtifexLoader->mTerrain);
	//m_scene_manager->setSkyDome(true, "Examples/CloudySky");
	
	/*TriggerDef trigger_def;
	trigger_def.body_type = STATIC_BODY;
	trigger_def.collider_type = COLLIDER_BOX;
	trigger_def.x = 2.0f;
	trigger_def.y = 2.0f;
	trigger_def.z = 2.0f;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_TRIGGER_TEST, Ogre::Vector3(x + 2.0f, y - 10.0f ,z + 2.0f), &trigger_def);*/
	Ogre::String terrain = "Dayarea";
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_TERRAIN, Ogre::Vector3(0,0,0), &terrain);

	CharacterControllerDef player_def;
	player_def.friction = 1.0f;
	player_def.velocity = 5.0f;
	player_def.max_speed = 3.0f;
	player_def.deceleration = 10.0f;
	player_def.jump_power = 250.0f;
	player_def.restitution = 0.0f;
	player_def.step_height = 0.35f;
	player_def.turn_speed = 1000.0f;
	player_def.max_jump_height = 0.6f;
	player_def.mass = 1.0f;
	player_def.radius = 0.3f;
	player_def.height = 0.4f;
	player_def.collision_filter.filter = COL_PLAYER;
	player_def.collision_filter.mask = COL_BUBBLE | COL_BUBBLE_TRIG | COL_TOTT | COL_WORLD_STATIC | COL_WORLD_TRIGGER;
	player_def.offset.y = 0.5f;
	/*DynamicCharacterControllerDef player_def;
	player_def.deceleration = 1.0f;
	player_def.height = 1.0f;
	player_def.jump_impulse = 10.0f;
	player_def.jump_recharge_time = 2.0f;
	player_def.mass = 1.0f;
	player_def.max_speed = 2.0f;
	player_def.radius = 0.5f;
	player_def.step_height = 0.35f;*/
	
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(200,80,200), &player_def);

	Ogre::Vector3 player_pos = Ogre::Vector3::ZERO;
	//Ogre::Vector3 player_pos(700.0f, 800.0f, 700.0f);

	if (terrain == "NightArea"){
		player_pos = Ogre::Vector3(230, 72, 298);
	}
	else if (terrain == "Dayarea"){
		player_pos = Ogre::Vector3(170, 75, 173);
		
		ParticleDef particleDef;
		particleDef.particle_name = "Particle/Smoke";
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(168,75,175), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(179,84,191), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(178,79,229), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(236,90,234), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(157,77,213), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(149,79,202), &particleDef);
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(131,89,203), &particleDef);
		
		
	}
	else if (terrain == "try"){
		player_pos = Ogre::Vector3(230, 72, 298);
	}

	/*Ogre::Light* light = m_scene_manager->createLight("light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(player_pos + Ogre::Vector3(0.0,2.0f,0.0f));
	light->setSpecularColour(Ogre::ColourValue::Blue);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setAttenuation(75.0f, 1.0f, 1.1f, 0.012f);
	light->setCastShadows(true);*/

	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(player_pos.x,player_pos.y+8.5f,player_pos.z), &player_def);
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_GATE, player_pos, NULL);
	PlaneDef plane_def;
	plane_def.material_name = "Examples/BeachStones";
	plane_def.plane_name = "plane";
	plane_def.friction = 1.0f;
	plane_def.restitution = 0.8f;
	plane_def.collision_filter.filter = COL_WORLD_STATIC;
	plane_def.collision_filter.mask = COL_BUBBLE | COL_PLAYER | COL_TOTT;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(player_pos.x,player_pos.y + 8.0f,player_pos.z), &plane_def);
	m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	m_scene_manager->setShadowUseInfiniteFarPlane(false);
	m_scene_manager->setShadowTextureSelfShadow(true);
	m_scene_manager->setShadowTextureCount(1);
	m_scene_manager->setShadowTextureSize(2048);
	m_scene_manager->setShadowColour(Ogre::ColourValue(0.6f,0.6f,0.6f,1.0f));
	m_scene_manager->setShadowFarDistance(30.0f);
	m_camera->setNearClipDistance(1.0f);
	//m_scene_manager->showBoundingBoxes(true);
	
if(MaterialManager::getSingleton().getByName("Ogre/TextureShadowCaster").isNull())
    // Render a frame to get the shadow materials created
		Ogre::Root::getSingleton().renderOneFrame();

   // Get all shadow materials
	std::vector<MaterialPtr> tmpMaterials;
   TexturePtr tmpTexturePtr = m_scene_manager->getShadowTexture(0);
   String tmpMaterialName = tmpTexturePtr->getName() + "Mat" + m_scene_manager->getName();
   tmpMaterials.push_back(MaterialManager::getSingleton().getByName(tmpMaterialName));
   tmpMaterials.push_back(MaterialManager::getSingleton().getByName("Ogre/TextureShadowCaster"));
   tmpMaterials.push_back(MaterialManager::getSingleton().getByName("Ogre/TextureShadowReceiver"));

   // Loop through the list of shadow materials
   unsigned int i = 0;
   for( ; i < tmpMaterials.size(); i++ )
   {
    // Check if the current shadow material exists
    if( !tmpMaterials[i].isNull() )
     // Set the depth bias of the shadow material
     tmpMaterials[i]->getTechnique(0)->getPass(0)->setDepthBias(5.0f);
   }

   // Clear the temporary list of shadow materials
   tmpMaterials.clear();

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
	if (m_input_manager->IsButtonPressed(BTN_BACK)){
		return false;
	}
	if (m_input_manager->IsButtonDown(BTN_ARROW_UP)){
		m_physics_engine->ShowDebugDraw(true);
	}
	else{
		m_physics_engine->ShowDebugDraw(false);
	}
	
	return true;
}
