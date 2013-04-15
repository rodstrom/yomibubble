#ifndef _N_PHYSICS_COMPONENTS_H_
#define _N_PHYSICS_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "BulletDynamics\Character\btKinematicCharacterController.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class PhysicsEngine;
class RigidbodyComponent : public Component, public IComponentObserver{
public:
	RigidbodyComponent(void) : m_rigidbody(NULL), m_shape(NULL), m_motion_state(NULL), m_collision_object(NULL){ m_type = COMPONENT_RIGIDBODY; }
	virtual ~RigidbodyComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, PhysicsEngine* physics_engine, float mass, int collider_type, int body_type = DYNAMIC_BODY);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	btRigidBody* GetRigidbody() { return m_rigidbody; }

protected:
	btRigidBody*			m_rigidbody;
	btCollisionShape*		m_shape;
	btCollisionObject*		m_collision_object;
	btMotionState*			m_motion_state;
	PhysicsEngine*			m_physics_engine;
};

class CharacterController : public Component, public IComponentObserver, public IComponentUpdateable, public IComponentLateUpdate{
public:
	CharacterController(void) : m_controller(NULL), m_ghost_object(NULL), m_shape(NULL), m_velocity(0.0), m_turn_speed(0.0f), 
		m_move_backwards(false), m_move_forward(false), m_move_left(false), m_move_right(false), m_has_follow_cam(false) 
	{ m_type = COMPONENT_CHARACTER_CONTROLLER; }
	virtual ~CharacterController(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, int collider_type, PhysicsEngine* physics_engine);
	virtual void Update(float dt);
	virtual void LateUpdate(float dt);
	void SetVelocity(btScalar velocity) { m_velocity = velocity; }
	void SetTurnSpeed(float turn_speed) { m_turn_speed = turn_speed; }
	void HasFollowCam(bool value) { m_has_follow_cam = value; }

protected:
	btKinematicCharacterController*		m_controller;
	btPairCachingGhostObject*			m_ghost_object;
	PhysicsEngine*						m_physics_engine;
	btConvexShape*						m_shape;
	btScalar							m_velocity;
	float								m_turn_speed;
	bool								m_move_forward;
	bool								m_move_backwards;
	bool								m_move_left;
	bool								m_move_right;
	bool								m_has_follow_cam;
};

#endif // _N_PHYSICS_COMPONENTS_H_