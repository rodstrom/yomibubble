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

void RigidbodyComponent::Init(const Ogre::Vector3& position, Ogre::Entity* entity, PhysicsEngine* physics_engine, const RigidBodyDef& def){
	m_physics_engine = physics_engine;
	BtOgre::StaticMeshToShapeConverter converter(entity);
	switch (def.collider_type){
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
	if (def.body_type == DYNAMIC_BODY){
		btScalar mass = (btScalar)def.mass;
		btVector3 inertia;
		m_shape->calculateLocalInertia(mass, inertia);
		m_motion_state = new BtOgre::RigidBodyState(m_messenger);
		m_rigidbody = new btRigidBody(mass, m_motion_state, m_shape, inertia);
	}
	else if (def.body_type == STATIC_BODY){
		m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		m_rigidbody = new btRigidBody(0, m_motion_state, m_shape, btVector3(0,0,0));
	}
	m_collision_def.flag = COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_rigidbody->setRestitution(def.restitution);
	m_rigidbody->setFriction(def.friction);
	m_rigidbody->setRollingFriction(def.rolling_friction);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody, def.collision_filter.filter, def.collision_filter.mask);
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

class IgnoreBodyCast : public btCollisionWorld::ClosestRayResultCallback{
public:
	IgnoreBodyCast(btRigidBody* body) :
		btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
		m_body(body){}

	btScalar AddSingleResult(btCollisionWorld::LocalRayResult& ray_result, bool normal_in_world_space){
		if (ray_result.m_collisionObject == m_body){
			return 1.0f;
		}
		return ClosestRayResultCallback::addSingleResult(ray_result, normal_in_world_space);
	}

private:
	btRigidBody* m_body;
};

void CharacterController::QueryRaycast(){
	IgnoreBodyCast ray_callback_bottom(m_rigidbody);
	m_physics_engine->GetDynamicWorld()->rayTest((m_rigidbody->getWorldTransform().getOrigin() + m_offset), (m_rigidbody->getWorldTransform().getOrigin() + m_offset) - btVector3(0,m_y_bottom_offset + m_step_height,0), ray_callback_bottom);
	if (ray_callback_bottom.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(ray_callback_bottom.m_collisionObject->getUserPointer());
		if (def.flag == COLLISION_FLAG_STATIC){
			m_messenger->Notify(MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT, NULL);
		}
		else if (def.flag == COLLISION_FLAG_GAME_OBJECT){
			GameObject* go = static_cast<GameObject*>(def.data);
			m_messenger->Notify(MSG_RAYCAST_COLLISION_GAME_OBJECT, &go);
		}
		m_on_ground = true;
	}
}

void CharacterController::Notify(int type, void* msg){
	RigidbodyComponent::Notify(type, msg);
	switch (type){
	case MSG_CHARACTER_CONTROLLER_VELOCITY_SET:
		m_velocity = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_TURN_SPEED_SET:
		m_turn_speed = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET:
		m_has_follow_cam = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET:
		*static_cast<bool*>(msg) = m_has_follow_cam;
		break;
	case MSG_CHARACTER_CONTROLLER_JUMP:
		{
			bool jump = *static_cast<bool*>(msg);
			if (jump){
				if (m_on_ground){
					m_is_jumping = true;
					m_jump_timer = 0.0f;
				}
			}
			else{
				m_is_jumping = false;
			}
		}
		break;
	case MSG_CHARACTER_CONTROLLER_GRAVITY_SET:
		m_rigidbody->setGravity(BtOgre::Convert::toBullet(*static_cast<Ogre::Vector3*>(msg)));
		break;
	case MSG_CHARACTER_CONTROLLER_SET_DIRECTION:
		{
			m_direction = *static_cast<Ogre::Vector3*>(msg);
			if (m_on_ground){
				m_direction *= 10.0f;
			}
			else {
				m_direction *= 2.0f;
			}
		}
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
	m_on_ground = false;
	QueryRaycast();
	btVector3 vel = m_rigidbody->getLinearVelocity();
	float speed = m_velocity * dt;
	float jump_strength = 0.0f;
	m_is_moving = false;
	if (m_direction.x != 0.0f || m_direction.z != 0.0f){
		m_is_moving = true;
	}
	
	if (m_on_ground){
		if (m_has_follow_cam){
		Ogre::SceneNode* node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				Ogre::Vector3 goal_dir = m_direction;
				m_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &goal_dir);
				Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(goal_dir);
				Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
				Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

				if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
				node->yaw(Ogre::Degree(yaw_to_goal));
				m_rigidbody->applyCentralImpulse(btVector3(goal_dir.x * speed, 0.0f, goal_dir.z * speed));
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
				m_rigidbody->applyCentralImpulse(btVector3(m_direction.x * speed, 0.0f, m_direction.z * speed));
			}
		}
	}
	else{
		if (!m_has_follow_cam){
			m_rigidbody->applyCentralImpulse(btVector3(m_direction.x * speed, 0.0f, m_direction.z * speed));
		}
		else {
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				Ogre::Vector3 goal_dir = m_direction;
				m_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &goal_dir);
				m_rigidbody->applyCentralImpulse(btVector3(goal_dir.x * speed, 0.0f, goal_dir.z * speed));
			}
		}
	}
	if (m_is_jumping){
		m_jump_timer += dt;
		if (m_jump_timer >= m_max_jump_height){
			m_is_jumping = false;
			m_jump_timer = 0.0f;
		}
		float jump_strength = m_jump_pwr * dt;
		vel = m_rigidbody->getLinearVelocity();
		m_rigidbody->setLinearVelocity(btVector3(vel.x(), jump_strength, vel.z()));
	}
}

void CharacterController::Shut(){
	m_compound_shape->removeChildShape(m_shape);
	m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
	delete m_shape;
	delete m_compound_shape;
	delete m_motion_state;
	delete m_rigidbody;
	if (m_messenger){
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_JUMP, this);
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
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_JUMP, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET, this);
}

void CharacterController::Init(const Ogre::Vector3& position, PhysicsEngine* physics_engine, const CharacterControllerDef& def){
	m_physics_engine = physics_engine;
	m_shape = new btCapsuleShape(def.radius, def.height);
	m_y_bottom_offset = def.height / 2.0f + def.radius;
	m_step_height = def.step_height;
	m_jump_pwr = def.jump_power;
	m_max_jump_height = def.max_jump_height;
	m_velocity = def.velocity;
	m_max_speed = def.max_speed;
	m_deceleration = def.deceleration;
	m_air_deceleration = def.air_deceleration;
	m_turn_speed = def.turn_speed;
	m_offset = BtOgre::Convert::toBullet(def.offset);
	m_compound_shape = new btCompoundShape;
	btTransform offset;
	offset.setIdentity();
	offset.setOrigin(BtOgre::Convert::toBullet(def.offset));
	m_compound_shape->addChildShape(offset, m_shape);

	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(BtOgre::Convert::toBullet(position));

	btVector3 inertia;
	m_shape->calculateLocalInertia(def.mass, inertia);
	m_motion_state = new BtOgre::RigidBodyState(m_messenger);
	static_cast<BtOgre::RigidBodyState*>(m_motion_state)->UpdateOrientation(false);
	m_rigidbody = new btRigidBody(def.mass, m_motion_state, m_compound_shape, inertia);
	m_collision_def.flag = COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->setWorldTransform(start_transform);
	m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_rigidbody->setAngularFactor(0);
	m_rigidbody->setRestitution(def.restitution);
	m_rigidbody->setFriction(def.friction);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody, def.collision_filter.filter, def.collision_filter.mask);
	m_physics_engine->AddObjectSimulationStep(this);
}

void CharacterController::SimulationStep(btScalar time_step){
	Ogre::Vector2 velXZ(m_rigidbody->getLinearVelocity().x(), m_rigidbody->getLinearVelocity().z());
	btScalar speedXZ = velXZ.length();
	if (speedXZ > m_max_speed){
		velXZ = velXZ / speedXZ * m_max_speed;
		m_rigidbody->setLinearVelocity(btVector3(velXZ.x, m_rigidbody->getLinearVelocity().y(), velXZ.y));
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
	m_collision_def.flag = COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(pos));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody, def->collision_filter.filter, def->collision_filter.mask);
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

void RaycastCollisionComponent::Init(PhysicsEngine* physics_engine){
	m_physics_engine = physics_engine;
}

void PlayerRaycastCollisionComponent::Shut(){
	m_messenger->Unregister(MSG_RAYCAST_COLLISION_GAME_OBJECT, this);
	m_messenger->Unregister(MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT, this);
}

void PlayerRaycastCollisionComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_RAYCAST_COLLISION_GAME_OBJECT:
		{
			GameObject* go = *static_cast<GameObject**>(msg);
			int type = go->GetType();
			if ((type == GAME_OBJECT_PINK_BUBBLE) || (type == GAME_OBJECT_BLUE_BUBBLE)){
				PlayerBubble(go);
			}
		}
		break;
	case MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT:
		PlayerLandscape();
		break;
	default:
		break;
	}
}

void PlayerRaycastCollisionComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RAYCAST_COLLISION_GAME_OBJECT, this);
	m_messenger->Register(MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT, this);
}

void PlayerRaycastCollisionComponent::PlayerBubble(GameObject* go){
	int player_state = 0;
	m_messenger->Notify(MSG_PLAYER_INPUT_STATE_GET, &player_state);
	if (player_state == PLAYER_STATE_NORMAL || player_state == PLAYER_STATE_BOUNCING){
		btRigidBody* body = NULL;
		m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
		if (body){   // bounce on bubble
			float y_vel = body->getLinearVelocity().y();
			if (y_vel < -4.0f && y_vel > -10.0f){
				player_state = PLAYER_STATE_BOUNCING;
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				Ogre::Vector3 impulse(0.0f, y_vel * -2.3f, 0.0f);
				m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse, "body");
			}
			else if (y_vel < -10.0f){   // go inside bubble
				player_state = PLAYER_STATE_INSIDE_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				int coll = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				m_messenger->Notify(MSG_RIGIDBODY_COLLISION_FLAG_SET, &coll, "body");
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);	// make sure direction is set to zero
			}
			else {
				player_state = PLAYER_STATE_ON_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);
			}
		}
	}
}

void PlayerRaycastCollisionComponent::PlayerLandscape(){
	int player_state = PLAYER_STATE_INSIDE_BUBBLE;
	m_messenger->Notify(MSG_PLAYER_INPUT_STATE_GET, &player_state);
	if (player_state != PLAYER_STATE_INSIDE_BUBBLE){
		player_state = PLAYER_STATE_NORMAL;
		m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
	}
}

void BobbingComponent::Shut(){

};

void BobbingComponent::SetMessenger(ComponentMessenger* messenger){

};

void BobbingComponent::Init(Ogre::SceneNode* node){
	m_node = node;

	m_current_time = 0.0f;
	m_bob_timer = 2.0f;

	m_up = true;
};

void BobbingComponent::Update(float dt){
	m_current_time += dt;

	if (m_current_time >= m_bob_timer){
		m_current_time = 0.0f;
	if (m_up) { m_up = false; }
	else { m_up = true; }
	}
	else{
	if (m_up){
		m_node->setPosition(m_node->getPosition().x, m_node->getPosition().y + 0.01, m_node->getPosition().z);
	}
	else{
			m_node->setPosition(m_node->getPosition().x, m_node->getPosition().y - 0.01, m_node->getPosition().z);
		}
	}
};

void SyncedTriggerComponent::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, struct TriggerDef* def){
	TriggerComponent::Init(pos, physics_engine, def);
	m_offset = def->offset;
}

void SyncedTriggerComponent::Update(float dt){
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		Ogre::Vector3 pos = node->_getDerivedPosition();
		pos += m_offset;
		m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(pos));
	}
}