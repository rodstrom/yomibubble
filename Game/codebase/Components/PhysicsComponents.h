#ifndef _N_PHYSICS_COMPONENTS_H_
#define _N_PHYSICS_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "BulletDynamics\Character\btKinematicCharacterController.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class PhysicsEngine;
class RigidbodyComponent : public Component, public IComponentObserver{
public:
	RigidbodyComponent(void) : m_rigidbody(NULL), m_shape(NULL), m_motion_state(NULL){ m_type = COMPONENT_RIGIDBODY; }
	virtual ~RigidbodyComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, PhysicsEngine* physics_engine, float mass, int collider_type, int body_type = DYNAMIC_BODY);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	btRigidBody* GetRigidbody() { return m_rigidbody; }

protected:
	btRigidBody*			m_rigidbody;
	btCollisionShape*		m_shape;
	btMotionState*			m_motion_state;
	PhysicsEngine*			m_physics_engine;
};

class CharacterController : public RigidbodyComponent, public IComponentUpdateable{
public:
	CharacterController(void) : m_velocity(0.0), m_turn_speed(0.0f), 
		m_has_follow_cam(false), m_is_jumping(false), m_on_ground(false),
		m_max_jump_height(0.0f), m_direction(btVector3(0,0,0))
	{ m_type = COMPONENT_CHARACTER_CONTROLLER; }
	virtual ~CharacterController(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, PhysicsEngine* physics_engine);
	virtual void Update(float dt);
	void SetVelocity(float velocity) { m_velocity = velocity; }
	void SetTurnSpeed(float turn_speed) { m_turn_speed = turn_speed; }
	void HasFollowCam(bool value) { m_has_follow_cam = value; }
	void SetMaxJumpHeight(float value) { m_max_jump_height = value; }
	void SetJumpPower(float value) { m_jump_pwr = value; }


protected:
	void Move(const btVector3& dir, float movement_speed);

	btVector3	m_direction;
	float		m_max_speed;
	float		m_velocity;
	float		m_deacc;
	float		m_acceleration;
	float		m_turn_speed;
	float		m_max_jump_height;
	float		m_jump_pwr;
	float		m_last_y_pos;
	bool		m_has_follow_cam;
	bool		m_is_jumping;
	bool		m_on_ground;
	bool		m_is_moving;
};

class Point2PointConstraintComponent : public Component, public IComponentObserver{
public:
	Point2PointConstraintComponent(void) : m_constraint(NULL){ m_type = COMPONENT_POINT2POINT_CONSTRAINT; }
	virtual ~Point2PointConstraintComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b);
	btPoint2PointConstraint* GetConstraint() const { return m_constraint; }

private:
	btPoint2PointConstraint* m_constraint;
	PhysicsEngine* m_physics_engine;
};

class Generic6DofConstrantComponent : public Component, public IComponentObserver{
public:
	Generic6DofConstrantComponent(void) : m_physics_engine(NULL), m_contraint(NULL) {}
	virtual ~Generic6DofConstrantComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b);

private:
	btGeneric6DofConstraint* m_contraint;
	PhysicsEngine* m_physics_engine;
};

#endif // _N_PHYSICS_COMPONENTS_H_