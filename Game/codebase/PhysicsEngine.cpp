#include "stdafx.h"
#include "PhysicsEngine.h"
#include "Components\GameObject.h"
#include "Components\GameObjectPrereq.h"
#include "Managers\CollisionManager.h"

PhysicsEngine::PhysicsEngine(void) : 
	m_broadphase(NULL), 
	m_collision_configuration(NULL), 
	m_collision_dispatcher(NULL), 
	m_dynamic_world(NULL), 
	m_seq_impulse_con_solver(NULL),
	m_debug_drawer(NULL),
	m_ghost_pair_callback(NULL),
	m_has_terrain_coll(false),
	m_collision_manager(NULL){}

PhysicsEngine::~PhysicsEngine(void){}

bool PhysicsEngine::Init(){
	m_broadphase = new btDbvtBroadphase;
	m_ghost_pair_callback = new btGhostPairCallback;
	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghost_pair_callback);
	m_collision_configuration = new btDefaultCollisionConfiguration;
	m_collision_dispatcher = new btCollisionDispatcher(m_collision_configuration);
	m_seq_impulse_con_solver = new btSequentialImpulseConstraintSolver;
	m_dynamic_world = new btDiscreteDynamicsWorld(m_collision_dispatcher, m_broadphase, m_seq_impulse_con_solver, m_collision_configuration);
	m_dynamic_world->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	m_collision_manager = new CollisionManager(this);
	m_collision_manager->Init();
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
	if (m_collision_manager){
		m_collision_manager->Shut();
		delete m_collision_manager;
		m_collision_manager = NULL;
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
		m_dynamic_world->debugDrawWorld();
	}
}

void PhysicsEngine::Step(float dt){
	float fixed_time_step = 1.0f/60.0f;
	float physics_time = dt / 1000.0f;
	int max_steps = physics_time / (fixed_time_step) + 1;

	m_dynamic_world->stepSimulation(dt, max_steps, fixed_time_step);
	if (m_debug_drawer){
		m_debug_drawer->step();
	}
	int num_manifolds = m_dynamic_world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < num_manifolds; i++){
		btPersistentManifold* contact_manifold = m_dynamic_world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obj_a = static_cast<const btCollisionObject*>(contact_manifold->getBody0());
		const btCollisionObject* obj_b = static_cast<const btCollisionObject*>(contact_manifold->getBody1());
		int num_contacts = contact_manifold->getNumContacts();
		for (int j = 0; j < num_contacts; j++){
			btManifoldPoint& pt = contact_manifold->getContactPoint(j);
			if (pt.getDistance() < 0.0f){
				const btVector3& pt_a = pt.getPositionWorldOnA();
				const btVector3& pt_b = pt.getPositionWorldOnB();
				const btVector3& normal_on_b = pt.m_normalWorldOnB;
				m_collision_manager->ProcessCollision(obj_a, obj_b);
			}
		}
	}
}
/*
void PhysicsEngine::CreateTerrainCollision(const ET::TerrainInfo& terrain_info){
	if (!m_has_terrain_coll){
		size_t height_stick_width = terrain_info.getWidth();
		size_t height_stick_length = terrain_info.getWidth();
		std::vector<float> terrain_data = terrain_info.getHeightmapData();
		float max_height = terrain_info.getHeight();


		m_terrain_shape = new btHeightfieldTerrainShape(height_stick_width, height_stick_length, &terrain_data, 1, 0.0f, max_height, 1, PHY_FLOAT, true);
		m_terrain_shape->setUseDiamondSubdivision(true);
		m_terrain_motion_state = new btDefaultMotionState;
		m_terrain_body = new btRigidBody(0, m_terrain_motion_state, m_terrain_shape);
		m_dynamic_world->addRigidBody(m_terrain_body);
		m_terrain_coll_obj = new btCollisionObject;
		m_terrain_coll_obj->setCollisionShape(m_terrain_shape);
		m_dynamic_world->addCollisionObject(m_terrain_coll_obj);
		m_has_terrain_coll = true;
	}

}
*/

void PhysicsEngine::DestroyTerrainCollision(){

}