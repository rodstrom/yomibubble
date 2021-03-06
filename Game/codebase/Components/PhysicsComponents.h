#ifndef _N_PHYSICS_COMPONENTS_H_
#define _N_PHYSICS_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "GameObjectPrereq.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "..\Managers\SoundManager.h"

class PhysicsEngine;
class RigidbodyComponent : public Component, public IComponentObserver{
public:
	RigidbodyComponent(void) : m_rigidbody(NULL), m_shape(NULL), m_motion_state(NULL), m_compound_shape(NULL){ m_type = COMPONENT_RIGIDBODY; }
	virtual ~RigidbodyComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, PhysicsEngine* physics_engine, const RigidBodyDef& def);
	virtual void Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, const RigidBodyDef& def);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	btRigidBody* GetRigidbody() { return m_rigidbody; }
	CollisionDef& GetCollisionDef() { return m_collision_def; }

protected:
	btRigidBody*			m_rigidbody;
	btCompoundShape*		m_compound_shape;
	btCollisionShape*		m_shape;
	btMotionState*			m_motion_state;
	PhysicsEngine*			m_physics_engine;
	CollisionDef			m_collision_def;
};

class ShapeComponent : public Component, public IComponentObserver {
public:
	ShapeComponent(void){ m_type = COMPONENT_SHAPE; }
	virtual ~ShapeComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Init(const ShapeDef& def);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	btCollisionShape* GetShape() const { return m_shape; }

protected:
	btCompoundShape* m_compoound_shape;
	btCollisionShape* m_shape;
};

class TriggerComponent : public RigidbodyComponent{
public:
	TriggerComponent(void){ m_type = COMPONENT_TRIGGER; }
	virtual ~TriggerComponent(void){}

	virtual void Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, const TriggerDef& def);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:

};

class SyncedTriggerComponent : public TriggerComponent, public IComponentUpdateable{
public:
	SyncedTriggerComponent(void){ m_type = COMPONENT_TRIGGER; }
	virtual ~SyncedTriggerComponent(void){}

	virtual void Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, struct TriggerDef* def);
	virtual void Update(float dt);

protected:
	Ogre::Vector3 m_offset;
};


class CharacterController : public RigidbodyComponent, public IComponentUpdateable, public IComponentSimulationStep{
public:
	CharacterController(void) : m_velocity(0.0), m_turn_speed(0.0f), 
		m_has_follow_cam(false), m_is_jumping(false), m_on_ground(true), m_jump_timer(0.0f), m_y_bottom_offset(0.0f),
		m_max_jump_height(0.0f), m_direction(btVector3(0,0,0)), m_deceleration(0.0f), m_max_speed(0.0f), m_step_height(0.0f), m_current_friction(0.0f),
		m_actual_direction(Ogre::Vector3::ZERO), m_fall_acceleration(0.0f), m_max_fall_speed(0.0f), m_limit_max_speed(true), m_can_move(true)
	{ m_type = COMPONENT_CHARACTER_CONTROLLER; m_update = true; }
	virtual ~CharacterController(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::Vector3& position, PhysicsEngine* physics_engine, const CharacterControllerDef& def);
	virtual void Update(float dt);
	virtual void SimulationStep(btScalar time_step);
	void SetVelocity(float velocity) { m_velocity = velocity; }
	void SetTurnSpeed(float turn_speed) { m_turn_speed = turn_speed; }
	void HasFollowCam(bool value) { m_has_follow_cam = value; }
	void SetMaxJumpHeight(float value) { m_max_jump_height = value; }
	void SetJumpPower(float value) { m_jump_pwr = value; }
	void SetMaxSpeed(float value) { m_max_speed = value; }
	void SetDeacceleration(float value) { m_deceleration = value; }



protected:
	void ApplyRotation(const Ogre::Vector3& dir, float dt);
	void ApplyImpulse(const Ogre::Vector3& dir, float dt);
	void QueryRaycast();

	Ogre::Vector3	m_direction;
	Ogre::Vector3	m_actual_direction;
	btVector3 m_offset;
	Ogre::Timer m_air_timer;

	float		m_max_speed;
	float		m_max_fall_speed;
	float		m_velocity;
	float		m_deceleration;
	float		m_friction;
	float		m_current_friction;
	float		m_air_deceleration;
	float		m_turn_speed;
	float		m_max_jump_height;
	float		m_jump_pwr;
	float		m_jump_timer;
	float		m_step_height;
	float		m_y_bottom_offset;
	float		m_fall_acceleration;
	float		m_fall_velocity;
	bool		m_has_follow_cam;
	bool		m_is_jumping;
	bool		m_on_ground;
	bool		m_is_moving;
	bool		m_limit_max_speed;
	bool		m_can_move;
};

enum TOTT_STATE{
	IDLING,
	CURIOUS,
	HAPPY
};

class TottController : public Component, public IComponentObserver, public IComponentSimulationStep, public IComponentUpdateable {
public:
	TottController(void) : m_quest_complete(false), m_rotation_target(Ogre::Vector3::ZERO), m_resize_bubble(false), m_speech_transition_state(0), m_quest_started(false), m_excited(false), m_exited_timer(0.0f), m_active(true) { m_type = COMPONENT_TOTT_CONTROLLER; }
	virtual ~TottController(void) {}
	
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, const Ogre::String& idle_anim_id, const Ogre::String& excited_anim_id, const Ogre::String& quest_item);
	virtual void Update(float dt);
	virtual void SimulationStep(btScalar time_step);

	void SetSpeechBubble(Ogre::String spbubble){ m_speech_bubble = spbubble; }
	Ogre::String GetSpeechBubble() { return m_speech_bubble; }

	void Idling();
	void Curious();
	void Happy();
	
	TottDef m_def;
	bool m_quest_done;
	float m_state_timer_counter;
	float m_state_timer;
	bool m_can_change_state;
	Ogre::Vector3 m_rotation_target;
	bool m_quest_complete;
	bool m_resize_bubble;
	bool m_quest_started;
	short m_speech_transition_state;
	bool m_colliding;
	bool m_excited;
	bool m_active;
	float m_exited_timer;
	PhysicsEngine* m_physics_engine;
	SoundData3D m_music;
	Ogre::String m_idle_anim_id;
	Ogre::String m_excited_anim_id;
	Ogre::String m_quest_item;
	Ogre::Camera* m_camera;
	Ogre::SceneNode* m_player_node;
	
protected:
	TOTT_STATE m_state;
	AnimationMsg m_anim_msg;

	Ogre::String m_speech_bubble;

	Ogre::String m_idle_animation;
	Ogre::String m_walk_animation;
	Ogre::String m_run_animation;
	Ogre::String m_react_animation;
	Ogre::String m_happy_animation;
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
	Generic6DofConstraintComponent(void) : m_physics_engine(NULL), m_constraint(NULL){ m_type = COMPONENT_GENERIC_6DOF_COMPONENT; }
	virtual ~Generic6DofConstraintComponent(void){}

	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine, const Generic6DofDef& def);
	btGeneric6DofConstraint* GetConstraint() const { return m_constraint; }

private:
	btGeneric6DofConstraint* m_constraint;
	PhysicsEngine* m_physics_engine;
};

class HingeConstraintComponent : public Component, public IComponentObserver{
public:
	HingeConstraintComponent(void){ m_type = COMPONENT_HINGE_CONSTRAINT; }
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

class RaycastCollisionComponent : public Component, public IComponentObserver{
public:
	RaycastCollisionComponent(void) : m_physics_engine(NULL){}
	virtual ~RaycastCollisionComponent(void){}

	virtual void Notify(int type, void* msg){}
	virtual void Shut(){}
	virtual void SetMessenger(ComponentMessenger* messenger){}
	virtual void Init(PhysicsEngine* physics_engine);

protected:
	PhysicsEngine* m_physics_engine;
};

class CameraRaycastCollisionComponent : public RaycastCollisionComponent {
public:
	CameraRaycastCollisionComponent(void){}
	virtual ~CameraRaycastCollisionComponent(void){}

	virtual void Init(PhysicsEngine* physics_engine);
	virtual void Shut();
	virtual void Notify(int type, void* msg);
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
};

class PlayerRaycastCollisionComponent : public RaycastCollisionComponent {
public:
	PlayerRaycastCollisionComponent(void) : m_into_bubble_vel(0.0f), m_stand_on_vel(0.0f), m_bounce_vel(0.0f){}
	virtual ~PlayerRaycastCollisionComponent(void){}

	virtual void Shut();
	virtual void Notify(int type, void* msg);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(PhysicsEngine* physics_engine);

	virtual void SetCustomVariables(float bounce_mod);

protected:
	int m_num_bounces;
	float m_bounce_mod;
	void PlayerBubble(GameObject*);
	void PlayerLandscape();

	float m_into_bubble_vel;
	float m_stand_on_vel;
	float m_bounce_vel;
	float m_bounce_power;
};

class BobbingComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	BobbingComponent(void) : m_player_node(NULL), m_start_moving(false){}
	virtual ~BobbingComponent(void){}

	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Notify(int type, void* msg);
	virtual void Init(Ogre::SceneNode* node);
	virtual void Update(float dt);

private:
	Ogre::SceneNode* m_node;
	Ogre::SceneNode* m_player_node;
	float m_current_time;
	float m_bob_timer;
	float m_rotation_speed;
	bool m_start_moving;	// decides if the leaf should start moving towards the player or not
	bool m_up;
	float m_y_distance;
	float m_move_speed;
	float m_scale_speed;
	float m_min_scale;
};

#endif // _N_PHYSICS_COMPONENTS_H_