#ifndef _N_COMPONENTS_PREREQ_H_
#define _N_COMPONENTS_PREREQ_H_

enum EComponentType{
	COMPONENT_NONE = 0,
	COMPONENT_RENDERER ,
	COMPONENT_COLLIDER,
	COMPONENT_CONTROLLER,
	COMPONENT_ANIMATION,
	COMPONENT_RIGIDBODY,
	COMPONENT_AI
};

enum EComponentMsg{
	MSG_ADD_FORCE = 0,
	MSG_SET_ANIMATION,
	MSG_LOSE_LIFE,
	MSG_TRANSFORM_POSITION_SET,
	MSG_TRANSFORM_SCALE_SET,
	MSG_TRANSFORM_POSITION_GET,
	MSG_TRANSFORM_ORIENTATION_GET,
	MSG_TRANSFORM_ORIENTATION_SET,
	MSG_TRANSFORM_SCALE_GET,
	MSG_ANIMATION_PLAY,
	MSG_ANIMATION_PAUSE,
	MSG_SIZE
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
	virtual void Update(float deltatime) = 0;
};

struct AnimationMsg{
	int index;
	Ogre::String id;
};

struct AddForceMsg{
	float strength;
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