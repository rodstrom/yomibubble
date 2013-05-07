#include "stdafx.h"
#include "LevelManager.h"
#include "GameObjectManager.h"
#include "..\MessageSystem.h"

LevelManager::LevelManager(GameObjectManager* game_object_manager, Ogre::SceneManager* scene_manager,  MessageSystem* message_system) : 
	m_game_object_manager(game_object_manager), m_scene_manager(scene_manager), m_message_system(message_system), m_current_level(0){
		m_message_system->Register<LevelManager>(EEventType::EVT_CHANGE_LEVEL, this, &LevelManager::ChangeLevel);
}
LevelManager::~LevelManager(void){}


void LevelManager::ChangeLevel(IEvent*){
	Ogre::String next_level = m_levels[m_current_level].next_level;
	m_game_object_manager->ClearAllGameObjects();
	// set loading screen game object here
	LoadLevel(next_level);
}

void LevelManager::LoadLevel(const Ogre::String& level_id){
	for (unsigned int i = 0; i < m_levels.size(); i++){
		if (m_levels[i].id == level_id){
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