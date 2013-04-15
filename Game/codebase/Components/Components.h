#ifndef _N_COMPONENTS_H_
#define _N_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "..\BtOgrePG.h"
#include "BulletDynamics\Character\btKinematicCharacterController.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class MeshRenderComponent : public Component, public IComponentObserver {
public:
	MeshRenderComponent(void) : m_node(NULL), m_entity(NULL), m_scene_manager(NULL){ m_type = COMPONENT_RENDERER; }
	virtual ~MeshRenderComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager);
	Ogre::SceneNode* GetSceneNode() const { return m_node; }
	Ogre::Entity* GetEntity() const { return m_entity; }
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	Ogre::SceneNode*	m_node;
	Ogre::Entity*		m_entity;
	Ogre::SceneManager* m_scene_manager;
};

class ChildSceneNodeComponent : public Component, public IComponentObserver{
public:
	ChildSceneNodeComponent(void){}
	virtual ~ChildSceneNodeComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Init(const Ogre::Vector3& position, const Ogre::String& id, Ogre::SceneNode* parent);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	Ogre::SceneNode* GetNode() const { return m_node; }
	const Ogre::String& GetId() const { return m_id; }

protected:
	Ogre::SceneNode* m_node;
	Ogre::String m_id;
};

class AnimationComponent : public MeshRenderComponent, public IComponentUpdateable{
public:
	AnimationComponent(void){ m_type = COMPONENT_ANIMATION; }
	virtual ~AnimationComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager);
	virtual void AddAnimationStates(unsigned int value = 1);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	std::vector<Ogre::AnimationState*> m_animation_states;
};

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
	virtual void Init(const Ogre::Vector3& position, Ogre::Entity* entity, float width, float height, float step_height, PhysicsEngine* physics_engine);
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

class CameraComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	CameraComponent(void) : m_scene_manager(NULL), m_camera(NULL), m_viewport(NULL), m_camera_id(Ogre::StringUtil::BLANK) { m_type = COMPONENT_CAMERA; }
	virtual ~CameraComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate = false, const Ogre::String& camera_id = Ogre::StringUtil::BLANK);
	virtual void Update(float dt);

	const Ogre::String& GetCameraId() const { return m_camera_id; }
	Ogre::Camera* GetCamera() const { return m_camera; }
	void ActivateCamera();
protected:
	Ogre::SceneManager* m_scene_manager;
	Ogre::Camera*		m_camera;
	Ogre::Viewport*		m_viewport;
	Ogre::String		m_camera_id;
};

class FollowCameraComponent : public CameraComponent{
public:
	FollowCameraComponent(void) : m_camera_goal(NULL), m_camera_pivot(NULL), m_camera_node(NULL), m_pivot_pitch(0) { m_type = COMPONENT_FOLLOW_CAMERA; }
	virtual ~FollowCameraComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate = false, const Ogre::String& camera_id = Ogre::StringUtil::BLANK);
	virtual void Update(float dt);

protected:
	void UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom);
	Ogre::SceneNode*	m_camera_pivot;
	Ogre::SceneNode*	m_camera_goal;
	Ogre::SceneNode*	m_camera_node;
	Ogre::Real			m_pivot_pitch;
};

#endif // _N_COMPONENTS_H_