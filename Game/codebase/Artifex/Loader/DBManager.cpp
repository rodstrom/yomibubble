#include "stdafx.h"
#include "DBManager.h"
#include "..\..\Managers\GameObjectManager.h"
#include "..\..\PhysicsEngine.h"
#include "..\..\Components\GameObjectPrereq.h"
#include "..\..\PhysicsPrereq.h"
#include "..\..\Components\GameObject.h"
#include "..\..\Components\VisualComponents.h"
#include "..\..\Managers\SoundManager.h"
#include "..\..\Components\AIComponents.h"
#include "..\..\Managers\VariableManager.h"
#include "TreeLoader3D.h"
#include "WindBatchPage.h"
#include "ImpostorPage.h"
#include "..\..\Managers\IniFile.h"
#include "..\..\AI\TottAIStates.h"
#include <map>

DBManager::DBManager(ArtifexLoader *artifexloader, PhysicsEngine *physics_engine, GameObjectManager *game_object_manager, SoundManager *sound_manager) {

	m_physics_engine = physics_engine;
	mArtifexLoader = artifexloader;	
	mDB = new CppSQLite3DB();
	saving = false;
	m_game_object_manager = game_object_manager;
	//m_game_object_manager->Init(
	m_sound_manager = sound_manager;
	m_paged_geometry = NULL;
};

DBManager::~DBManager() {
	Shut();
};
int DBManager::Open(const std::string path) {
	mDB->open(path.c_str());
	return 0;
};

int DBManager::Load() {
	string meshFile="";
	string entName="";
	mArtifexLoader->mObjectFile.clear();
	m_paged_geometry = new Forests::PagedGeometry;
	m_paged_geometry->setCamera(mArtifexLoader->mCamera);
	m_paged_geometry->setPageSize(60);
	m_paged_geometry->setInfinite();
	m_paged_geometry->addDetailLevel<Forests::WindBatchPage>(40,10);
	m_paged_geometry->addDetailLevel<Forests::ImpostorPage>(100, 10);

	Forests::TreeLoader3D* tree_loader = new Forests::TreeLoader3D(m_paged_geometry, Forests::TBounds(0,0,500,500));
	m_paged_geometry->setPageLoader(tree_loader);
	//******** get objects from sqlite file *********
	string query = "SELECT * FROM objects";
	
	IniFile wind_settings("../../resources/config/wind_settings.cfg");
	if (!wind_settings.parse()){
		System::Notify("Error opening wind_settings.cfg, this might break...", "File Open Error");
	}

	CppSQLite3Table t;
	
	std::vector<Ogre::String> shadow_map;
	this->InitShadowList(shadow_map);

	try {
		t = mDB->getTable(query.c_str());	
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return -1;
	};	
	//***********************************************	
	
	std::map<std::string, Ogre::SceneNode*> followables;
	//std::map<std::string, Ogre::SceneNode*> clans; //list all clans, if any from same clan is near, follow
	std::map<GameObject*, std::string> followers;

	for (int counter=0; counter<t.numRows(); counter ++)
	{		
		t.setRow(counter);

		{
			//***************************					
			entName = t.getStringField("entity");
			meshFile = t.getStringField("meshfile");
			//***************************

			int dtg = t.getIntField("drop_to_ground");
			float x = strToF(t.fieldValue("x"));
			float y = strToF(t.fieldValue("y")); 
			float z = strToF(t.fieldValue("z"));
			float sy = strToF(t.fieldValue("scale_y"));


			Spawn2 spawn;

			spawn.name = entName;
			spawn.meshfile = meshFile;
			spawn.x = x;
			spawn.y = y;
			spawn.z = z;
			spawn.ry = strToF(t.fieldValue("rot_y")); //r;
			spawn.rx = strToF(t.fieldValue("rot_x")); //rx;
			spawn.rz = strToF(t.fieldValue("rot_z")); //rz;			
			spawn.sx = strToF(t.fieldValue("scale_x")); //
			spawn.sy = sy;
			spawn.sz = strToF(t.fieldValue("scale_z"));
			spawn.drop_to_ground = dtg;
			spawn.render_static = t.getIntField("static");
			spawn.spawntype = t.getStringField("object_type");
			spawn.mask = t.getIntField("mask");
			
			bool success = getObjectProperties(spawn);
			
			if (spawn.spawntype.length() < 1) spawn.spawntype = "default";
			
			//mArtifexLoader->mObjectFile.push_back(spawn);
			
			bool interactive = false;
			if (spawn.attributes.size() > 0) {
				GameObject* temp;
				
				//make sure to check if interactive first and get nodeName etc.
				//attributemap::iterator i = spawn.attributes.begin();	
				for ( attributemap::iterator i = spawn.attributes.begin(); i != spawn.attributes.end(); i++ )
				{
					if (i->first == "interactive") {
						if (i->second == "player") {
							PlayerDef player_def;
							CharacterControllerDef char_def;
							char_def.friction = 0.5f;
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
							char_def.collision_filter.mask = COL_BUBBLE | COL_BUBBLE_TRIG | COL_TOTT | COL_WORLD_STATIC | COL_WORLD_TRIGGER | COL_QUESTITEM;
							char_def.mesh = meshFile;
							player_def.character_contr = &char_def;
							player_def.level_id = mArtifexLoader->mZoneName;
							player_def.camera_speed = 2.5f;
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(x,y,z), &player_def);
						}
						else if (i->second == "tott" || i->second == "hidehog") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.3f;
							tott_def.character_controller.velocity = 5.1f;
							tott_def.character_controller.max_speed = 4.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.25f;
							tott_def.character_controller.turn_speed = 500.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 2.0f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "Excited";
							tott_def.idle_animation = "Idle";
							tott_def.mesh_name = "Hidehog.mesh";
							tott_def.play_music = true;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "Respond";
							tott_def.run_animation = "Run";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "Hidehog_Theme";
							tott_def.node_name = "";
							tott_def.type_name = "Hidehog";
							tott_def.walk_animation = "walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							//m_game_object_manager->CreateGameObject(GAME_OBJECT_SPEECH_BUBBLE, Ogre::Vector3(x,y,z), &tott);
							interactive = true;
							continue;
						}

						else if (i->second == "questhidehog") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.3f;
							tott_def.character_controller.velocity = 5.1f;
							tott_def.character_controller.max_speed = 4.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.25f;
							tott_def.character_controller.turn_speed = 500.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 2.0f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "Excited";
							tott_def.idle_animation = "Idle";
							tott_def.mesh_name = "Hidehog.mesh";
							tott_def.play_music = true;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "Respond";
							tott_def.run_animation = "Run";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "Hidehog_Theme";
							tott_def.node_name = "";
							tott_def.type_name = "Hidehog";
							tott_def.walk_animation = "walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_QUEST_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							//m_game_object_manager->CreateGameObject(GAME_OBJECT_SPEECH_BUBBLE, Ogre::Vector3(x,y,z), &tott);
							interactive = true;
							continue;
						}
						else if (i->second == "kittyshroom") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.1f;
							tott_def.character_controller.velocity = 20.1f;
							tott_def.character_controller.max_speed = 10.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.35f;
							tott_def.character_controller.turn_speed = 100.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 0.5f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "excited";
							tott_def.idle_animation = "walk";
							tott_def.mesh_name = "kittyshroom.mesh";
							tott_def.play_music = false;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "react";
							tott_def.run_animation = "walk";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "";
							tott_def.type_name = "Kittyshroom";
							tott_def.walk_animation = "walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							interactive = true;
							continue;
						}
						else if (i->second == "questkittyshroom") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.1f;
							tott_def.character_controller.velocity = 20.1f;
							tott_def.character_controller.max_speed = 10.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.35f;
							tott_def.character_controller.turn_speed = 100.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 0.5f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "excited";
							tott_def.idle_animation = "walk";
							tott_def.mesh_name = "kittyshroom.mesh";
							tott_def.play_music = false;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "react";
							tott_def.run_animation = "walk";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "";
							tott_def.type_name = "Kittyshroom";
							tott_def.walk_animation = "walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							interactive = true;
							continue;
						}
						else if (i->second == "shroomfox") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.1f;
							tott_def.character_controller.velocity = 20.1f;
							tott_def.character_controller.max_speed = 10.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.35f;
							tott_def.character_controller.turn_speed = 100.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 0.5f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "Excited";
							tott_def.idle_animation = "Idle";
							tott_def.mesh_name = "Shroomfox.mesh";
							tott_def.play_music = false;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "Creep";
							tott_def.run_animation = "Walk";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "";
							tott_def.type_name = "Shroomfox";
							tott_def.walk_animation = "Walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							interactive = true;
							continue;
						}
						
						else if (i->second == "nightcap") {
							TottDef tott_def;
							tott_def.character_controller.friction = 0.1f;
							tott_def.character_controller.velocity = 20.1f;
							tott_def.character_controller.max_speed = 10.0f;
							tott_def.character_controller.jump_power = 200.0f;
							tott_def.character_controller.restitution = 0.0f;
							tott_def.character_controller.step_height = 0.35f;
							tott_def.character_controller.turn_speed = 100.0f;
							tott_def.character_controller.max_jump_height = 10.0f;
							tott_def.character_controller.mass = 1.0f;
							tott_def.character_controller.max_fall_speed = 0.1f;
							tott_def.character_controller.fall_acceleration = 0.1f;
							tott_def.character_controller.air_deceleration = 0.5f;
							tott_def.character_controller.deceleration = 0.5f;
							tott_def.character_controller.collision_filter.filter = COL_TOTT;
							tott_def.character_controller.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT | COL_QUESTITEM;
							tott_def.character_controller.radius = 0.5f;
							tott_def.character_controller.height = 0.1f;
							tott_def.character_controller.offset.y = 0.4f;
							tott_def.happy_animation = "excited";
							tott_def.idle_animation = "idle";
							tott_def.mesh_name = "nightcap.mesh";
							tott_def.play_music = false;
							tott_def.quest_object_mesh_name = "Questitem_Cherry.mesh";
							tott_def.react_animation = "idle2";
							tott_def.run_animation = "Run";
							tott_def.sb_node_name = "node_main";
							tott_def.sfx_curious = "";
							tott_def.sfx_happy = "";
							tott_def.theme_music = "";
							tott_def.type_name = "Nightcap";
							tott_def.walk_animation = "walk";
							attributemap::iterator j = i;
							j++;
							for (; j != spawn.attributes.end(); j++){
								this->CreateTottAIState(tott_def.ai_states, j->first, j->second);
							}
							GameObject* tott = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
							interactive = true;
							continue;
						}
						else if (i->second == "leaf") {
							ParticleDef particleDef;
							particleDef.particle_name = "Particle/Smoke";
							m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(x,y,z), &particleDef);
						}
						else if (i->second == "gate"){
							ConfigFile cfgfile;
							cfgfile.load(mArtifexLoader->mZonePath+mArtifexLoader->mZoneName+"/zonesettings.cfg");
							int leaves = strToI(cfgfile.getSetting("Leaves"));
							if (cfgfile.getSetting("Leaves") == "") {
								System::Notify("ERROR: Leaves for level " + mArtifexLoader->mZoneName + " has not been set, default Leaves set to 3.", "Level Load Error");
								leaves = 1;
							}
							GateDef gate_def;
							gate_def.leaves = leaves;
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_GATE, Ogre::Vector3(x,y,z), &gate_def);
							Ogre::Quaternion quat = Ogre::Quaternion ((Degree(spawn.rx)), Vector3::UNIT_X)*Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y)*Quaternion ((Degree(spawn.rz)), Vector3::UNIT_Z);
							temp->GetComponentMessenger()->Notify(MSG_SET_OBJECT_ORIENTATION, &quat);
							Ogre::SceneNode* gate_node = NULL;
							temp->GetComponentMessenger()->Notify(MSG_NODE_GET_NODE, &gate_node);
							if (gate_node){
								gate_node->setScale(spawn.sx, spawn.sy, spawn.sz);
							}
						}
						else if (i->second == "levelchange"){
							TriggerDef def;
							def.body_type = STATIC_BODY;
							def.collision_filter.filter = COL_WORLD_TRIGGER;
							def.collision_filter.mask = COL_PLAYER;
							def.mass = 1.0f;
							def.collider_type = COLLIDER_BOX;
							def.x = spawn.sx * 3.5f;	// "godtyckliga" values to scale the magic teleportation box into Bullet values
							def.y = spawn.sy * 3.5f;
							def.z = spawn.sz * 3.5f;
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_LEVEL_CHANGE, Ogre::Vector3(spawn.x, spawn.y, spawn.z), &def);
							Ogre::Quaternion quat = Ogre::Quaternion ((Degree(spawn.rx)), Vector3::UNIT_X)*Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y)*Quaternion ((Degree(spawn.rz)), Vector3::UNIT_Z);
							temp->GetComponentMessenger()->Notify(MSG_SET_OBJECT_ORIENTATION, &quat);
						}
						interactive = true;
					} //if interactive == true
				}

				for ( attributemap::iterator i = spawn.attributes.begin(); i != spawn.attributes.end(); i++ )
				{
					if (i->first == "sound") {
						SoundData3D m_3D_music_data;
						m_3D_music_data = m_sound_manager->Create3DData(i->second, 
							static_cast<NodeComponent*>(temp->GetComponent(COMPONENT_NODE))->GetSceneNode()->getName(), 
							false, false, false, 1.0f, 1.0f);
					} 
					else if (i->first == "waypoints") {
						//WayPointComponent* tempWP = static_cast<WayPointComponent*>(temp->GetComponent(EComponentType::COMPONENT_AI));
						//std::vector<std::string> waypoints = split(i->second, ',');
						//for(int j = 0; j < waypoints.size(); j++) tempWP->AddWayPoint(getWaypoint(waypoints.at(j)));
					}
					else if (i->first == "loopWaypoints") {
						//WayPointComponent* tempWP = static_cast<WayPointComponent*>(temp->GetComponent(EComponentType::COMPONENT_AI));
						//tempWP->SetLoopable(i->second);
					}
					else if (i->first == "waypoint") {		//dont render the waypoints
						//interactive = true;
					}
					else if (i->first == "followable") { 
						//followables[i->second] = static_cast<NodeComponent*>(temp->GetComponent(EComponentType::COMPONENT_NODE))->GetSceneNode();
					}
					else if (i->first == "particle"){
						static int particle_counter = 0;
						ParticleDef particleDef;
						particleDef.particle_name = i->second;
						particleDef.particle_id = "particle" + NumberToString(particle_counter);
						temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_PARTICLE, Ogre::Vector3(x,y,z), &particleDef);
						particle_counter++;
						interactive = true;
					}
				}

				for ( attributemap::iterator i = spawn.attributes.begin(); i != spawn.attributes.end(); i++ )
				{
					if (i->first == "follow") { 
						//followers[temp] = i->second;
					}
				}
			}

			if(!interactive) {
				bool collision = true;
				Ogre::String nocoll = meshFile.substr(0,6);
				if (nocoll == "nocoll"){
					collision = false;
				}
				//****************************		
				
				//***************************
				Entity *newModel = NULL;
				lCreateEntity:
				try {
					if (collision){
						newModel = mArtifexLoader->mSceneMgr->createEntity((string) entName, (string) meshFile);
						std::vector<Ogre::String>::iterator it = std::find(shadow_map.begin(), shadow_map.end(), meshFile);
						if (it != shadow_map.end()){
							newModel->setCastShadows(false);
						}
					}
					else {
						MeshList::iterator it = m_vegetation.find(meshFile);
						if (it == m_vegetation.end()){
							newModel = mArtifexLoader->mSceneMgr->createEntity((string) entName, (string) meshFile);
							//newModel = mArtifexLoader->mSceneMgr->createEntity((string) entName, "nocoll_TestMedDayGrass1.mesh");
							newModel->setCastShadows(false);
							m_vegetation[meshFile] = newModel;
							m_paged_geometry->setCustomParam(newModel->getName(), "windFactorX", wind_settings.getValueAsFloat(meshFile, "X", 0.4f));
							m_paged_geometry->setCustomParam(newModel->getName(), "windFactorY", wind_settings.getValueAsFloat(meshFile, "Y", 0.01f));
						}
						else {
							newModel = it->second;
						}
						tree_loader->addTree(newModel, Ogre::Vector3(spawn.x, spawn.y, spawn.z), Ogre::Degree(spawn.ry), spawn.sx);
					}
					
				} catch(Exception &e) {
					if (e.getNumber() != 4) {
						cout << "\n|= ArtifexTerra3D =| Zoneloader v1.0 RC1 OT beta: Error " << e.getNumber() << " creating Entity " << entName.c_str() << ": " << e.getFullDescription().c_str() << "\n";
					} else if (e.getNumber() == 4) {
						cout << "\n|= ArtifexTerra3D =| Zoneloader v1.0 RC1 OT beta: Entity with the name " << entName.c_str() << " already exists, creating a random name:";
						entName = "Entity"+StringConverter::toString((int)Math::RangeRandom(100000,999999));
						cout << entName.c_str() << ".\n";
						goto lCreateEntity;
					}
					continue;
				};					
				newModel->setQueryFlags(t.getIntField("mask"));
				//****************************
				
				// auto drop models on terrain
				if (y==0 || dtg==1) {		
					float tmpY = mArtifexLoader->getMeshYAdjust(meshFile)* sy;
					y = mArtifexLoader->getHeightAt(x,z)+tmpY;
				}

				{
					SceneNode *mNode = NULL;
					try {
					if (collision){
						mArtifexLoader->mObjectFile.push_back(spawn);

						mNode = mArtifexLoader->mSceneMgr->getRootSceneNode()->createChildSceneNode(entName+"Node",Ogre::Vector3(spawn.x,spawn.y,spawn.z),Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y));
						mNode->attachObject(newModel);
						mNode->setScale(spawn.sx,spawn.sy,spawn.sz);
						Ogre::Quaternion quat = Ogre::Quaternion ((Degree(spawn.rx)), Vector3::UNIT_X)*Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y)*Quaternion ((Degree(spawn.rz)), Vector3::UNIT_Z);
						mNode->setOrientation(quat);
						
						// Create collision shape and set position if desired
						//newModel->setRenderingDistance(80.0f);
						BtOgre::StaticMeshToShapeConverter converter(newModel);
						btCollisionShape* shape = converter.createTrimesh();
						m_shapes.push_back(shape);
						btMotionState* motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
						m_motion_states.push_back(motion_state);
						btRigidBody* body = new btRigidBody(0, motion_state, shape, btVector3(0,0,0));
						m_bodies.push_back(body);

						btTransform transform;
						transform.setIdentity();
						transform.setOrigin(btVector3(spawn.x, spawn.y, spawn.z));
						transform.setRotation(BtOgre::Convert::toBullet(quat));
						body->setWorldTransform(transform);
						CollisionDef* collision_def = new CollisionDef;
						collision_def->data = NULL;
						collision_def->flag = COLLISION_FLAG_STATIC;
						body->setUserPointer(collision_def);
						body->setRestitution(1.0f);
						body->setFriction(1.0f);
						m_collision_defs.push_back(collision_def);
						int mask = COL_TOTT | COL_BUBBLE | COL_PLAYER;
						m_physics_engine->GetDynamicWorld()->addRigidBody(body, COL_WORLD_STATIC, mask);
					}

				} catch (Exception &e) {
					cout << "\n|= ArtifexTerra3D =| Zoneloader v1.0 RC1 OT beta SQLite: spawnloader error - problems creating " << spawn.name.c_str() << ":" << e.what() << "\n";
				};
				}
			}
		}		
	} 			

	std::map<GameObject*, std::string>::iterator goIter;
	for (goIter = followers.begin(); goIter != followers.end(); goIter++) {
		static_cast<WayPointComponent*>(goIter->first->GetComponent(COMPONENT_AI))->AddWayPoint(followables[goIter->second]);
	}
	followers.clear();
	m_paged_geometry->preloadGeometry(Forests::TBounds(0,0,500,500));
	t.finalize();
	return 0;	
};

std::vector<std::string> &DBManager::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> DBManager::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Vector3 DBManager::getWaypoint(string waypoint_id) {
	string query = "SELECT object_name FROM attributes WHERE attribute='waypoint' AND value='"+waypoint_id+"'";
	CppSQLite3Table t;
	try {
		t = mDB->getTable(query.c_str());	
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		//return Vector3();
	};
	if (t.numRows()<1) {
		t.finalize();
		//return Vector3();
	}
	t.setRow(0);
	string waypointObject = t.getStringField("object_name");
	t.finalize();

	query  = "SELECT x, y, z FROM objects WHERE entity='"+waypointObject+"'";
	try {
		t = mDB->getTable(query.c_str());	
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		//return Vector3();
	};
	if (t.numRows()<1) {
		t.finalize();
		//return Vector3();
	}
	t.setRow(0);
	Vector3 result = Vector3(t.getFloatField("x"), t.getFloatField("y"), t.getFloatField("z"));
	t.finalize();
	return result;	
};

int DBManager::getObjectProperties(Spawn2 &spawn) {
	string query  = "SELECT object_name, attribute, value FROM attributes WHERE object_name='"+spawn.name+"'";
	
	CppSQLite3Table t;
	
	try {
		t = mDB->getTable(query.c_str());	
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return -1;
	};
	
	int n = t.numRows();
	
	if (n<1) {
		t.finalize();
		return 0;
	}
	for (int a=0; a<t.numRows(); a++) {
		t.setRow(a);
		spawn.attributes[t.getStringField("attribute")] = t.getStringField("value");	
	}	
	t.finalize();
	return n;	
};
int DBManager::Save() {
	saving = true;
	Ogre::String filename = "";	
	Ogre::String spawntype = "";

	std::vector <Spawn2> *tmpSpawns;

	EmptyTrash();

	tmpSpawns = &mArtifexLoader->mObjectFile; 
			
	for (unsigned int b=0;b<tmpSpawns->size();b++) {				
		WriteSpawn (tmpSpawns->at(b),spawntype);				
	}
				
	saving = false;
	return 0;
};
bool DBManager::EmptyTrash() {
	std::vector<Spawn2>::iterator tmpIter;
	for (tmpIter = mArtifexLoader->mObjectTrashCan.begin(); tmpIter != mArtifexLoader->mObjectTrashCan.end(); ++tmpIter) {
		DeleteSpawn(*tmpIter);		
	}
	mArtifexLoader->mObjectTrashCan.clear();
	return true;
};
bool DBManager::DeleteSpawn(const Spawn2& spawn, bool clear_attributes) {	
	if (clear_attributes) ClearAttributes(spawn);
	string query = "DELETE FROM objects WHERE entity='"+spawn.name+"'";
	return ExecSQL(query);
};
bool DBManager::ClearAttributes(const Spawn2& spawn) {
	string query = "DELETE FROM attributes WHERE object_name='"+spawn.name+"'";
	return ExecSQL(query);
}
bool DBManager::WriteSpawn(Spawn2 spawn, std::string spawntype) {
			
	Ogre::String sto = "";
	
	sto += "INSERT OR REPLACE INTO objects "; 

	sto +="(entity, meshfile, x, y, z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z, mask, static, drop_to_ground, object_type) VALUES ";
	sto += +"('";
	sto += spawn.name+"', '";
	sto += spawn.meshfile+"', ";
	sto += fToStr( spawn.x )+", ";
	sto += fToStr( spawn.y )+", ";
	sto += fToStr( spawn.z )+", ";
	sto += fToStr( spawn.rx )+", ";
	sto += fToStr( spawn.ry )+", ";
	sto += fToStr( spawn.rz )+", ";
	sto += fToStr( spawn.sx )+", ";
	sto += fToStr( spawn.sy )+", ";
	sto += fToStr( spawn.sz )+", ";
	
	try {
		Ogre::Entity *oe = mArtifexLoader->mSceneMgr->getEntity(spawn.name);
		if (oe!=NULL){
			sto += iToStr( oe->getQueryFlags() ) + ", ";
		} else sto += "0, ";
	} catch(Exception &e) {
		sto += "0, ";
	}
	
	if (spawntype == "static" || spawntype == "groundcover") sto += "1, ";
	else sto += "0, ";
	
	if (spawntype == "groundcover") sto += "1, ";
	else sto += "0, ";
	
	sto += "'"+spawntype+"'";				
	sto += ") ";	
		
	
	if(ExecSQL(sto)) {
		WriteAttributes(spawn);
	} else {		
		return false;
	}
	
	return true;	
};

bool DBManager::WriteAttributes(Spawn2& spawn) {
	string query1 = "INSERT INTO attributes ( object_name, attribute, value ) VALUES ( '";
	attributemap::iterator i = spawn.attributes.begin();	
	for( ; i != spawn.attributes.end(); ++i )
	{
		string query = query1 + spawn.name +"', '"+i->first+"', '"+i->second+"')";
		ExecSQL(query);
	}
	return true;
};

bool DBManager::ExecSQL (std::string query) {

	if (query.length() <= 0) {
		cerr << "ExecSQL: query missing!\n";
		return false;
	}

	try {
		mDB->execDML(query.c_str());
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return false;
	};
	return true;
};

bool DBManager::EmptyTable(std::string tablename) {

	if (tablename.length() <= 0) {
		cerr << "EmptyTable: tablename missing!\n";
		return false;
	}

	string query = "DELETE FROM "+tablename;
	try {					
			mDB->execDML(query.c_str());
	} catch (CppSQLite3Exception& e) {
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return false;
	};			
	return true;
};

int DBManager::Close(void){
	mDB->close();
	return 0;
};


void DBManager::Shut(){
	for (unsigned int i = 0; i < m_motion_states.size(); i++){ // will always have the same size as the other lists
		m_physics_engine->GetDynamicWorld()->removeRigidBody(m_bodies[i]);
		delete m_motion_states[i];
		delete m_shapes[i];
		delete m_bodies[i];
		delete m_collision_defs[i];
	}
	m_motion_states.clear();
	m_shapes.clear();
	m_bodies.clear();
	m_collision_defs.clear();
	m_vegetation.clear();
	delete m_paged_geometry->getPageLoader();
	delete m_paged_geometry;
	m_paged_geometry = NULL;
}

void DBManager::Update(){
	m_paged_geometry->update();	// Update LOD from camera position each frame
}

void DBManager::InitShadowList(std::vector<Ogre::String>& shadow_map){
	shadow_map.push_back("RockPAth.mesh");
	shadow_map.push_back("RockPAth2.mesh");
	shadow_map.push_back("Rock_asset1.mesh");
	shadow_map.push_back("Rock_asset2.mesh");
	shadow_map.push_back("RockPlatform_2.mesh");
	shadow_map.push_back("RockPlatform_Low.mesh");
	shadow_map.push_back("RockPlatform_Low2.mesh");
	shadow_map.push_back("Puzzle1.mesh");
	shadow_map.push_back("Puzzle2.mesh");
	shadow_map.push_back("Puzzle3.mesh");
	shadow_map.push_back("RockSlide.mesh");
}

void DBManager::CreateTottAIState(std::vector<AIState*>& vector, const Ogre::String& id, const Ogre::String& line){
	size_t find_type = id.find("move");
	if (find_type != std::string::npos){
		size_t find = line.find_first_of(",");
		size_t find2 = line.find(",", find + 1);
		Ogre::String anim = line.substr(0, find);
		float x = StringToNumber<float>(line.substr(find + 1, find2 - (find + 1)));
		float z = StringToNumber<float>(line.substr(find2 + 1, line.length() - find2));
		TottMoveAIDef def;
		def.animation = anim;
		def.target_position = Ogre::Vector2(x,z);
		vector.push_back(new TottAIStateMove(def));
		return;
	}
	find_type = id.find("wait");
	if (find_type != std::string::npos){
		size_t find = line.find_first_of(",");
		Ogre::String anim = line.substr(0, find);
		float time = StringToNumber<float>(line.substr(anim.length() + 1, line.length() - anim.length()));
		TottWaitAIDef def;
		def.animation = anim;
		def.target_time = time;
		vector.push_back(new TottAIStateWait(def));
	}
	/*

	size_t find = line.find_first_of(id);
	std::string x = line.substr(0, find);
	std::string y = line.substr(x.length() + 1, line.length() - x.length());

	if (id == "move"){

	}
	else if (id == "wait"){
		size_t find = line.find_first_of(",");
		size_t find2 = line.find(",", find + 1);
		Ogre::String anim = line.substr(0, find);
		size_t find_type = line.find("random");
		if (find_type != std::string::npos){
			size_t find3 = line.find(id, find2 + 1);

		}
		else {

		}
	}*/
}

/*
		size_t find = line.find_first_of(id);
		size_t find2 = line.find(id, find + 1);
		size_t find3 = line.find(id, find2 + 1);
		std::string x = line.substr(0, find);
		std::string y = line.substr(find + 1, find2 - (find + 1));
		std::string w = line.substr(find2 + 1, find3 - (find2 + 1));
		std::string h = line.substr(find3 + 1, line.length() - find3);
		*/