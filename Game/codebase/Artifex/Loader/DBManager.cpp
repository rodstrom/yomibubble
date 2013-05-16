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
#include <map>

DBManager::DBManager(ArtifexLoader *artifexloader, PhysicsEngine *physics_engine, GameObjectManager *game_object_manager, SoundManager *sound_manager) {

	m_physics_engine = physics_engine;
	mArtifexLoader = artifexloader;	
	mDB = new CppSQLite3DB();
	saving = false;
	m_game_object_manager = game_object_manager;
	m_sound_manager = sound_manager;

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
	
	//******** get objects from sqlite file *********
	string query = "SELECT * FROM objects";
	
	CppSQLite3Table t;
	
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
							player_def.level_id = mArtifexLoader->mZoneName;
							player_def.camera_speed = 2.5f;
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_PLAYER, Ogre::Vector3(x,y,z), &player_def);
							m_game_object_manager->CreateGameObject(GAME_OBJECT_CAMERA, Ogre::Vector3(x,y,z), m_game_object_manager->GetGameObject("Player"));
						}
						else if (i->second == "tott") {
							CharacterControllerDef tott_def;
							tott_def.friction = 1.0f;
							tott_def.velocity = 500.0f;
							tott_def.jump_power = 200.0f;
							tott_def.restitution = 0.0f;
							tott_def.step_height = 0.35f;
							tott_def.turn_speed = 1000.0f;
							tott_def.max_jump_height = 10.0f;
							tott_def.collision_filter.filter = COL_TOTT;
							tott_def.collision_filter.mask = COL_PLAYER | COL_WORLD_STATIC | COL_BUBBLE | COL_TOTT;
							//temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_TOTT, Ogre::Vector3(x,y,z), &tott_def);
						}
						else if (i->second == "leaf") {
							ParticleDef particleDef;
							particleDef.particle_name = "Particle/Smoke";
							m_game_object_manager->CreateGameObject(GAME_OBJECT_LEAF, Ogre::Vector3(x,y,z), &particleDef);
						}
						else if (i->second == "gate"){
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_GATE, Ogre::Vector3(x,y,z), NULL);
							Ogre::Quaternion quat = Ogre::Quaternion ((Degree(spawn.rx)), Vector3::UNIT_X)*Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y)*Quaternion ((Degree(spawn.rz)), Vector3::UNIT_Z);
							temp->GetComponentMessenger()->Notify(MSG_SET_OBJECT_ORIENTATION, &quat);
						}
						else if (i->second == "particle"){
							ParticleDef particleDef;
							particleDef.particle_name = "Particle/Smoke";
							temp = m_game_object_manager->CreateGameObject(GAME_OBJECT_PARTICLE, Ogre::Vector3(x,y,z), &particleDef);
						}
						
						interactive = true;
					}
				}

				for ( attributemap::iterator i = spawn.attributes.begin(); i != spawn.attributes.end(); i++ )
				{
					if (i->first == "sound") {
						SoundData3D m_3D_music_data;
						m_3D_music_data = m_sound_manager->Create3DData(i->second, 
							static_cast<NodeComponent*>(temp->GetComponent(EComponentType::COMPONENT_NODE))->GetSceneNode()->getName(), 
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
				}

				for ( attributemap::iterator i = spawn.attributes.begin(); i != spawn.attributes.end(); i++ )
				{
					if (i->first == "follow") { 
						//followers[temp] = i->second;
					}
				}
			}

			if(!interactive) {
				//****************************		
				
				//***************************
				Entity *newModel = NULL;
				lCreateEntity:
				try {
					newModel = mArtifexLoader->mSceneMgr->createEntity((string) entName, (string) meshFile);
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
						mArtifexLoader->mObjectFile.push_back(spawn);
						bool collision = true;
						mNode = mArtifexLoader->mSceneMgr->getRootSceneNode()->createChildSceneNode(entName+"Node",Ogre::Vector3(spawn.x,spawn.y,spawn.z),Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y));
						mNode->attachObject(newModel);
						mNode->setScale(spawn.sx,spawn.sy,spawn.sz);
						Ogre::Quaternion quat = Ogre::Quaternion ((Degree(spawn.rx)), Vector3::UNIT_X)*Quaternion ((Degree(spawn.ry)), Vector3::UNIT_Y)*Quaternion ((Degree(spawn.rz)), Vector3::UNIT_Z);
						mNode->setOrientation(quat);
						
						Ogre::String nocoll = meshFile.substr(0,6);
						if (nocoll == "nocoll"){
							collision = false;
						}
						// Create collision shape and set position if desired
						if (collision){
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
		static_cast<WayPointComponent*>(goIter->first->GetComponent(EComponentType::COMPONENT_AI))->AddWayPoint(followables[goIter->second]);
	}
	followers.clear();

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
}
