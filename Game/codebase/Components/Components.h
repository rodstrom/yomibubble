#ifndef _N_COMPONENTS_H_
#define _N_COMPONENTS_H_

#include "ComponentsPrereq.h"
#include "..\BtOgrePG.h"

class TransformComponent : public Component, public IComponentObserver{
public:
	TransformComponent(void) : m_position(Ogre::Vector3::ZERO), m_orientation(Ogre::Quaternion::IDENTITY), m_scale(Ogre::Vector3::UNIT_SCALE){ m_type = COMPONENT_TRANSFORM; }
	virtual ~TransformComponent(void){}
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

class RenderComponent : public Component, public IComponentObserver {
public:
	RenderComponent(void) : m_node(NULL), m_entity(NULL), m_scene_manager(NULL){ m_type = COMPONENT_RENDERER; }
	virtual ~RenderComponent(void){}
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

class AnimationComponent : public RenderComponent, public IComponentUpdateable{
public:
	AnimationComponent(void){ m_type = COMPONENT_ANIMATION; }
	virtual ~AnimationComponent(void){}
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
class RigidbodyComponent : public Component, public IComponentObserver{
public:
	RigidbodyComponent(void) : m_rigidbody(NULL), m_rigidbody_state(NULL), m_shape(NULL){ m_type = COMPONENT_RIGIDBODY; }
	virtual ~RigidbodyComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(Ogre::Entity* entity, Ogre::SceneNode* node, PhysicsEngine* physics_engine, btScalar p_mass, int collider_type);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	btRigidBody* GetRigidbody() { return m_rigidbody; }

protected:
	btRigidBody*			m_rigidbody;
	BtOgre::RigidBodyState* m_rigidbody_state;
	btCollisionShape*		m_shape;
	PhysicsEngine*			m_physics_engine;
};

class ColliderComponent : public Component, public IComponentObserver{
public:
	ColliderComponent(void){ m_type = COMPONENT_COLLIDER; }
	virtual ~ColliderComponent(void){}
	virtual void Notify(int type, void* message){}
	virtual void Shut(){}
	virtual void SetMessenger(ComponentMessenger* messenger){}

protected:

};

#endif // _N_COMPONENTS_H_