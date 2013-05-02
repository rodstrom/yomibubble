#include "stdafx.h"
#include "PhysicsComponents.h"
#include "..\PhysicsEngine.h"
#include "ComponentMessenger.h"
#include "GameObject.h"

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
	case MSG_RIGIDBODY_POSITION_SET:
		m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_RIGIDBODY_APPLY_IMPULSE:
		m_rigidbody->applyCentralImpulse(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_RIGIDBODY_COLLISION_FLAG_SET:
		m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | *static_cast<int*>(msg));
		break;
	case MSG_RIGIDBODY_COLLISION_FLAG_REMOVE:
		m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() &~ *static_cast<int*>(msg));
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
	m_collision_def.flag |= COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
}

void RigidbodyComponent::Shut(){
	if (m_rigidbody){
		m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
		delete m_rigidbody;
		m_rigidbody = NULL;
	}
	if (m_motion_state){
		delete m_motion_state;
		m_motion_state = NULL;
	}
	if (m_shape){
		delete m_shape;
		m_shape = NULL;
	}

	m_messenger->Unregister(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Unregister(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_APPLY_IMPULSE, this);
	m_messenger->Unregister(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Register(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_APPLY_IMPULSE, this);
	m_messenger->Register(MSG_RIGIDBODY_COLLISION_FLAG_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, this);
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
			bool jump = *static_cast<bool*>(msg);
			if (jump){
				if (m_on_ground){
					m_is_jumping = true;
					m_start_y_pos = m_rigidbody->getWorldTransform().getOrigin().y();
					m_on_ground = false;
				}
			}
			else{
				m_is_jumping = false;
				m_on_ground = false;
			}
		}
		break;
	case MSG_CHARACTER_CONTROLLER_GRAVITY_SET:
		m_rigidbody->setGravity(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_CHARACTER_CONTROLLER_SET_DIRECTION:
		m_direction = *static_cast<Ogre::Vector3*>(msg);
		m_direction *= 10.0f;
		break;
	case MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET:
		m_on_ground = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET:
		*static_cast<bool*>(msg) = m_on_ground;
		break;
	default:
		break;
	}
}

void CharacterController::Update(float dt){
	// remove?
}

void CharacterController::Shut(){
	RigidbodyComponent::Shut();
	if (m_messenger){
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONROLLER_JUMP, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET, this);
	}
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void CharacterController::SetMessenger(ComponentMessenger* messenger){
	RigidbodyComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
	m_messenger->Register(MSG_CHARACTER_CONROLLER_JUMP, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET, this);
}

void CharacterController::Init(const Ogre::Vector3& position, Ogre::Entity* entity, float step_height, PhysicsEngine* physics_engine){
	m_physics_engine = physics_engine;
	BtOgre::StaticMeshToShapeConverter converter(entity);
	m_shape = converter.createCapsule();
	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(BtOgre::Convert::toBullet(position));

	btScalar mass = 1.0f;
	btVector3 inertia;
	m_shape->calculateLocalInertia(mass, inertia);
	m_motion_state = new BtOgre::RigidBodyState(m_messenger);
	static_cast<BtOgre::RigidBodyState*>(m_motion_state)->UpdateOrientation(false);
	m_rigidbody = new btRigidBody(mass, m_motion_state, m_shape, inertia);
	m_collision_def.flag |= COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->setWorldTransform(start_transform);
	m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_rigidbody->setAngularFactor(0);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody);
	m_physics_engine->AddObjectSimulationStep(this);
}

void CharacterController::SimulationStep(btScalar time_step){
	float dt = (float)time_step;
	btVector3 vel = m_rigidbody->getLinearVelocity();
	float jump_strength = 0.0f;
	if (m_direction.x != 0.0f || m_direction.z != 0.0f){
		m_is_moving = true;
	}
	else{
		m_is_moving = false;
	}
	
	if (m_on_ground){
		//Ogre::Vector3 dir = m_direction;
		if (m_has_follow_cam){
		Ogre::SceneNode* node = NULL;
		Ogre::SceneNode* camera_node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &camera_node);
			if (node && camera_node){
				Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
				goal_dir += m_direction.z * camera_node->getOrientation().zAxis();
				goal_dir += m_direction.x * camera_node->getOrientation().xAxis();
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
				m_direction.y = 0.0f;
				m_direction.normalise();
				Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(m_direction);
				Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
				Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

				if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				node->yaw(Ogre::Degree(yaw_to_goal));
				m_rigidbody->setLinearVelocity(btVector3(m_direction.x, vel.y(), m_direction.z));
			}
		}
	}
	else{
		if (!m_has_follow_cam){
			m_rigidbody->setLinearVelocity(btVector3(m_direction.x, vel.y(), m_direction.z));
		}
		else {
			Ogre::SceneNode* node = NULL;
			Ogre::SceneNode* camera_node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &camera_node);
			if (node && camera_node){
				Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
				goal_dir += m_direction.z * camera_node->getOrientation().zAxis();
				goal_dir += m_direction.x * camera_node->getOrientation().xAxis();
				goal_dir.y = 0.0f;
				m_rigidbody->setLinearVelocity(btVector3(goal_dir.x, vel.y(), goal_dir.z));
			}
		}
	}
	if (m_is_jumping){
		float y_pos = m_rigidbody->getWorldTransform().getOrigin().y();
		if ((y_pos - m_start_y_pos) >= m_max_jump_height){
			m_is_jumping = false;
		}
		float jump_strength = m_jump_pwr * dt;
		vel = m_rigidbody->getLinearVelocity();
		m_rigidbody->setLinearVelocity(btVector3(vel.x(), jump_strength, vel.z()));
	}
}

void Point2PointConstraintComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_P2P_GET_CONSTRAINT:
		*static_cast<btPoint2PointConstraint**>(msg) = m_constraint;
		break;
	case MSG_P2P_GET_CONSTRAINT_SET_PIVOTA:
		m_constraint->setPivotA(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_P2P_GET_CONSTRAINT_SET_PIVOTB:
		m_constraint->setPivotB(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	default:
		break;
	}
}

void Point2PointConstraintComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_P2P_GET_CONSTRAINT, this);
		m_messenger->Unregister(MSG_P2P_GET_CONSTRAINT_SET_PIVOTA, this);
		m_messenger->Unregister(MSG_P2P_GET_CONSTRAINT_SET_PIVOTB, this);
	}
	if (m_constraint){
		m_physics_engine->GetDynamicWorld()->removeConstraint(m_constraint);
		delete m_constraint;
		m_constraint = NULL;
	}
}

void Point2PointConstraintComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_P2P_GET_CONSTRAINT, this);
	m_messenger->Register(MSG_P2P_GET_CONSTRAINT_SET_PIVOTA, this);
	m_messenger->Register(MSG_P2P_GET_CONSTRAINT_SET_PIVOTB, this);
}

void Point2PointConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b){
	m_physics_engine = physics_engine;
	m_constraint = new btPoint2PointConstraint(*body_a, *body_b, pivot_a, pivot_b);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint);
}

void Point2PointConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, const btVector3& pivot_a){
	m_physics_engine = physics_engine;
	m_constraint = new btPoint2PointConstraint(*body_a, pivot_a);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint);
}

void TriggerComponent::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, TriggerDef* def){
	m_physics_engine = physics_engine;
	switch (def->collider_type)
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

void TriggerComponent::Notify(int type, void* msg){
	RigidbodyComponent::Notify(type, msg);
}

void TriggerComponent::Shut(){
	RigidbodyComponent::Shut();
}

void TriggerComponent::SetMessenger(ComponentMessenger* messenger){
	RigidbodyComponent::SetMessenger(messenger);
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
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint, false);
}

void RaycastComponent::Init(PhysicsEngine* physics_engine, btCollisionObject* obj, const Ogre::String& body_id){
	m_physics_engine = physics_engine;
	m_physics_engine->AddRaycastComponent(this);
	m_raycast_def.collision_object = obj;
	m_raycast_def.body_id = body_id;
}

void RaycastComponent::Notify(int type, void* msg){

}

void RaycastComponent::Shut(){
	m_physics_engine->RemoveRaycastComponent(this);
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
		if (m_raycast_def.body_id != Ogre::StringUtil::BLANK){
			m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, m_raycast_def.body_id);
			if (body){
				m_raycast_def.origin = body->getWorldTransform().getOrigin();
			}
		}
		else {
			m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body);
			if (body){
				m_raycast_def.origin = body->getWorldTransform().getOrigin();
			}
		}
	}
	return m_raycast_def;
}

void HingeConstraintComponent::Notify(int type, void* msg){

}

void HingeConstraintComponent::Shut(){
	if (m_constraint){
		m_physics_engine->GetDynamicWorld()->removeConstraint(m_constraint);
		delete m_constraint;
		m_constraint = NULL;
	}
}

void HingeConstraintComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void HingeConstraintComponent::Init(PhysicsEngine* physics_engine, btRigidBody* body_a, btRigidBody* body_b, const btVector3& pivot_a, const btVector3& pivot_b, const btVector3& axis_a, const btVector3& axis_b){
	m_physics_engine = physics_engine;
	m_constraint = new btHingeConstraint(*body_a, *body_b, pivot_a, pivot_b, axis_a, axis_b);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint, true);
}

class IgnoreBodyAndGhostCast : public btCollisionWorld::ClosestRayResultCallback{
public:
	IgnoreBodyAndGhostCast(btRigidBody* body, btPairCachingGhostObject* ghost_object) :
		btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
		m_body(body), m_ghost_object(ghost_object){}

	btScalar AddSingleResult(btCollisionWorld::LocalRayResult& ray_result, bool normal_in_world_space){
		if (ray_result.m_collisionObject == m_body || ray_result.m_collisionObject == m_ghost_object){
			return 1.0f;
		}
		return ClosestRayResultCallback::addSingleResult(ray_result, normal_in_world_space);
	}

private:
	btRigidBody* m_body;
	btPairCachingGhostObject* m_ghost_object;
};

void DynamicCharacterController::Notify(int type, void* msg){
	switch (type){
	case MSG_CHARACTER_CONROLLER_JUMP:
		Jump();
		break;
	case MSG_CHARACTER_CONTROLLER_SET_DIRECTION:
		Move(*static_cast<Ogre::Vector3*>(msg));
		break;
	default:
		break;
	}
}

void DynamicCharacterController::Shut(){
	m_messenger->Unregister(MSG_CHARACTER_CONROLLER_JUMP, this);
	m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
	m_physics_engine->GetDynamicWorld()->removeRigidBody(m_body);
	m_physics_engine->GetDynamicWorld()->removeCollisionObject(m_ghost_object);
	delete m_collision_shape;
	delete m_motion_state;
	delete m_body;
	delete m_ghost_object;
}

void DynamicCharacterController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CHARACTER_CONROLLER_JUMP, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
}

void DynamicCharacterController::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, const DynamicCharacterControllerDef& def){
	m_physics_engine = physics_engine;
	m_deceleration = def.deceleration;
	m_jump_impulse = def.jump_impulse;
	m_jump_recharge_time = def.jump_recharge_time;
	m_step_height = def.step_height;
	m_max_speed = def.max_speed;
	m_bottom_y_offset = def.height / 2.0f + def.radius;
	m_bottom_rounded_region_y_offset = (def.height + def.radius) / 2.0f;
	m_previous_position = pos;
	m_manual_velocity = Ogre::Vector3::ZERO;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(BtOgre::Convert::toBullet(pos));
	m_collision_shape = new btCapsuleShape(def.radius, def.height);
	m_motion_state = new BtOgre::RigidBodyState(m_messenger);
	static_cast<BtOgre::RigidBodyState*>(m_motion_state)->UpdateOrientation(false);
	btVector3 intertia;
	m_collision_shape->calculateLocalInertia(def.mass, intertia);
	btRigidBody::btRigidBodyConstructionInfo rigidbody_ci(def.mass, m_motion_state, m_collision_shape, intertia);
	rigidbody_ci.m_friction = 0.0f;
	rigidbody_ci.m_restitution = 0.0f;
	rigidbody_ci.m_linearDamping = 0.0f;
	m_body = new btRigidBody(rigidbody_ci);
	m_body->setAngularFactor(0.0f);
	m_body->setActivationState(DISABLE_DEACTIVATION);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_body);
	m_ghost_object = new btPairCachingGhostObject;
	m_ghost_object->setCollisionShape(m_collision_shape);
	m_collision_def.flag |= COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_body->setUserPointer(&m_collision_def);
	m_body->setWorldTransform(transform);
	m_ghost_object->setUserPointer(&m_collision_def);
	m_ghost_object->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_physics_engine->GetDynamicWorld()->addCollisionObject(m_ghost_object, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
}

void DynamicCharacterController::Update(float dt){
	m_ghost_object->setWorldTransform(m_body->getWorldTransform());
	m_motion_state->getWorldTransform(m_motion_transform);
	m_on_ground = false;
	ParseGhostContacts();
	UpdatePosition(dt);
	UpdateVelocity(dt);
	if (m_jump_recharge_timer < m_jump_recharge_time){
		m_jump_recharge_timer += dt;
	}
}

void DynamicCharacterController::Jump(){
	if (m_on_ground && m_jump_recharge_timer >= m_jump_recharge_time){
		m_jump_recharge_timer = 0.0f;
		m_body->applyCentralImpulse(btVector3(0.0f, m_jump_impulse, 0.0f));
		const float jump_y_offset = 0.01f;
		float previous_y = m_body->getWorldTransform().getOrigin().y();
		m_body->getWorldTransform().getOrigin().setY(previous_y + jump_y_offset);
	}
}


void DynamicCharacterController::Move(const Ogre::Vector3& dir){
	Move(Ogre::Vector2(dir.x, dir.z));
}

void DynamicCharacterController::Move(const Ogre::Vector2& dir){
	Ogre::Vector2 velocityXZ(dir + Ogre::Vector2(m_manual_velocity.x, m_manual_velocity.z));
	float speedXZ = velocityXZ.length();
	if (speedXZ > m_max_speed){
		velocityXZ = velocityXZ / speedXZ * m_max_speed;
	}
	m_manual_velocity.x = velocityXZ.x;
	m_manual_velocity.z = velocityXZ.y;
}

void DynamicCharacterController::ParseGhostContacts(){
	btManifoldArray manifold_array;
	btBroadphasePairArray& pair_array = m_ghost_object->getOverlappingPairCache()->getOverlappingPairArray();
	int num_pairs = pair_array.size();
	m_hitting_wall = false;
	m_surface_hit_normals.clear();
	for (int i = 0; i < num_pairs; i++){
		manifold_array.clear();
		const btBroadphasePair& pair = pair_array[i];
		btBroadphasePair* collision_pair = m_physics_engine->GetDynamicWorld()->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
		if (collision_pair == NULL){
			continue;
		}
		if (collision_pair->m_algorithm != NULL){
			collision_pair->m_algorithm->getAllContactManifolds(manifold_array);
		}
		for (int j = 0; j < manifold_array.size(); j++){
			btPersistentManifold* manifold = manifold_array[j];
			if (manifold->getBody0() == m_body){
				continue;
			}
			for (int p = 0; p < manifold->getNumContacts(); p++){
				const btManifoldPoint& point = manifold->getContactPoint(p);
				if (point.getDistance() < 0.0f){
					const btVector3& pt_b = point.getPositionWorldOnB();
					if (pt_b.getY() < m_motion_transform.getOrigin().getY() - m_bottom_rounded_region_y_offset){
						m_on_ground = true;
					}
					else {
						m_hitting_wall = true;
						m_surface_hit_normals.push_back(BtOgre::Convert::toOgre(point.m_normalWorldOnB));
					}
				}
			}
		}
	}
}

void DynamicCharacterController::UpdatePosition(float dt){
	IgnoreBodyAndGhostCast ray_callback_bottom(m_body, m_ghost_object);
	m_physics_engine->GetDynamicWorld()->rayTest(m_body->getWorldTransform().getOrigin(), m_body->getWorldTransform().getOrigin() - btVector3(0.0f, m_bottom_y_offset + m_step_height, 0.0f), ray_callback_bottom);
	if (ray_callback_bottom.hasHit()){
		float previous_y = m_body->getWorldTransform().getOrigin().y();
		m_body->getWorldTransform().getOrigin().setY(previous_y + (m_bottom_y_offset + m_step_height) * (1.0f - ray_callback_bottom.m_closestHitFraction));
		btVector3 vel(m_body->getLinearVelocity());
		vel.setY(0.0f);
		m_body->setLinearVelocity(vel);
		m_on_ground = true;
	}
	float test_offset = 1.07f;
	IgnoreBodyAndGhostCast ray_callback_top(m_body, m_ghost_object);
	/*m_physics_engine->GetDynamicWorld()->rayTest(m_body->getWorldTransform().getOrigin(), m_body->getWorldTransform().getOrigin() + btVector3(0.0f, m_bottom_y_offset + test_offset, 0.0f), ray_callback_top);
	if (ray_callback_top.hasHit()){
		m_body->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(m_previous_position));
		btVector3 vel(m_body->getLinearVelocity());
		vel.setY(0.0f);
		m_body->setLinearVelocity(vel);
	}*/
	m_previous_position = BtOgre::Convert::toOgre(m_body->getWorldTransform().getOrigin());
}

void DynamicCharacterController::UpdateVelocity(float dt){
	m_manual_velocity.y = m_body->getLinearVelocity().getY();
	m_body->setLinearVelocity(BtOgre::Convert::toBullet(m_manual_velocity));
	m_manual_velocity -= m_manual_velocity * m_deceleration * dt;
	if (m_hitting_wall){
		for (unsigned int i = 0, size = m_surface_hit_normals.size(); i < size; i++){
			Ogre::Vector3 vel_in_normal_dir(Project(m_manual_velocity, m_surface_hit_normals[i]));
			m_manual_velocity -= vel_in_normal_dir * 1.05f;
		}
		std::cout << m_manual_velocity.y << std::endl;
		m_body->setLinearVelocity(BtOgre::Convert::toBullet(m_manual_velocity));
		//return;
	}
}

Ogre::Vector3 DynamicCharacterController::Project(const Ogre::Vector3& lhs, const Ogre::Vector3& rhs) const{
	return lhs.dotProduct(rhs) / rhs.squaredLength() * rhs;
}