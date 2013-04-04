#include "..\stdafx.h"
#include "Components.h"
#include "ComponentMessenger.h"
#include "..\BtOgrePG.h"
#include "..\BtOgreGP.h"
#include "..\PhysicsEngine.h"

void Transform::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_TRANSFORM_POSITION_GET, this);
	m_messenger->Register(MSG_TRANSFORM_SCALE_GET, this);
	m_messenger->Register(MSG_TRANSFORM_POSITION_SET, this);
	m_messenger->Register(MSG_TRANSFORM_SCALE_SET, this);
	m_messenger->Register(MSG_TRANSFORM_ORIENTATION_GET, this);
	m_messenger->Register(MSG_TRANSFORM_ORIENTATION_SET, this);
}

void Transform::Shut(){
	if (m_messenger){
		m_messenger->Unregister(this);
	}
}

void Transform::Notify(int type, void* msg){
	switch (type){
	case MSG_TRANSFORM_POSITION_GET:
		*static_cast<Ogre::Vector3*>(msg) = m_position;
		break;
	case MSG_TRANSFORM_POSITION_SET:
		m_position = *static_cast<Ogre::Vector3*>(msg);
		break;
	case MSG_TRANSFORM_SCALE_GET:
		*static_cast<Ogre::Vector3*>(msg) = m_scale;
		break;
	case MSG_TRANSFORM_SCALE_SET:
		m_scale = *static_cast<Ogre::Vector3*>(msg);
		break;
	case MSG_TRANSFORM_ORIENTATION_GET:
		*static_cast<Ogre::Quaternion*>(msg) = m_orientation;
		break;
	case MSG_TRANSFORM_ORIENTATION_SET:
		m_orientation = *static_cast<Ogre::Quaternion*>(msg);
		break;
	default:
		break;
	}
}

void Renderer::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->attachObject(m_entity);
}

void Renderer::Notify(int type, void* msg){
	switch (type){
	case MSG_TRANSFORM_POSITION_SET:
		m_node->setPosition(*static_cast<Ogre::Vector3*>(msg));
		break;
	case MSG_TRANSFORM_SCALE_SET:
		m_node->setScale(*static_cast<Ogre::Vector3*>(msg));
		break;
	case MSG_TRANSFORM_ORIENTATION_SET:
		m_node->setOrientation(*static_cast<Ogre::Quaternion*>(msg));
		break;
	default:
		break;
	}
}

void Renderer::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_TRANSFORM_POSITION_SET, this);
	m_messenger->Register(MSG_TRANSFORM_ORIENTATION_SET, this);
	m_messenger->Register(MSG_TRANSFORM_SCALE_SET, this);
}

void Renderer::Shut(){
	if (m_messenger){
		m_messenger->Unregister(this);
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

void Animation::SetMessenger(ComponentMessenger* messenger){
	Renderer::SetMessenger(messenger);
	m_messenger->Register(MSG_ANIMATION_PLAY, this);
	m_messenger->Register(MSG_ANIMATION_PAUSE, this);
}

void Animation::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	Renderer::Init(filename, scene_manager);
}

void Animation::AddAnimationStates(unsigned int value){
	for (unsigned int i = 0; i < value; i++){
		Ogre::AnimationState* a = NULL;
		m_animation_states.push_back(a);
	}
}

void Animation::Update(float deltatime){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		if (m_animation_states[i] != NULL){
			if (m_animation_states[i]->getEnabled()){
				m_animation_states[i]->addTime(deltatime);
			}
		}
	}
}

void Animation::Notify(int type, void* msg){
	Renderer::Notify(type, msg);
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

void Animation::Shut(){
	if (!m_animation_states.empty()){
		for (unsigned int i = 0; i < m_animation_states.size(); i++){
			if (m_animation_states[i] != NULL){
				m_animation_states[i]->setEnabled(false);
			}
		}
	}
	m_animation_states.clear();
	Renderer::Shut();
}


void Rigidbody::Notify(int type, void* msg){
	switch (type){
	case MSG_ADD_FORCE:
		{
			AddForceMsg& force = *static_cast<AddForceMsg*>(msg);
			m_rigidbody->applyForce(btVector3(force.strength, force.strength, force.strength), btVector3(force.dir.x, force.dir.y, force.dir.z));
		}
		break;
	default:
		break;
	}
}

void Rigidbody::Init(Ogre::Entity* entity, Ogre::SceneNode* node, PhysicsEngine* physics_engine){
	m_physics_engine = physics_engine;
	BtOgre::StaticMeshToShapeConverter converter(entity);
	m_shape = converter.createBox();
	btScalar mass = 5;
	btVector3 inertia;
	m_shape->calculateLocalInertia(mass, inertia);
	m_rigidbody_state = new BtOgre::RigidBodyState(m_messenger, node);
	m_rigidbody = new btRigidBody(mass, m_rigidbody_state, m_shape, inertia);
	m_physics_engine->AddRigidBody(m_rigidbody);
}

void Rigidbody::Shut(){
	m_physics_engine->RemoveRigidBody(m_rigidbody);
	m_rigidbody->getMotionState();
	delete m_rigidbody;
	m_rigidbody = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_rigidbody_state;
	m_rigidbody_state = NULL;
	m_messenger->Unregister(this);
}

void Rigidbody::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_ADD_FORCE, this);
}
