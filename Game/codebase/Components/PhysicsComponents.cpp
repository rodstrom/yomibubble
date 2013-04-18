#include "stdafx.h"
#include "PhysicsComponents.h"
#include "..\PhysicsEngine.h"
#include "ComponentMessenger.h"

void RigidbodyComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_ADD_FORCE:
		{
			AddForceMsg& force = *static_cast<AddForceMsg*>(msg);
			m_rigidbody->applyForce(btVector3(force.pwr.x, force.pwr.y, force.pwr.z), btVector3(force.dir.x, force.dir.y, force.dir.z));
		}
		break;
	case MSG_INCREASE_SCALE_BY_VALUE:
		{
			btVector3 scale = m_shape->getLocalScaling();
			scale += BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg));
			m_shape->setLocalScaling(scale);
		}
		break;
	case MSG_SET_OBJECT_POSITION:
			m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_RIGIDBODY_GRAVITY_SET:
		m_rigidbody->setGravity(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_RIGIDBODY_GET_BODY:
		*static_cast<btRigidBody**>(msg) = m_rigidbody;
		break;
	default:
		break;
	}
}

void RigidbodyComponent::Init(const Ogre::Vector3& position, Ogre::Entity* entity, PhysicsEngine* physics_engine, float p_mass, int collider_type, int body_type){
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
	case COLLIDER_TRIANGLE_MESH_SHAPE:
		m_shape = converter.createTrimesh();
		break;
	default:
		break;
	}
	if (body_type == DYNAMIC_BODY){
		btScalar mass = (btScalar)p_mass;
		btVector3 inertia;
		m_shape->calculateLocalInertia(mass, inertia);
		m_motion_state = new BtOgre::RigidBodyState(m_messenger);
		m_rigidbody = new btRigidBody(mass, m_motion_state, m_shape, inertia);
	}
	else if (body_type == STATIC_BODY){
		m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		m_rigidbody = new btRigidBody(0, m_motion_state, m_shape, btVector3(0,0,0));
	}
	m_collision_object = new btCollisionObject;
	m_collision_object->setCollisionShape(m_shape);
	m_collision_object->setUserPointer(m_owner);
	m_rigidbody->setUserPointer(m_owner);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
	m_physics_engine->GetDynamicWorld()->addCollisionObject(m_collision_object);
}

void RigidbodyComponent::Shut(){
	m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
	m_physics_engine->GetDynamicWorld()->removeCollisionObject(m_collision_object);
	m_rigidbody->getMotionState();
	delete m_rigidbody;
	m_rigidbody = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_motion_state;
	m_motion_state = NULL;
	delete m_collision_object;
	m_collision_object = NULL;
	m_messenger->Unregister(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Unregister(MSG_RIGIDBODY_GRAVITY_SET, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Register(MSG_RIGIDBODY_GRAVITY_SET, this);
}

void CharacterController::Notify(int type, void* msg){
	switch (type){
	case MSG_CHARACTER_CONROLLER_VELOCITY_SET:
		m_velocity = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONROLLER_TURN_SPEED_SET:
		m_turn_speed = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET:
		m_has_follow_cam = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET:
		*static_cast<bool*>(msg) = m_has_follow_cam;
		break;
	case MSG_CHARACTER_CONROLLER_JUMP:
		{
			if (m_controller->canJump()){
				m_controller->jump();
				m_is_jumping = true;
			}
		}
		break;
	case MSG_CHARACTER_CONTROLLER_GRAVITY_SET:
		m_controller->setGravity(*static_cast<float*>(msg));
		break;
	case MSG_CHARACTER_CONTROLLER_WARP:
		m_controller->warp(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_CHARACTER_CONTROLLER_SET_DIRECTION:
		m_direction = BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg));
		break;
	default:
		break;
	}
}

void CharacterController::Update(float dt){
	m_last_y_pos = m_current_y_pos;
	m_current_y_pos = m_controller->getGhostObject()->getWorldTransform().getOrigin().y();

	//btVector3 walk_direction = btVector3(0.0,0.0,0.0);
	btScalar walk_speed = m_velocity * dt;

	/*if (m_move_left){
		walk_direction += btVector3(-1.0, 0.0, 0.0);
	}
	if (m_move_right){
		walk_direction += btVector3(1.0, 0.0, 0.0);
	}
	if (m_move_forward){
		walk_direction += btVector3(0.0, 0.0, -1.0);
	}
	if (m_move_backwards){
		walk_direction += btVector3(0.0, 0.0, 1.0);
	}*/
	if (m_direction != btVector3(0.0f, 0.0f, 0.0f)){   //if the character is moving in any direction
		if (m_has_follow_cam){
		Ogre::SceneNode* node = NULL;
		Ogre::SceneNode* camera_node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &camera_node);
			if (node && camera_node){
				Ogre::Vector3 dir = BtOgre::Convert::toOgre(m_direction);
				Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
				goal_dir += dir.z * camera_node->getOrientation().zAxis();
				goal_dir += dir.x * camera_node->getOrientation().xAxis();
				goal_dir.y = 0.0f;
				goal_dir.normalise();
				if (!m_is_jumping){
					Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(goal_dir);
					Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
					Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

					if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
					else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
					node->yaw(Ogre::Degree(yaw_to_goal));
				}
				m_controller->setWalkDirection(BtOgre::Convert::toBullet(goal_dir * (float)walk_speed));
			}
		}
		else{
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				Ogre::Vector3 dir = BtOgre::Convert::toOgre(m_direction);
				dir.y = 0.0f;
				dir.normalise();
				Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(dir);
				Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
				Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

				if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				node->yaw(Ogre::Degree(yaw_to_goal));
				m_controller->setWalkDirection(BtOgre::Convert::toBullet(dir * (float)walk_speed));
			}
		}
	}
	else{
		m_controller->setWalkDirection(m_direction * walk_speed);
	}
	if (m_is_jumping){
		if (m_controller->onGround()){
			m_is_jumping = false;
		}
	}
}

void CharacterController::LateUpdate(float dt){
	if (m_ghost_object){
		btTransform transform = m_ghost_object->getWorldTransform();
		btVector3 pos = transform.getOrigin();
		Ogre::SceneNode* node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
		if (node){
			node->setPosition(BtOgre::Convert::toOgre(pos));
		}
	}
}

bool CharacterController::IsFalling(){
	if (m_controller->onGround()){
		return false;
	}

	if (m_last_y_pos > m_current_y_pos){
		return true;
	}
	return false;
}

void CharacterController::Shut(){
	if (m_ghost_object){
		m_physics_engine->GetDynamicWorld()->removeCollisionObject(m_ghost_object);
		delete m_ghost_object;
		m_ghost_object = NULL;
	}
	if (m_shape){
		delete m_shape;
		m_shape = NULL;
	}
	if (m_controller){
		m_physics_engine->GetDynamicWorld()->removeAction(m_controller);
		delete m_controller;
		m_controller = NULL;
	}
	if (m_messenger){
		//m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_FORWARD, this);
		//m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS, this);
		//m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_LEFT, this);
		//m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_RIGHT, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONROLLER_JUMP, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_WARP, this);
	}
}

void CharacterController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	//m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_FORWARD, this);
	//m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS, this);
	//m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_LEFT, this);
	//m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_RIGHT, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
	m_messenger->Register(MSG_CHARACTER_CONROLLER_JUMP, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_WARP, this);
}

void CharacterController::Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, int collider_type, PhysicsEngine* physics_engine){
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
	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(BtOgre::Convert::toBullet(position));
	m_ghost_object = new btPairCachingGhostObject;
	m_ghost_object->setWorldTransform(start_transform);
	m_ghost_object->setCollisionShape(m_shape);
	m_ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	m_controller = new btKinematicCharacterController(m_ghost_object, m_shape, (btScalar)step_height);
	m_ghost_object->setUserPointer(m_owner);
	m_physics_engine->GetDynamicWorld()->addCollisionObject(m_ghost_object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter|btBroadphaseProxy::CharacterFilter);
	m_physics_engine->GetDynamicWorld()->addAction(m_controller);
}

void Point2PointConstraintComponent::Notify(int type, void* msg){

}

void Point2PointConstraintComponent::Shut(){

}

void Point2PointConstraintComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void Point2PointConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b){
	m_physics_engine = physics_engine;
	m_constraint = new btPoint2PointConstraint(*body_a, *body_b, pivot_a, pivot_b);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint);
}
