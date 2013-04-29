#include "stdafx.h"
#include "PhysicsEngine.h"
#include "Components\GameObject.h"
#include "Components\GameObjectPrereq.h"
#include "Components\PhysicsComponents.h"
#include "Managers\CollisionManager.h"

void GameTickCallback(btDynamicsWorld* world, btScalar time_step){
	PhysicsEngine* pe = static_cast<PhysicsEngine*>(world->getWorldUserInfo());
	pe->ProcessSimulationTick(time_step);
}

PhysicsEngine::PhysicsEngine(void) : 
	m_broadphase(NULL), 
	m_collision_configuration(NULL), 
	m_collision_dispatcher(NULL), 
	m_dynamic_world(NULL), 
	m_seq_impulse_con_solver(NULL),
	m_debug_drawer(NULL),
	m_ghost_pair_callback(NULL),
	m_has_terrain_coll(false){}

PhysicsEngine::~PhysicsEngine(void){}

bool PhysicsEngine::Init(){
	m_broadphase = new btAxisSweep3(btVector3(-10000, -10000, -10000), btVector3(10000,10000,10000),1024);
	m_ghost_pair_callback = new btGhostPairCallback;
	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghost_pair_callback);
	m_collision_configuration = new btDefaultCollisionConfiguration;
	m_collision_dispatcher = new btCollisionDispatcher(m_collision_configuration);
	m_seq_impulse_con_solver = new btSequentialImpulseConstraintSolver;
	m_dynamic_world = new btDiscreteDynamicsWorld(m_collision_dispatcher, m_broadphase, m_seq_impulse_con_solver, m_collision_configuration);
	m_dynamic_world->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	m_dynamic_world->setInternalTickCallback(GameTickCallback, static_cast<void*>(this), true);
	return true;
}

void PhysicsEngine::Shut(){
	if (m_dynamic_world){
		delete m_dynamic_world;
		m_dynamic_world = NULL;
	}
	if (m_seq_impulse_con_solver){
		delete m_seq_impulse_con_solver;
		m_seq_impulse_con_solver = NULL;
	}
	if (m_collision_configuration){
		delete m_collision_configuration;
		m_collision_configuration = NULL;
	}
	if (m_collision_dispatcher){
		delete m_collision_dispatcher;
		m_collision_dispatcher = NULL;
	}
	if (m_ghost_pair_callback){
		delete m_ghost_pair_callback;
		m_ghost_pair_callback = NULL;
	}
	if (m_broadphase){
		delete m_broadphase;
		m_broadphase = NULL;
	}
}

void PhysicsEngine::SetDebugDraw(Ogre::SceneManager* scene_manager){
	if (!m_debug_drawer){
		m_debug_drawer = new BtOgre::DebugDrawer(scene_manager->getRootSceneNode(), m_dynamic_world);
		m_dynamic_world->setDebugDrawer(m_debug_drawer);
	}
}

void PhysicsEngine::CloseDebugDraw(){
	if (m_debug_drawer){
		m_dynamic_world->setDebugDrawer(NULL);
		delete m_debug_drawer;
		m_debug_drawer = NULL;
	}
}

void PhysicsEngine::ShowDebugDraw(bool value){
	if (m_debug_drawer){
		m_debug_drawer->setDebugMode(value);
	}
}

void PhysicsEngine::Step(float dt){
	
	float fixed_time_step = 1.0f/60.0f;
	float physics_time = dt / 1000.0f;

	int max_steps = 2;
	while (fixed_time_step > (float)max_steps * fixed_time_step){
		max_steps++;
	}
	
	m_dynamic_world->stepSimulation(dt, max_steps);
	
	if (m_debug_drawer){
		m_debug_drawer->step();
	}
	
	RaycastQuery();
}

void PhysicsEngine::CreateTerrainCollision(Ogre::Terrain* terrain){
	if (!m_has_terrain_coll){
		size_t w = terrain->getSize();
		float* terrain_height_data = terrain->getHeightData();
		float world_size = terrain->getWorldSize();
		Ogre::Vector3 pos = terrain->getPosition();
		float max_height = terrain->getMaxHeight();
		float min_height = terrain->getMinHeight();

		float* data_converter = new float[terrain->getSize() * terrain->getSize()];
		for (int i = 0; i < terrain->getSize(); i++){
			memcpy(
				data_converter + terrain->getSize() * i,
				terrain_height_data + terrain->getSize() * (terrain->getSize()-i-1),
				sizeof(float)*(terrain->getSize())
				);
		}

		m_terrain_shape = new btHeightfieldTerrainShape(terrain->getSize(), terrain->getSize(), data_converter, 1, terrain->getMinHeight(), terrain->getMaxHeight(), 1, PHY_FLOAT, true);
		
		float units_between_vertices = terrain->getWorldSize() / (w - 1);
		btVector3 local_scaling(units_between_vertices, 1, units_between_vertices);
		m_terrain_shape->setLocalScaling(local_scaling);
		m_terrain_shape->setUseDiamondSubdivision(true);

		m_terrain_motion_state = new btDefaultMotionState;
		m_terrain_body = new btRigidBody(0, m_terrain_motion_state, m_terrain_shape);
		m_terrain_body->getWorldTransform().setOrigin(
			btVector3(
			pos.x,
			pos.y + (terrain->getMaxHeight() - terrain->getMinHeight()) / 2,
			pos.z
			)
			);

		m_terrain_body->getWorldTransform().setRotation(
			btQuaternion(Ogre::Quaternion::IDENTITY.x, Ogre::Quaternion::IDENTITY.y, Ogre::Quaternion::IDENTITY.z, Ogre::Quaternion::IDENTITY.w)
			);

		m_terrain_body->setCollisionFlags(m_terrain_body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		m_dynamic_world->addRigidBody(m_terrain_body);
		m_has_terrain_coll = true;
	}
}


void PhysicsEngine::DestroyTerrainCollision(){

}

void PhysicsEngine::RaycastQuery(){
	if (!m_raycast_components.empty()){
		for (unsigned int i = 0; i < m_raycast_components.size(); i++){
			RaycastDef& def = m_raycast_components[i]->GetRaycastDef();
			btVector3 from = def.origin;
			btVector3 to = def.origin + def.length;
			if (m_debug_drawer){
				m_debug_drawer->drawLine(from, to, btVector4(0,0,0,1));
			}
			btCollisionWorld::AllHitsRayResultCallback re(from,to);
			m_dynamic_world->rayTest(from, to, re);
			for (unsigned int i = 0; i < re.m_collisionObjects.size(); i++){
				CollisionManager::GetSingletonPtr()->ProcessRaycast(def.collision_object, re.m_collisionObjects[i]);
			}
		}
	}
}

void PhysicsEngine::RemoveRaycastComponent(RaycastComponent* comp){
	std::vector<RaycastComponent*>::iterator it = std::find(m_raycast_components.begin(), m_raycast_components.end(), comp);
	if (it != m_raycast_components.end()){
		m_raycast_components.erase(it);
	}
}

void PhysicsEngine::RemoveObjectSimulationStep(IComponentSimulationStep* ob){
	std::vector<IComponentSimulationStep*>::iterator it = std::find(m_ob_simulation_steps.begin(), m_ob_simulation_steps.end(), ob);
	if (it != m_ob_simulation_steps.end()){
		m_ob_simulation_steps.erase(it);
	}
}

void PhysicsEngine::ProcessSimulationTick(btScalar time_step){
	for (unsigned int i = 0; i < m_ob_simulation_steps.size(); i++){
		m_ob_simulation_steps[i]->SimulationStep(time_step);
	}
}