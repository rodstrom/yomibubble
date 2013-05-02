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
	COMPONENT_AI,
	COMPONENT_CHARACTER_CONTROLLER,
	COMPONENT_AUDIO,
	COMPONENT_CAMERA,
	COMPONENT_POINT2POINT_CONSTRAINT,
	COMPONENT_HINGE_CONSTRAINT,
	COMPONENT_FOLLOW_CAMERA,
	COMPONENT_NODE,
	COMPONENT_CHILD_NODE,
	COMPONENT_TRIGGER,
	COMPONENT_RAYCAST,
	COMPONENT_SIZE
};

enum EComponentMsg{
	MSG_ADD_FORCE = 0,
	MSG_NODE_GET_NODE,
	MSG_NODE_ATTACH_ENTITY,
	MSG_MESH_RENDERER_GET_ENTITY,
	MSG_RIGIDBODY_GET_BODY,
	MSG_RIGIDBODY_GRAVITY_SET,
	MSG_RIGIDBODY_POSITION_SET,
	MSG_RIGIDBODY_APPLY_IMPULSE,
	MSG_RIGIDBODY_COLLISION_FLAG_SET,
	MSG_RIGIDBODY_COLLISION_FLAG_REMOVE,
	MSG_ANIMATION_PLAY,
	MSG_ANIMATION_PAUSE,
	MSG_ANIMATION_BLEND,
	MSG_CHARACTER_CONROLLER_VELOCITY_SET,
	MSG_CHARACTER_CONROLLER_TURN_SPEED_SET,
	MSG_CHARACTER_CONTROLLER_SET_DIRECTION,
	MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET,
	MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET,
	MSG_CHARACTER_CONROLLER_JUMP,
	MSG_CHARACTER_CONTROLLER_GRAVITY_SET,
	MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET,
	MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET,
	MSG_CAMERA_GET_CAMERA_NODE,
	MSG_CAMERA_GET_CAMERA,
	MSG_CAMERA_SET_ACTIVE,
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
	MSG_INCREASE_SCALE_BY_VALUE,
	MSG_SET_OBJECT_POSITION,
	MSG_OVERLAY_HOVER_ENTER,
	MSG_OVERLAY_HOVER_EXIT,
	MSG_OVERLAY_CALLBACK,
	MSG_CREATE_PARTICLE,
	MSG_LEAF_PICKUP,
	MSG_PLAYER_INPUT_SET_BUBBLE,
	MSG_PLAYER_INPUT_SET_STATE,
	MSG_PLAYER_INPUT_STATE_GET,
	MSG_BUBBLE_CONTROLLER_APPLY_IMPULSE,
	MSG_BUBBLE_CONTROLLER_CAN_ATTACH_SET,
	MSG_BUBBLE_CONTROLLER_CAN_ATTACH_GET,
	MSG_P2P_GET_CONSTRAINT,
	MSG_P2P_GET_CONSTRAINT_SET_PIVOTA,
	MSG_P2P_GET_CONSTRAINT_SET_PIVOTB,
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
	PLAYER_STATE_NORMAL = 0,
	PLAYER_STATE_ON_BUBBLE,
	PLAYER_STATE_INSIDE_BUBBLE,
	PLAYER_STATE_BOUNCING,
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

struct RigidBodyDef{
	int collider_type;
	float mass;
	float restitution;
	float friction;
};

struct AnimationMsg{
	int index;
	Ogre::String id;
};

struct CharControllerJumpDef{
	CharControllerJumpDef(void) : jump(false), _override(false){}
	CharControllerJumpDef(bool p_jump, bool p_override) : jump(p_jump), _override(p_override){}
	bool jump;
	bool _override;
};

struct AddForceMsg{
	Ogre::Vector3 pwr;
	Ogre::Vector3 dir;
};

struct ButtonDef{
	Ogre::String mat_hover;
	Ogre::String mat_exit;
	Ogre::String cont_name;
	Ogre::String overlay_name;
	Ogre::String mat_start_button;
	std::function<void()> func;
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
};

struct RaycastDef{
	btCollisionObject* collision_object;
	btVector3 origin;
	btVector3 length;
	Ogre::String body_id;
};

struct BubbleDef{
	btRigidBody* connection_body;		// the static trigger body the joint will connect to while blowing.
	float start_scale;
	float restitution;
	float friction;
};

struct PlayerInputDef{

};

struct DynamicCharacterControllerDef{
	float step_height;
	float radius;
	float height;
	float jump_impulse;
	float deceleration;
	float max_speed;
	float jump_recharge_time;
	float mass;
};

#endif // _N_COMPONENTS_PREREQ_H_