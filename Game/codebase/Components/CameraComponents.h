#ifndef _N_CAMERA_COMPONENTS_H_
#define _N_CAMERA_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "PhysicsComponents.h"
#include "..\PhysicsEngine.h"
#include "..\InputSystem.h"

class CameraComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	CameraComponent(void) : m_scene_manager(NULL), m_camera(NULL), m_viewport(NULL), m_camera_id(Ogre::StringUtil::BLANK) { m_type = COMPONENT_CAMERA; m_update = true; }
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

class FollowCameraComponent : public CameraComponent, public IComponentSimulationStep{
public:
	FollowCameraComponent(void) : m_camera_goal(NULL), m_camera_pivot(NULL), m_camera_node(NULL), m_pivot_pitch(0), m_movement_speed(1.0f) { m_type = COMPONENT_FOLLOW_CAMERA; }
	virtual ~FollowCameraComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate = false, const Ogre::String& camera_id = Ogre::StringUtil::BLANK);
	virtual void Update(float dt);
	virtual void SimulationStep(btScalar time_step);
	void SetTrigger(TriggerComponent* trigger) { m_trigger = trigger; }
	void SetPhysEngine(PhysicsEngine* physics_engine) { m_physics_engine = physics_engine; m_physics_engine->AddObjectSimulationStep(this); }
	void SetMovementSpeed(float value) { m_movement_speed = value; }
	void SetInputSystem(InputSystem* input_system) { m_input_system = input_system; }

	void SetCustomVariables(int inverted_camera, float camera_zoom_speed, float stick_rotation_acceleration, float change_angle_after_player, float default_distance, float default_pitch);

	AltRaycastDef		m_left_ray;
	AltRaycastDef		m_right_ray;
	AltRaycastDef		m_top_ray;
	AltRaycastDef		m_bot_ray;

	void SetNode(Ogre::SceneNode* node) { 
		m_node = node; 
		m_left_ray.node = node;
		m_right_ray.node = node;
		m_top_ray.node = node;
		m_bot_ray.node = node;
	}
	bool QueryRaycast();

	Ogre::SceneNode*	m_node; //testing for env coll

protected:
	void UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom);
	Ogre::SceneNode*	m_camera_pivot;
	Ogre::SceneNode*	m_camera_goal;
	Ogre::SceneNode*	m_camera_node;
	Ogre::Real			m_pivot_pitch;
	Ogre::Vector3		m_default_position;
	Ogre::Vector3		m_player_direction;
	bool				m_getting_input;
	bool				m_env_collision;
	bool				m_check_cam;
	float				m_default_distance;
	float				m_default_pitch;
	TriggerComponent*	m_trigger; //testing for env coll
	
	PhysicsEngine*		m_physics_engine;
	float				m_movement_speed;
	InputSystem*		m_input_system;

	bool				m_inverted_controller;

	float				m_camera_zoom_speed;
	float				m_camera_stick_rotation_acceleration;
	float				m_camera_change_angle_after_player;

	//float				m_min_pitch_angle;
	//float				m_max_pitch_angle;

	Ogre::TerrainGroup*	m_terrain_group;

//	TriggerComponent*	m_trigger;
};
//
//class CameraCollisionComponent : public Component, public IComponentObserver, public IComponentUpdateable{
//public:
//	CameraCollisionComponent(void) { m_type = COMPONENT_CAMERA_COLLISION; }
//	virtual ~CameraCollisionComponent(void){}
//
//	virtual void Notify(int type, void* msg);
//	virtual void Shut();
//	virtual void SetMessenger(ComponentMessenger* messenger);
//	virtual void Init(GameObject* player);
//	virtual void Update(float dt);
//
//protected:
//	GameObject* m_player;
//};

#endif //_N_CAMERA_COMPONENTS_H_