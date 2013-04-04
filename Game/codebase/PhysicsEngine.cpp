#include "PhysicsEngine.h"


PhysicsEngine::PhysicsEngine(void) : 
	m_broadphase(NULL), 
	m_collision_configuration(NULL), 
	m_collision_dispatcher(NULL), 
	m_dynamic_world(NULL), 
	m_seq_impulse_con_solver(NULL),
	m_debug_drawer(NULL){}

PhysicsEngine::~PhysicsEngine(void){}

bool PhysicsEngine::Init(){
	m_broadphase = new btDbvtBroadphase;
	m_collision_configuration = new btDefaultCollisionConfiguration;
	m_collision_dispatcher = new btCollisionDispatcher(m_collision_configuration);
	m_seq_impulse_con_solver = new btSequentialImpulseConstraintSolver;
	m_dynamic_world = new btDiscreteDynamicsWorld(m_collision_dispatcher, m_broadphase, m_seq_impulse_con_solver, m_collision_configuration);
	m_dynamic_world->setGravity(btVector3(0.0f, -10.0f, 0.0f));
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
	if (m_collision_dispatcher){
		delete m_collision_dispatcher;
		m_collision_dispatcher = NULL;
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

void PhysicsEngine::AddRigidBody(btRigidBody* rigidbody){
	m_dynamic_world->addRigidBody(rigidbody);
}

void PhysicsEngine::RemoveRigidBody(btRigidBody* rigidbody){
	m_dynamic_world->removeRigidBody(rigidbody);
}

void PhysicsEngine::ShowDebugDraw(bool value){
	if (m_debug_drawer){
		m_debug_drawer->setDebugMode(value);
	}
}

void PhysicsEngine::Step(float timestep, int substeps){
	m_dynamic_world->stepSimulation(timestep, substeps);
	if (m_debug_drawer){
		m_dynamic_world->debugDrawWorld();
		m_debug_drawer->step();
	}
}