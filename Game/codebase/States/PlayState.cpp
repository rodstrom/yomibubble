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

PlayState::PlayState(void) : m_physics_engine(NULL), m_game_object_manager(NULL), m_level_manager(NULL), m_pause(false), m_running(true){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager("OctreeSceneManager");
	//m_scene_manager->setDisplaySceneNodes(true);
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_camera = m_scene_manager->createCamera("MainCamera");

	m_camera->setFarClipDistance(5000.0f);
	m_camera->setNearClipDistance(1.0f);
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));

	m_game_object_manager = new GameObjectManager;
	m_sound_manager = new SoundManager(m_scene_manager, m_camera);
	m_sound_manager->LoadAudio();
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system);
	
	m_variable_manager = new VariableManager();
	m_variable_manager->Init();
	m_variable_manager->LoadVariables();
	
	// Create plane mesh
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 50, 50, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	m_level_manager = new LevelManager(m_game_object_manager, m_scene_manager, m_message_system);

	m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	m_scene_manager->setShadowUseInfiniteFarPlane(false);
	m_scene_manager->setShadowTextureSelfShadow(false);
	m_scene_manager->setShadowTextureCount(1);
	m_scene_manager->setShadowTextureSize(1024);
	m_scene_manager->setShadowColour(Ogre::ColourValue(0.6f,0.6f,0.6f,1.0f));
	m_scene_manager->setShadowFarDistance(25.0f);

	/*
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
	m_level_manager->LoadLevel("Dayarea");*/

	float x = 180.0f;
	float y = 90.0f;
	float z = 230.0f;

	CharacterControllerDef tott_def;
	tott_def.friction = 1.0f;
	tott_def.velocity = 500.0f;
	tott_def.jump_power = 200.0f;
	tott_def.restitution = 0.0f;
	tott_def.step_height = 2.35f;
	tott_def.turn_speed = 1000.0f;
	tott_def.max_jump_height = 10.0f;

	

	CharacterControllerDef player_def;
	player_def.friction = 1.0f;
	player_def.velocity = m_variable_manager->GetValue("Player_Speed");
	player_def.max_speed = m_variable_manager->GetValue("Player_Max_Speed");
	player_def.deceleration = m_variable_manager->GetValue("Player_Deceleration");
	player_def.jump_power = m_variable_manager->GetValue("Player_Jump_Power");
	player_def.restitution = 0.0f;
	player_def.step_height = 0.35f;
	player_def.turn_speed = 1000.0f;
	player_def.max_jump_height = m_variable_manager->GetValue("Player_Max_Jump_Height");
	player_def.mass = 1.0f;
	player_def.radius = 0.3f;
	player_def.height = 0.4f;
	player_def.collision_filter.filter = COL_PLAYER;
	player_def.collision_filter.mask = COL_BUBBLE | COL_BUBBLE_TRIG | COL_TOTT | COL_WORLD_STATIC | COL_WORLD_TRIGGER;
	player_def.offset.y = 0.5f;

	CharacterControllerDef companion_def;
	companion_def.friction = 0.1f;
	companion_def.velocity = 2.0f;
	companion_def.max_speed = 4.0f;
	companion_def.deceleration = 10.0f;
	companion_def.jump_power = 250.0f;
	companion_def.restitution = 0.0f;
	companion_def.step_height = 0.35f;
	companion_def.turn_speed = 1000.0f;
	companion_def.max_jump_height = 0.6f;
	companion_def.mass = 1.0f;
	companion_def.radius = 0.3f;
	companion_def.height = 0.4f;
	companion_def.collision_filter.filter = COL_COMPANION;
	companion_def.collision_filter.mask = COL_BUBBLE | COL_BUBBLE_TRIG | COL_TOTT | COL_WORLD_STATIC | COL_WORLD_TRIGGER;
	companion_def.offset.y = 0.5f;			  

	Ogre::Vector3 player_pos = Ogre::Vector3::ZERO;
	Ogre::String terrain = "Dayarea";
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_TERRAIN, Ogre::Vector3(0,0,0), &terrain);
	if (terrain == "NightArea"){
		player_pos = Ogre::Vector3(230, 72, 298);
	}
	else if (terrain == "Dayarea"){
		player_pos = Ogre::Vector3(170, 75, 173);
		
		ParticleDef particleDef;
		particleDef.particle_name = "Particle/Smoke";
		m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(168,75,175), &particleDef);
	}
	else if (terrain == "try"){
		player_pos = Ogre::Vector3(230, 72, 298);
	}

	Ogre::Light* light = m_scene_manager->createLight("light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(player_pos);
	light->setSpecularColour(Ogre::ColourValue::Blue);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setAttenuation(75.0f, 0.0f, 0.1f, 0.012f);
	light->setPowerScale(0.01f);

	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(player_pos.x,player_pos.y+1.0f,player_pos.z), &player_def, "Player");
	m_game_object_manager->CreateGameObject(GAME_OBJECT_COMPANION, Ogre::Vector3(player_pos.x+15.0f, player_pos.y+1.0f, player_pos.z), &companion_def);
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_GATE, player_pos, NULL);
	PlaneDef plane_def;
	plane_def.material_name = "Examples/BeachStones";
	plane_def.plane_name = "plane";
	plane_def.friction = 1.0f;
	plane_def.restitution = 0.8f;
	plane_def.collision_filter.filter = COL_WORLD_STATIC;
	plane_def.collision_filter.mask = COL_BUBBLE | COL_PLAYER | COL_TOTT | COL_COMPANION;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(player_pos.x,player_pos.y - 2.0f,player_pos.z), &plane_def);

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
	delete m_level_manager;
	m_level_manager = NULL;
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	delete m_sound_manager;
	m_sound_manager = NULL;
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	m_scene_manager = NULL;
}

bool PlayState::Update(float dt){

	m_game_object_manager->Update(dt);

	//if(m_pause){
	//	//CreatePauseScreen();
	//	PushState(FindByName("PauseState"));
	//}
	//else {
		m_sound_manager->Update(m_camera, m_scene_manager, dt);
		m_physics_engine->Step(dt);
	
		if (m_input_manager->IsButtonDown(BTN_ARROW_UP)){
			m_physics_engine->ShowDebugDraw(true);
		}
		else{
			m_physics_engine->ShowDebugDraw(false);
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


void PlayState::CreatePauseScreen(){
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
	buttonDef.func = [this] { Resume(); };
	m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);
	
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Options";
	buttonDef.mat_start_hover = "Menu/OptionsHover";
	buttonDef.mat_start = "Menu/Options";
	buttonDef.func = [this] { Quit(); };
	m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);

}

void PlayState::Resume(){
	m_pause = false;
}

void PlayState::Quit(){
	m_running = false;
}