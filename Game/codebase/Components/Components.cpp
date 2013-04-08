#include "..\stdafx.h"
#include "Components.h"
#include "ComponentMessenger.h"
#include "..\BtOgrePG.h"
#include "..\BtOgreGP.h"
#include "..\PhysicsEngine.h"

void TransformComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_TRANSFORM_POSITION_GET, this);
	m_messenger->Register(MSG_TRANSFORM_SCALE_GET, this);
	m_messenger->Register(MSG_TRANSFORM_POSITION_SET, this);
	m_messenger->Register(MSG_TRANSFORM_SCALE_SET, this);
	m_messenger->Register(MSG_TRANSFORM_ORIENTATION_GET, this);
	m_messenger->Register(MSG_TRANSFORM_ORIENTATION_SET, this);
}

void TransformComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_TRANSFORM_POSITION_GET, this);
		m_messenger->Unregister(MSG_TRANSFORM_SCALE_GET, this);
		m_messenger->Unregister(MSG_TRANSFORM_POSITION_SET, this);
		m_messenger->Unregister(MSG_TRANSFORM_SCALE_SET, this);
		m_messenger->Unregister(MSG_TRANSFORM_ORIENTATION_GET, this);
		m_messenger->Unregister(MSG_TRANSFORM_ORIENTATION_SET, this);
	}
}

void TransformComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_TRANSFORM_POSITION_GET:
		*static_cast<Ogre::Vector3*>(msg) = m_position;
		break;
	case MSG_TRANSFORM_POSITION_SET:
		m_position = *static_cast<Ogre::Vector3*>(msg);
		m_messenger->Notify(MSG_ALL_POSITION_SET, msg);
		break;
	case MSG_TRANSFORM_SCALE_GET:
		*static_cast<Ogre::Vector3*>(msg) = m_scale;
		break;
	case MSG_TRANSFORM_SCALE_SET:
		m_scale = *static_cast<Ogre::Vector3*>(msg);
		m_messenger->Notify(MSG_ALL_SCALE_SET, msg);
		break;
	case MSG_TRANSFORM_ORIENTATION_GET:
		*static_cast<Ogre::Quaternion*>(msg) = m_orientation;
		break;
	case MSG_TRANSFORM_ORIENTATION_SET:
		m_orientation = *static_cast<Ogre::Quaternion*>(msg);
		m_messenger->Notify(MSG_ALL_ORIENTATION_SET, msg);
		break;
	default:
		break;
	}
}

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->attachObject(m_entity);
}

void MeshRenderComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_ALL_POSITION_SET:
		m_node->setPosition(*static_cast<Ogre::Vector3*>(msg));
		break;
	case MSG_ALL_SCALE_SET:
		m_node->setScale(*static_cast<Ogre::Vector3*>(msg));
		break;
	case MSG_ALL_ORIENTATION_SET:
		m_node->setOrientation(*static_cast<Ogre::Quaternion*>(msg));
		break;
	default:
		break;
	}
}

void MeshRenderComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_ALL_POSITION_SET, this);
	m_messenger->Register(MSG_ALL_ORIENTATION_SET, this);
	m_messenger->Register(MSG_ALL_SCALE_SET, this);
}

void MeshRenderComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_ALL_POSITION_SET, this);
		m_messenger->Unregister(MSG_ALL_ORIENTATION_SET, this);
		m_messenger->Unregister(MSG_ALL_SCALE_SET, this);
	}
	if (m_node != NULL){
		m_scene_manager->destroySceneNode(m_node);
		m_node = NULL;
	}
	if (m_entity != NULL){
		m_scene_manager->destroyEntity(m_entity);
		m_entity = NULL;
	}
}

void AnimationComponent::SetMessenger(ComponentMessenger* messenger){
	MeshRenderComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_ANIMATION_PLAY, this);
	m_messenger->Register(MSG_ANIMATION_PAUSE, this);
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	MeshRenderComponent::Init(filename, scene_manager);
}

void AnimationComponent::AddAnimationStates(unsigned int value){
	for (unsigned int i = 0; i < value; i++){
		Ogre::AnimationState* a = NULL;
		m_animation_states.push_back(a);
	}
}

void AnimationComponent::Update(float deltatime){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		if (m_animation_states[i] != NULL){
			if (m_animation_states[i]->getEnabled()){
				m_animation_states[i]->addTime(deltatime);
			}
		}
	}
}

void AnimationComponent::Notify(int type, void* msg){
	MeshRenderComponent::Notify(type, msg);
	switch (type){
	case MSG_ANIMATION_PLAY:
		{
			int index = static_cast<AnimationMsg*>(msg)->index;
			m_animation_states[index] = m_entity->getAnimationState(static_cast<AnimationMsg*>(msg)->id);
			if (m_animation_states[index] != NULL){
				m_animation_states[index]->setEnabled(true);
				m_animation_states[index]->setLoop(true);
			}
		}
		break;
	case MSG_ANIMATION_PAUSE:
		{
			int index = static_cast<AnimationMsg*>(msg)->index;
			if (m_animation_states[index] != NULL){
				m_animation_states[index]->setEnabled(false);
				m_animation_states[index]->setLoop(false);
			}
		}
		break;
	default:
		break;
	}
}

void AnimationComponent::Shut(){
	if (!m_animation_states.empty()){
		for (unsigned int i = 0; i < m_animation_states.size(); i++){
			if (m_animation_states[i] != NULL){
				m_animation_states[i]->setEnabled(false);
			}
		}
	}
	m_animation_states.clear();
	m_messenger->Unregister(MSG_ANIMATION_PLAY, this);
	m_messenger->Unregister(MSG_ANIMATION_PAUSE, this);
	MeshRenderComponent::Shut();
}


void RigidbodyComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_ADD_FORCE:
		{
			AddForceMsg& force = *static_cast<AddForceMsg*>(msg);
			m_rigidbody->applyForce(btVector3(force.pwr.x, force.pwr.y, force.pwr.z), btVector3(force.dir.x, force.dir.y, force.dir.z));
		}
		break;
	default:
		break;
	}
}

void RigidbodyComponent::Init(Ogre::Entity* entity, Ogre::SceneNode* node, PhysicsEngine* physics_engine, btScalar p_mass, int collider_type){
	m_physics_engine = physics_engine;
	BtOgre::StaticMeshToShapeConverter converter(entity);
	switch (collider_type){
	case COLLIDER_BOX:
		m_shape = converter.createBox();
		break;
	case COLLIDER_CAPSULE:
		m_shape = converter.createCapsule();
		break;
	case COLLIDER_CYLINDER:
		m_shape = converter.createCylinder();
		break;
	case COLLIDER_SPHERE:
		m_shape = converter.createSphere();
		break;
	default:
		break;
	}
	btScalar mass = p_mass;
	btVector3 inertia;
	m_shape->calculateLocalInertia(mass, inertia);
	m_rigidbody_state = new BtOgre::RigidBodyState(m_messenger, node);
	m_rigidbody = new btRigidBody(mass, m_rigidbody_state, m_shape, inertia);
	m_physics_engine->AddRigidBody(m_rigidbody);
}

void RigidbodyComponent::Shut(){
	m_physics_engine->RemoveRigidBody(m_rigidbody);
	m_rigidbody->getMotionState();
	delete m_rigidbody;
	m_rigidbody = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_rigidbody_state;
	m_rigidbody_state = NULL;
	m_messenger->Unregister(MSG_ADD_FORCE, this);
	m_messenger->Unregister(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_ORIENTATION_SET, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_ADD_FORCE, this);
	m_messenger->Register(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_ORIENTATION_SET, this);
}
