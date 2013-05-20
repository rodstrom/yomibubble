#include "stdafx.h"
#include "LevelManager.h"
#include "GameObjectManager.h"
#include "..\MessageSystem.h"
#include "..\PhysicsEngine.h"
#include "VariableManager.h"
#include "..\PhysicsPrereq.h"

LevelManager::LevelManager(GameObjectManager* game_object_manager, Ogre::SceneManager* scene_manager, PhysicsEngine* physics_engine) : 
	m_game_object_manager(game_object_manager), m_scene_manager(scene_manager), m_physics_engine(physics_engine), m_current_level(0){
}
LevelManager::~LevelManager(void){}


void LevelManager::ChangeLevel(){
	Ogre::String next_level = m_levels[m_current_level].next_level;
	m_game_object_manager->ClearAllGameObjects();
	m_scene_manager->clearScene();
	m_physics_engine->ResetPhysicsEngine();
	// set loading screen game object here
	LoadLevel(next_level);
}

void LevelManager::LoadLevel(const Ogre::String& level_id){
	m_str_current_level = level_id;
	for (unsigned int i = 0; i < m_levels.size(); i++){
		if (m_levels[i].filepath == level_id){
			m_current_level = i;
			Ogre::String filepath = m_levels[i].filepath;
			m_game_object_manager->CreateGameObject(GAME_OBJECT_TERRAIN, Ogre::Vector3(0,0,0), &filepath);
			FixZFighting();
			break;
		}
	}
}

void LevelManager::FixZFighting(){
	if(Ogre::MaterialManager::getSingleton().getByName("Ogre/TextureShadowCaster").isNull())
		// Render a frame to get the shadow materials created
			Ogre::Root::getSingleton().renderOneFrame();

	   // Get all shadow materials
		std::vector<Ogre::MaterialPtr> tmpMaterials;
	   Ogre::TexturePtr tmpTexturePtr = m_scene_manager->getShadowTexture(0);
	   Ogre::String tmpMaterialName = tmpTexturePtr->getName() + "Mat" + m_scene_manager->getName();
	   tmpMaterials.push_back(Ogre::MaterialManager::getSingleton().getByName(tmpMaterialName));
	   tmpMaterials.push_back(Ogre::MaterialManager::getSingleton().getByName("Ogre/TextureShadowCaster"));
	   tmpMaterials.push_back(Ogre::MaterialManager::getSingleton().getByName("Ogre/TextureShadowReceiver"));

	   // Loop through the list of shadow materials
	   unsigned int i = 0;
	   for( ; i < tmpMaterials.size(); i++ ) {
		// Check if the current shadow material exists
		if( !tmpMaterials[i].isNull() )
		 // Set the depth bias of the shadow material
		 tmpMaterials[i]->getTechnique(0)->getPass(0)->setDepthBias(5.0f);
	   }
	   // Clear the temporary list of shadow materials
	   tmpMaterials.clear();
}

void LevelManager::LoadPlane(){
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_physics_engine->ShowDebugDraw(true);
	PlayerDef player_def;
	CharacterControllerDef char_def;
	char_def.friction = 1.0f;
	char_def.velocity = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Speed");
	char_def.max_speed = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Max_Speed");
	char_def.deceleration = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Deceleration");
	char_def.jump_power = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Jump_Power");
	char_def.restitution = 0.0f;
	char_def.step_height = 0.15f;
	char_def.turn_speed = 800.0f;
	char_def.max_jump_height = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Max_Jump_Height");
	char_def.offset.y = 0.5f;
	char_def.radius = 0.3f;
	char_def.height = 0.4f;
	char_def.mass = 1.0f;
	char_def.max_fall_speed = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Max_Fall_Speed");
	char_def.fall_acceleration = VariableManager::GetSingletonPtr()->GetAsFloat("Player_Fall_Acceleration");
	char_def.collision_filter.filter = COL_PLAYER;
	char_def.collision_filter.mask = COL_BUBBLE | COL_BUBBLE_TRIG | COL_TOTT | COL_WORLD_STATIC | COL_WORLD_TRIGGER;
	player_def.character_contr = &char_def;
	player_def.level_id = "try";
	player_def.camera_speed = 2.5f;
	GameObject* temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(0,0,0), &player_def);
	//m_game_object_manager->CreateGameObject(GAME_OBJECT_CAMERA, Ogre::Vector3(0,0,0), m_game_object_manager->GetGameObject("Player"));
	PlaneDef plane_def;
	plane_def.material_name = "Examples/BeachStones";
	plane_def.plane_name = "plane";
	plane_def.friction = 1.0f;
	plane_def.restitution = 0.8f;
	plane_def.collision_filter.filter = COL_WORLD_STATIC;
	plane_def.collision_filter.mask = COL_BUBBLE | COL_PLAYER | COL_TOTT;
	m_game_object_manager->CreateGameObject(GAME_OBJECT_PLANE, Ogre::Vector3(0, -1, 0), &plane_def);
}