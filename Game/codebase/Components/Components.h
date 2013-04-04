#ifndef _N_COMPONENTS_H_
#define _N_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "..\BtOgrePG.h"

class Transform : public Component, public IComponentObserver{
public:
	Transform(void) : m_position(Ogre::Vector3::ZERO), m_orientation(Ogre::Quaternion::IDENTITY), m_scale(Ogre::Vector3::UNIT_SCALE){}
	virtual ~Transform(void){}
	virtual void Notify(int type, void* message);

	const Ogre::Vector3& GetPosition() const { return m_position; }
	const Ogre::Quaternion& GetOrientation() const { return m_orientation; }
	const Ogre::Vector3& GetScale()    const { return m_scale; }
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Shut();

protected:
	Ogre::Vector3		m_position;
	Ogre::Vector3		m_scale;
	Ogre::Quaternion	m_orientation;
};

class Renderer : public Component, public IComponentObserver {
public:
	Renderer(void) : m_node(NULL), m_entity(NULL), m_scene_manager(NULL){}
	virtual ~Renderer(void){}
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

class Animation : public Renderer, public IComponentUpdateable{
public:
	Animation(void){}
	virtual ~Animation(void){}
	virtual void Update(float deltatime);
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager);
	virtual void AddAnimationStates(unsigned int value = 1);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	std::vector<Ogre::AnimationState*> m_animation_states;
};

class PhysicsEngine;
class Rigidbody : public Component, public IComponentObserver{
public:
	Rigidbody(void) : m_rigidbody(NULL), m_rigidbody_state(NULL), m_shape(NULL){}
	virtual ~Rigidbody(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(Ogre::Entity* entity, Ogre::SceneNode* node, PhysicsEngine* physics_engine);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	btRigidBody*			m_rigidbody;
	BtOgre::RigidBodyState* m_rigidbody_state;
	btCollisionShape*		m_shape;
	PhysicsEngine*			m_physics_engine;
};

class Collider : public Component, public IComponentObserver{
public:
	Collider(void){}
	virtual ~Collider(void){}
	virtual void Notify(int type, void* message){}
	virtual void Shut(){}
	virtual void SetMessenger(ComponentMessenger* messenger){}

protected:

};

#endif // _N_COMPONENTS_H_