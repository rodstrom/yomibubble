#ifndef _N_COMPONENTS_PREREQ_H_
#define _N_COMPONENTS_PREREQ_H_

#include <functional>

enum EComponentType{
	COMPONENT_NONE = 0,
	COMPONENT_MESH_RENDER,
	COMPONENT_ANIMATION,
	COMPONENT_PLAYER_INPUT,
	COMPONENT_BUBBLE_CONTROL,
	COMPONENT_RIGIDBODY,
	COMPONENT_SHAPE,
	COMPONENT_AI,
	COMPONENT_CHARACTER_CONTROLLER,
	COMPONENT_AUDIO,
	COMPONENT_CAMERA,
	COMPONENT_CAMERA_COLLISION,
	COMPONENT_POINT2POINT_CONSTRAINT,
	COMPONENT_GENERIC_6DOF_COMPONENT,
	COMPONENT_HINGE_CONSTRAINT,
	COMPONENT_FOLLOW_CAMERA,
	COMPONENT_NODE,
	COMPONENT_CHILD_NODE,
	COMPONENT_TRIGGER,
	COMPONENT_SYNCED_TRIGGER,
	COMPONENT_RAYCAST,
	COMPONENT_TERRAIN,
	COMPONENT_PLAYER_STAFF,
	COMPONENT_SPEECH_BUBBLE,
	COMPONENT_SIZE
};

enum EComponentMsg{
	MSG_ADD_FORCE = 0,
	MSG_NODE_GET_NODE,
	MSG_NODE_ATTACH_ENTITY,
	MSG_MESH_RENDERER_GET_ENTITY,
	MSG_MESH_SET_MATERIAL_NAME,
	MSG_RIGIDBODY_GET_BODY,
	MSG_RIGIDBODY_GRAVITY_SET,
	MSG_RIGIDBODY_POSITION_SET,
	MSG_RIGIDBODY_POSITION_GET,
	MSG_RIGIDBODY_APPLY_IMPULSE,
	MSG_RIGIDBODY_COLLISION_FLAG_SET,
	MSG_RIGIDBODY_COLLISION_FLAG_REMOVE,
	MSG_RIGIDBODY_COMPOUND_SHAPE_GET,
	MSG_ANIMATION_PLAY,
	MSG_ANIMATION_PAUSE,
	MSG_ANIMATION_BLEND,
	MSG_ANIMATION_LOOP,
	MSG_ANIMATION_QUEUE,
	MSG_ANIMATION_GET_CURRENT_NAME,
	MSG_ANIMATION_IS_DONE,
	MSG_ANIMATION_CLEAR_QUEUE,
	MSG_ANIMATION_CLEAR_CALLBACK,
	MSG_ANIMATION_CALLBACK,
	MSG_ANIMATION_SET_WAIT,
	MSG_CHARACTER_CONTROLLER_VELOCITY_SET,
	MSG_CHARACTER_CONTROLLER_TURN_SPEED_SET,
	MSG_CHARACTER_CONTROLLER_SET_DIRECTION,
	MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET,
	MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET,
	MSG_CHARACTER_CONTROLLER_GET_FALL_VELOCITY,
	MSG_CHARACTER_CONTROLLER_APPLY_IMPULSE,
	MSG_CHARACTER_CONTROLLER_APPLY_ROTATION,
	MSG_SET_OBJECT_ORIENTATION,
	MSG_CHARACTER_CONTROLLER_JUMP,
	MSG_CHARACTER_CONTROLLER_GRAVITY_SET,
	MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET,
	MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET,
	MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED,
	MSG_FOLLOW_CAMERA_GET_ORIENTATION,
	MSG_CAMERA_GET_CAMERA_NODE,
	MSG_CAMERA_GET_CAMERA,
	MSG_CAMERA_SET_ACTIVE,
	MSG_CAMERA_COLL_UPDATE,
	MSG_INPUT_MANAGER_GET,
	MSG_GET_2D_OVERLAY_CONTAINER,
	MSG_GET_OVERLAY_COLLISION_CALLBACK,
	MSG_CHILD_NODE_GET_NODE,
	MSG_SFX2D_PLAY,
	MSG_SFX2D_STOP,
	MSG_SFX3D_PLAY,
	MSG_SFX3D_STOP,
	MSG_MUSIC2D_PLAY,
	MSG_MUSIC2D_STOP,
	MSG_MUSIC3D_PLAY,
	MSG_MUSIC3D_STOP,
	MSG_DEFAULT_CAMERA_POS,
	MSG_INCREASE_SCALE_BY_VALUE,
	MSG_SET_OBJECT_POSITION,
	MSG_OVERLAY_SHOW,
	MSG_OVERLAY_HIDE,
	MSG_OVERLAY_HOVER_ENTER,
	MSG_OVERLAY_HOVER_EXIT,
	MSG_OVERLAY_CALLBACK,
	MSG_CREATE_PARTICLE,
	MSG_LEAF_PICKUP,
	MSG_SP_BUBBLE_SHOW,
	MSG_PLAYER_INPUT_SET_BUBBLE,
	MSG_PLAYER_INPUT_SET_STATE,
	MSG_PLAYER_INPUT_STATE_GET,
	MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE,
	MSG_BUBBLE_CONTROLLER_ACTIVATE,
	MSG_BUBBLE_CONTROLLER_READY,
	MSG_BUBBLE_CONTROLLER_TIMER_RUN,
	MSG_P2P_GET_CONSTRAINT,
	MSG_P2P_GET_CONSTRAINT_SET_PIVOTA,
	MSG_P2P_GET_CONSTRAINT_SET_PIVOTB,
	MSG_START_TIMER,
	MSG_RAYCAST_COLLISION_GAME_OBJECT,
	MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT,
	MSG_CAMERA_RAYCAST_COLLISION_STATIC_ENVIRONMENT,
	MSG_CAMERA_ENV_COLLISION,
	MSG_ON_GROUND,
	MSG_TGRAPH_STOP,
	MSG_TOTT_STATE_CHANGE,
	MSG_WAYPOINT_PAUSE,
	MSG_WAYPOINT_START,
	MSG_DEFAULT_WAY_POINTS,
	MSG_SIZE
};

enum EColliderType{
	COLLIDER_BOX = 0,
	COLLIDER_SPHERE,
	COLLIDER_CAPSULE,
	COLLIDER_CYLINDER,
	COLLIDER_TRIANGLE_MESH_SHAPE
};

enum EBodyType{
	DYNAMIC_BODY = 0,
	STATIC_BODY
};

enum EPlayerState{
	PLAYER_STATE_IDLE = 0,
	PLAYER_STATE_MOVE,
	PLAYER_STATE_BLOW_BUBBLE,
	PLAYER_STATE_ON_BUBBLE,
	PLAYER_STATE_INSIDE_BUBBLE,
	PLAYER_STATE_JUMP,
	PLAYER_STATE_FALLING,
	PLAYER_STATE_LAND,
	PLAYER_STATE_BOUNCE,
	PLAYER_STATE_HOLD_OBJECT,
	PLAYER_STATE_SIZE
};

class GameObject;
class ComponentMessenger;
class Component{
public:
	Component(void) : m_type(COMPONENT_NONE), m_messenger(NULL), m_owner(NULL), m_update(false){}
	virtual ~Component(void){}
	int GetComponentType() const { return m_type; }
	GameObject* GetOwner() const { return m_owner; }
	virtual void SetType(int type) { m_type = type; }
	virtual void SetOwner(GameObject* owner) { m_owner = owner; }
	virtual void SetMessenger(ComponentMessenger* messenger) = 0;
	virtual void Shut() = 0;
	virtual bool DoUpdate() const { return m_update; }
	ComponentMessenger* GetMessenger() const { return m_messenger; }
protected:
	ComponentMessenger* m_messenger;
	GameObject* m_owner;
	int m_type;
	bool m_update;
};

class IComponentObserver{
public:
	IComponentObserver(void){ m_id = "component" + NumberToString(m_object_counter); }
	virtual ~IComponentObserver(void){}
	virtual void Notify(int type, void* message) = 0;
	void SetId(const Ogre::String& id) { m_id = id; }
	const Ogre::String& GetId() const { return m_id; }
protected:
	Ogre::String m_id;
	static int m_object_counter;
};

class IComponentUpdateable{
public:
	IComponentUpdateable(void){}
	virtual ~IComponentUpdateable(void){}
	virtual void Update(float dt) = 0;
};

class IComponentLateUpdate{
public:
	IComponentLateUpdate(void){}
	virtual ~IComponentLateUpdate(void){}
	virtual void LateUpdate(float dt) = 0;
};

class IComponentSimulationStep{
public:
	IComponentSimulationStep(void){}
	virtual ~IComponentSimulationStep(void){}
	virtual void SimulationStep(btScalar time_step) = 0;
};

struct AnimationMsg{
	AnimationMsg(void) : index(0), loop(true), full_body(false), blend(false), wait(false), duration(0.0f), blending_transition(0){}
	int index;
	int blending_transition;
	bool blend;
	bool loop;
	bool full_body;
	bool wait;
	float duration;
	Ogre::String id;
};

struct AddForceMsg{
	Ogre::Vector3 pwr;
	Ogre::Vector3 dir;
};

class AnimationBlender;
struct AnimationData{
	AnimationData(void) : anim_state(NULL), id(Ogre::StringUtil::BLANK), wait(false), animation_blender(NULL), active(true){}
	AnimationData(Ogre::AnimationState* p_anim_state, const Ogre::String& p_id, bool p_wait, AnimationBlender* p_animation_blender) : anim_state(p_anim_state), id(p_id), wait(p_wait), animation_blender(p_animation_blender), active(true) {}
	Ogre::AnimationState* anim_state;
	Ogre::String id;
	AnimationBlender* animation_blender;
	bool wait;
	bool active;
};

struct ButtonDef{
	Ogre::String mat_start_hover;
	Ogre::String mat_start;
	Ogre::String mat_exit_hover;
	Ogre::String mat_exit;
	Ogre::String mat_options_hover;
	Ogre::String mat_options;
	Ogre::String mat_credits_hover;
	Ogre::String mat_credits;
	Ogre::String cont_name;
	Ogre::String overlay_name;
	std::function<void()> func;
};

struct OverlayDef{
	Ogre::String cont_name;
	Ogre::String overlay_name;
};

struct GuiDef{
	Ogre::String tex_act;
	Ogre::String tex_inact;
	Ogre::String name;
	Ogre::String cont_name;
	int num_elems;
};

struct ParticleDef{
	Ogre::String particle_name;
	Ogre::String test;
};

struct RaycastDef{
	btCollisionObject* collision_object;
	btVector3 origin;
	btVector3 length;
	Ogre::String body_id;
};

struct AltRaycastDef{
	Ogre::SceneNode* node;
	btVector3 origin;
	btVector3 length;
};

struct BubbleDef{
	btRigidBody* connection_body;		// the static trigger body the joint will connect to while blowing.
	float start_scale;
	float restitution;
	float friction;
};

struct HingeConstraintDef{
	HingeConstraintDef(void) : body_a(NULL), body_b(NULL), pivot_a(btVector3(0,0,0)), pivot_b(btVector3(0,0,0)), axis_a(btVector3(0,0,0)), axis_b(btVector3(0,0,0)) {}
	btRigidBody* body_a;
	btRigidBody* body_b;
	btVector3 pivot_a;
	btVector3 pivot_b;
	btVector3 axis_a;
	btVector3 axis_b;
};

struct Point2PointConstraintDef{
	Point2PointConstraintDef(void) : body_a(NULL), body_b(NULL), pivot_a(btVector3(0,0,0)), pivot_b(btVector3(0,0,0)) {}
	btRigidBody* body_a;
	btRigidBody* body_b;
	btVector3 pivot_a;
	btVector3 pivot_b;
};

struct ShapeDef{
	ShapeDef(void) : collider_type(0), radius(0.0f), x(0.0f), y(0.0f), z(0.0f), position(btVector3(0,0,0)), orientation(btQuaternion(0,0,0,1)){}
	int collider_type;
	float radius;
	float x;
	float y;
	float z;
	btVector3 position;
	btQuaternion orientation;
};

struct Generic6DofDef{
	Generic6DofDef(void) : body_a(NULL), body_b(NULL), pivot_a(btVector3(0,0,0)), pivot_b(btVector3(0,0,0)), linear_reference(false){}
	btRigidBody* body_a;
	btRigidBody* body_b;
	btVector3 pivot_a;
	btVector3 pivot_b;
	bool linear_reference;
};

struct AnimNameMsg{
	AnimNameMsg(void) : id(Ogre::StringUtil::BLANK), index(0) {}
	AnimNameMsg(const Ogre::String& p_name, int p_index) : id(p_name), index(p_index){}
	Ogre::String id;
	int index;
};
struct AnimIsDoneMsg{
	AnimIsDoneMsg(void) : index(0), is_done(false){}
	AnimIsDoneMsg(int p_index, bool p_is_done) : index(p_index), is_done(p_is_done) {}
	int index;
	bool is_done;
};

struct DirDT{
	DirDT(void) : dir(Ogre::Vector3::ZERO), dt(0.0f){}
	DirDT(const Ogre::Vector3& p_dir, float p_dt) : dir(p_dir), dt(p_dt){}
	Ogre::Vector3 dir;
	float dt;
};

#endif // _N_COMPONENTS_PREREQ_H_