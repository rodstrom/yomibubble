#ifndef _N_PHYSICS_ENGINE_H_
#define _N_PHYSICS_ENGINE_H_

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "OGRE\OgreVector3.h"
class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	bool Init();
	void Shut();

	void Step(float dt, int time);
	void SetDebugDraw(Ogre::SceneManager* scene_manager);
	void ShowDebugDraw(bool value);
	void CloseDebugDraw();
	void AddRigidBody(btRigidBody* rigidbody);
	void RemoveRigidBody(btRigidBody* rigidbody);

	btBroadphaseInterface* GetBroadphaseInterface() const { return m_broadphase; }
	btDiscreteDynamicsWorld* GetDynamicWorld() const { return m_dynamic_world; }

private:
	btBroadphaseInterface*					m_broadphase;
	btDefaultCollisionConfiguration*		m_collision_configuration;
	btCollisionDispatcher*					m_collision_dispatcher;
	btSequentialImpulseConstraintSolver*	m_seq_impulse_con_solver;
	btDiscreteDynamicsWorld*				m_dynamic_world;
	BtOgre::DebugDrawer*					m_debug_drawer;
	btGhostPairCallback*					m_ghost_pair_callback;
};

#endif // _N_PHYSICS_ENGINE_H_