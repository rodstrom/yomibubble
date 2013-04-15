#ifndef _N_COMPONENTS_PREREQ_H_
#define _N_COMPONENTS_PREREQ_H_

enum EComponentType{
	COMPONENT_NONE = 0,
	COMPONENT_RENDERER ,
	COMPONENT_COLLIDER,
	COMPONENT_PLAYER_INPUT,
	COMPONENT_ANIMATION,
	COMPONENT_TRANSFORM,
	COMPONENT_RIGIDBODY,
	COMPONENT_AI,
	COMPONENT_CHARACTER_CONTROLLER,
	COMPONENT_BUTTON
};

enum EComponentMsg{
	MSG_ADD_FORCE = 0,
	MSG_NODE_GET_NODE,
	MSG_MESH_RENDERER_GET_ENTITY,
	MSG_RIGIDBODY_GET_BODY,
	MSG_ANIMATION_PLAY,
	MSG_ANIMATION_PAUSE,
	MSG_CHARACTER_CONROLLER_VELOCITY_SET,
	MSG_CHARACTER_CONROLLER_TURN_SPEED_SET,
	MSG_CHARACTER_CONTROLLER_MOVE_FORWARD,
	MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS,
	MSG_CHARACTER_CONTROLLER_MOVE_LEFT,
	MSG_CHARACTER_CONTROLLER_MOVE_RIGHT,
	MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM,
	MSG_CAMERA_GET_CAMERA_NODE,
	MSG_CAMERA_GET_CAMERA,
	MSG_CAMERA_SET_ACTIVE,
	MSG_INPUT_MANAGER_GET,
	MSG_GET_2D_OVERLAY_CONTAINER,
	MSG_GET_OVERLAY_COLLISION_CALLBACK,
	MSG_SIZE
};

enum EColliderType{
	COLLIDER_BOX = 0,
	COLLIDER_SPHERE,
	COLLIDER_CAPSULE,
	COLLIDER_CYLINDER
};

class GameObject;
class ComponentMessenger;
class Component{
public:
	Component(void) : m_type(COMPONENT_NONE), m_messenger(NULL), m_owner(NULL){}
	virtual ~Component(void){}
	int GetComponentType() const { return m_type; }
	GameObject* GetOwner() const { return m_owner; }
	virtual void SetType(int type) { m_type = type; }
	virtual void SetOwner(GameObject* owner) { m_owner = owner; }
	virtual void SetMessenger(ComponentMessenger* messenger) = 0;
	virtual void Shut() = 0;
protected:
	ComponentMessenger* m_messenger;
	GameObject* m_owner;
	int m_type;
};

class IComponentObserver{
public:
	IComponentObserver(void){}
	virtual ~IComponentObserver(void){}
	virtual void Notify(int type, void* message) = 0;
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

struct RigidBodyDef{
	int collider_type;
	btScalar mass;
};

struct AnimationMsg{
	int index;
	Ogre::String id;
};

struct AddForceMsg{
	Ogre::Vector3 pwr;
	Ogre::Vector3 dir;
};

/*
struct IntMessage{
	int value;
};

struct StringMessage{
	std::string line;
}*/

#endif // _N_COMPONENTS_PREREQ_H_