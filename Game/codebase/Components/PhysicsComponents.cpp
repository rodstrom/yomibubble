#include "stdafx.h"
#include "PhysicsComponents.h"
#include "..\PhysicsEngine.h"
#include "ComponentMessenger.h"
#include "GameObject.h"
#include "GameObjectPrereq.h"

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
	case MSG_RIGIDBODY_APPLY_IMPULSE:
		m_rigidbody->applyCentralImpulse(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
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

	m_rigidbody->setUserPointer(m_owner);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
}

void RigidbodyComponent::Shut(){
	m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
	m_rigidbody->getMotionState();
	delete m_rigidbody;
	m_rigidbody = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_motion_state;
	m_motion_state = NULL;
	m_messenger->Unregister(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Unregister(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_APPLY_IMPULSE, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Register(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_APPLY_IMPULSE, this);
}


void CharacterController::Notify(int type, void* msg){
	RigidbodyComponent::Notify(type, msg);
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
			m_is_jumping = *static_cast<bool*>(msg);
		}
		break;
	case MSG_CHARACTER_CONTROLLER_GRAVITY_SET:
		m_rigidbody->setGravity(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_CHARACTER_CONTROLLER_SET_DIRECTION:
		m_direction = *static_cast<Ogre::Vector3*>(msg);
		m_direction *= 10.0f;
		break;
	default:
		break;
	}
}

void CharacterController::Update(float dt){
	btVector3 vel = m_rigidbody->getLinearVelocity();
	float jump_strength = 0.0f;
	if (m_direction.x != 0.0f || m_direction.z != 0.0f){
		m_is_moving = true;
	}
	else{
		m_is_moving = false;
	}
	
	if (m_is_moving){   //if the character is moving in any direction
		Ogre::Vector3 dir = m_direction;
		if (m_has_follow_cam){
		Ogre::SceneNode* node = NULL;
		Ogre::SceneNode* camera_node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &camera_node);
			if (node && camera_node){
				Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
				goal_dir += dir.z * camera_node->getOrientation().zAxis();
				goal_dir += dir.x * camera_node->getOrientation().xAxis();
				goal_dir.y = 0.0f;
				//goal_dir.normalise();
				if (!m_is_jumping){
					Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(goal_dir);
					Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
					Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

					if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
					else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
					node->yaw(Ogre::Degree(yaw_to_goal));
				}
				m_rigidbody->setLinearVelocity(btVector3(goal_dir.x, vel.y(), goal_dir.z));
			}
		}
		else{
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				dir.y = 0.0f;
				dir.normalise();
				Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(dir);
				Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
				Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

				if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				node->yaw(Ogre::Degree(yaw_to_goal));
				m_rigidbody->setLinearVelocity(btVector3(dir.x, vel.y(), dir.z));
			}
		}
	}
	else{
		m_rigidbody->setLinearVelocity(btVector3(m_direction.x, vel.y(), m_direction.z));
	}
	if (m_is_jumping){
		float jump_strength = m_jump_pwr * dt;
		vel = m_rigidbody->getLinearVelocity();
		m_rigidbody->setLinearVelocity(btVector3(vel.x(), jump_strength, vel.z()));
	}
}

void CharacterController::LateUpdate(float dt){
	/*btVector3 from = m_rigidbody->getWorldTransform().getOrigin();
	btVector3 to = from;
	to.setY(to.y() - m_ray_length);
	m_physics_engine->GetDebugDraw()->drawLine(from,to,btVector4(0,0,0,1));
	btCollisionWorld::AllHitsRayResultCallback results(from, to);
	m_physics_engine->GetDynamicWorld()->rayTest(from,to,results);
	for (unsigned int i = 0; i < results.m_collisionObjects.size(); i++){
		std::cout << static_cast<GameObject*>(results.m_collisionObjects[i]->getUserPointer())->GetType() << std::endl;
	}*/
}

void CharacterController::Shut(){
	RigidbodyComponent::Shut();
	if (m_messenger){
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONROLLER_JUMP, this);
	}
}

void CharacterController::SetMessenger(ComponentMessenger* messenger){
	RigidbodyComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
	m_messenger->Register(MSG_CHARACTER_CONROLLER_JUMP, this);
}

void CharacterController::Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, PhysicsEngine* physics_engine){
	m_physics_engine = physics_engine;
	BtOgre::StaticMeshToShapeConverter converter(entity);
	m_shape = converter.createBox();
	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(BtOgre::Convert::toBullet(position));

	btScalar mass = 1.0f;
	btVector3 inertia;
	m_shape->calculateLocalInertia(mass, inertia);
	m_motion_state = new BtOgre::RigidBodyState(m_messenger);
	static_cast<BtOgre::RigidBodyState*>(m_motion_state)->UpdateOrientation(false);
	m_rigidbody = new btRigidBody(mass, m_motion_state, m_shape, inertia);
	m_rigidbody->setUserPointer(m_owner);
	m_rigidbody->setWorldTransform(start_transform);
	m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_rigidbody->setAngularFactor(0);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
}

void Point2PointConstraintComponent::Notify(int type, void* msg){

}

void Point2PointConstraintComponent::Shut(){
	if (m_constraint){
		m_physics_engine->GetDynamicWorld()->removeConstraint(m_constraint);
		delete m_constraint;
		m_constraint = NULL;
	}
}

void Point2PointConstraintComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void Point2PointConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b){
	m_physics_engine = physics_engine;
	m_constraint = new btPoint2PointConstraint(*body_a, *body_b, pivot_a, pivot_b);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint);
}

void TriggerComponent::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, TriggerDef* def){
	m_physics_engine = physics_engine;
	switch (def->type)
	{
	case COLLIDER_BOX:
		m_shape = new btBoxShape(btVector3(def->x, def->y, def->z));
		break;
	case COLLIDER_SPHERE:
		m_shape = new btSphereShape(def->radius);
		break;
	case COLLIDER_CAPSULE:
		m_shape = new btCapsuleShape(def->radius, def->y);
		break;
	case COLLIDER_CYLINDER:
		m_shape = new btCylinderShape(btVector3(def->x, def->y, def->z));
		break;
	default:
		break;
	}

	if (def->body_type == DYNAMIC_BODY){
		btVector3 inertia;
		m_shape->calculateLocalInertia(def->mass, inertia);
		m_motion_state = new btDefaultMotionState;
		m_rigidbody = new btRigidBody(def->mass, m_motion_state, m_shape, inertia);
	}
	else if (def->body_type == STATIC_BODY){
		m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		m_rigidbody = new btRigidBody(0, m_motion_state, m_shape, btVector3(0,0,0));
	}
	m_rigidbody->setUserPointer(m_owner);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(pos));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
}

void TriggerComponent::Notify(int type, void* message){

}

void TriggerComponent::Shut(){
	RigidbodyComponent::Shut();
}

void TriggerComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void Generic6DofConstraintComponent::Notify(int type, void* msg){

}

void Generic6DofConstraintComponent::Shut(){
	if (m_constraint){
		m_physics_engine->GetDynamicWorld()->removeConstraint(m_constraint);
		delete m_constraint;
		m_constraint = NULL;
	}
}

void Generic6DofConstraintComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void Generic6DofConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b, bool linear_reference){
	m_physics_engine = physics_engine;
	btTransform transform_a;
	transform_a.setIdentity();
	transform_a.setOrigin(pivot_a);
	btTransform transform_b;
	transform_b.setIdentity();
	transform_b.setOrigin(pivot_b);
	m_constraint = new btGeneric6DofConstraint(*body_a, *body_b, transform_a, transform_b, linear_reference);
}

void RaycastComponent::Notify(int type, void* msg){

}

void RaycastComponent::Shut(){

}

void RaycastComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void RaycastComponent::SetLength(const Ogre::Vector3& length){
	m_raycast_def.length = BtOgre::Convert::toBullet(length);
}

RaycastDef& RaycastComponent::GetRaycastDef(){
	if (m_attached){
		btRigidBody* body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, body);
		if (body){
			m_raycast_def.origin = body->getWorldTransform().getOrigin();
		}
	}
	return m_raycast_def;
}
