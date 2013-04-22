#ifndef _N_PHYSICS_ENGINE_H_
#define _N_PHYSICS_ENGINE_H_

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
//#include "ETTerrainManager.h"
//#include "ETTerrainInfo.h"

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	bool Init();
	void Shut();

	void Step(float dt);
	void SetDebugDraw(Ogre::SceneManager* scene_manager);
	void ShowDebugDraw(bool value);
	void CloseDebugDraw();

	//void CreateTerrainCollision(ET::TerrainInfo* terrain_info);
	void CreateTerrainCollision(Ogre::Terrain* mTerrain);
	void DestroyTerrainCollision();

	//btBroadphaseInterface* GetBroadphaseInterface() const { return m_broadphase; }
	btDiscreteDynamicsWorld* GetDynamicWorld() const { return m_dynamic_world; }

private:
	btBroadphaseInterface*					m_broadphase;
	btDefaultCollisionConfiguration*		m_collision_configuration;
	btCollisionDispatcher*					m_collision_dispatcher;
	btSequentialImpulseConstraintSolver*	m_seq_impulse_con_solver;
	btDiscreteDynamicsWorld*				m_dynamic_world;
	BtOgre::DebugDrawer*					m_debug_drawer;
	btGhostPairCallback*					m_ghost_pair_callback;

	//Terrain Collision
	bool									m_has_terrain_coll;
	btHeightfieldTerrainShape*				m_terrain_shape;
	btRigidBody*							m_terrain_body;
	btDefaultMotionState*					m_terrain_motion_state;
	btCollisionObject*						m_terrain_coll_obj;
};

#endif // _N_PHYSICS_ENGINE_H_