#ifndef _N_PHYSICS_COMPONENTS_H_
#define _N_PHYSICS_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "GameObjectPrereq.h"
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
	CollisionDef			m_collision_def;
};

class TriggerComponent : public RigidbodyComponent{
public:
	TriggerComponent(void){}
	virtual ~TriggerComponent(void){}

	virtual void Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, struct TriggerDef* def);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:

};

class CharacterController : public RigidbodyComponent, public IComponentUpdateable, public IComponentSimulationStep{
public:
	CharacterController(void) : m_velocity(0.0), m_turn_speed(0.0f), 
		m_has_follow_cam(false), m_is_jumping(false), m_on_ground(true), m_start_y_pos(0.0f),
		m_max_jump_height(0.0f), m_direction(btVector3(0,0,0)), m_deacc(0.0f), m_max_speed(0.0f), m_acc_x(0.0f), m_acc_z(0.0f)
	{ m_type = COMPONENT_CHARACTER_CONTROLLER; m_update = true; }
	virtual ~CharacterController(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, PhysicsEngine* physics_engine);
	virtual void Update(float dt);
	virtual void SimulationStep(btScalar time_step);
	void SetVelocity(float velocity) { m_velocity = velocity; }
	void SetTurnSpeed(float turn_speed) { m_turn_speed = turn_speed; }
	void HasFollowCam(bool value) { m_has_follow_cam = value; }
	void SetMaxJumpHeight(float value) { m_max_jump_height = value; }
	void SetJumpPower(float value) { m_jump_pwr = value; }
	void SetMaxSpeed(float value) { m_max_speed = value; }
	void SetDeacceleration(float value) { m_deacc = value; }
	void SetRaycastLength(float value) { m_ray_length = value; }

protected:
	void ApplyAcceleration(Ogre::Vector3& dir, float dt);

	Ogre::Vector3	m_direction;

	float		m_max_speed;
	float		m_velocity;
	float		m_deacc;
	float		m_turn_speed;
	float		m_max_jump_height;
	float		m_jump_pwr;
	float		m_start_y_pos;
	float		m_acc_x;
	float		m_acc_z;
	float		m_ray_length;
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
	virtual void Init(PhysicsEngine* physics_engine, btRigidBody* body_a, const btVector3& pivot_a);
	btPoint2PointConstraint* GetConstraint() const { return m_constraint; }

private:
	btPoint2PointConstraint* m_constraint;
	PhysicsEngine* m_physics_engine;
};

class Generic6DofConstraintComponent : public Component, public IComponentObserver{
public:
	Generic6DofConstraintComponent(void) : m_physics_engine(NULL), m_constraint(NULL){}
	virtual ~Generic6DofConstraintComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b, bool linear_reference);
	btGeneric6DofConstraint* GetConstraint() const { return m_constraint; }

private:
	btGeneric6DofConstraint* m_constraint;
	PhysicsEngine* m_physics_engine;
};

class HingeConstraintComponent : public Component, public IComponentObserver{
public:
	HingeConstraintComponent(void){}
	virtual ~HingeConstraintComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b, const btVector3& axis_a, const btVector3& axis_b);
	btHingeConstraint* GetConstraint() const { return m_constraint; }

protected:
	btHingeConstraint* m_constraint;
	PhysicsEngine* m_physics_engine;
};

class RaycastComponent : public Component, public IComponentObserver{
public:
	RaycastComponent(void){}
	virtual ~RaycastComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, btCollisionObject* obj, const Ogre::String& body_id = Ogre::StringUtil::BLANK);
	void SetLength(const Ogre::Vector3& length);
	bool IsAttached() const { return m_attached; }
	void SetAttached(bool value) { m_attached = value; }
	RaycastDef& GetRaycastDef();

protected:
	bool m_attached;
	RaycastDef m_raycast_def;
	PhysicsEngine* m_physics_engine;
};

class DynamicCharacterController : public Component, public IComponentObserver, public IComponentUpdateable {
public:
	DynamicCharacterController(void) : m_on_ground(false), m_hitting_wall(false), m_bottom_y_offset(0.0f), m_bottom_rounded_region_y_offset(0.0f),
		m_step_height(0.0f), m_jump_recharge_time(0.0f), m_jump_recharge_timer(0.0f), m_deceleration(0.0f), m_max_speed(0.0f), m_jump_impulse(0.0f),
		m_manual_velocity(Ogre::Vector3::ZERO), m_collision_shape(NULL), m_motion_state(NULL), m_body(NULL), m_ghost_object(NULL), 
		m_previous_position(Ogre::Vector3::ZERO){}
	virtual ~DynamicCharacterController(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, const DynamicCharacterControllerDef& def);
	void Update(float dt);
	void Jump();

	void Move(const Ogre::Vector3& direction);
	void Move(const Ogre::Vector2& direction);

	bool IsOnGround() const;

	btRigidBody* GetRigidbody() const { return m_body; }
	btPairCachingGhostObject* GetGhostObject() const { return m_ghost_object; }

protected:
	btCollisionShape*			m_collision_shape;
	btMotionState*				m_motion_state;
	btRigidBody*				m_body;
	btPairCachingGhostObject*	m_ghost_object;
	CollisionDef				m_collision_def;
	PhysicsEngine*				m_physics_engine;

	btTransform m_motion_transform;

	Ogre::Vector3 m_manual_velocity;
	Ogre::Vector3 m_previous_position;
	std::vector<Ogre::Vector3> m_surface_hit_normals;

	bool m_on_ground;
	bool m_hitting_wall;
	float m_bottom_y_offset;
	float m_bottom_rounded_region_y_offset;
	float m_step_height;
	float m_jump_recharge_time;
	float m_jump_recharge_timer;
	float m_deceleration;
	float m_max_speed;
	float m_jump_impulse;

	void ParseGhostContacts();
	void UpdatePosition(float dt);
	void UpdateVelocity(float dt);
	Ogre::Vector3 Project(const Ogre::Vector3& lhs, const Ogre::Vector3& rhs) const;
};

#endif // _N_PHYSICS_COMPONENTS_H_