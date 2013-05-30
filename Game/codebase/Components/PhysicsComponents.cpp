#include "stdafx.h"
#include "PhysicsComponents.h"
#include "..\PhysicsEngine.h"
#include "ComponentMessenger.h"
#include "GameObject.h"
#include "PlayerInputComponent.h"
#include "..\Managers\VariableManager.h"
#include "..\RaycastCollision.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "VisualComponents.h"
#include "..\AnimationBlender.h"

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
	case MSG_SET_OBJECT_ORIENTATION:
		m_rigidbody->getWorldTransform().setRotation(BtOgre::Convert::toBullet(*static_cast<Ogre::Quaternion*>(msg)));
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
	case MSG_RIGIDBODY_POSITION_GET:
		*static_cast<Ogre::Vector3*>(msg) = BtOgre::Convert::toOgre(m_rigidbody->getWorldTransform().getOrigin());
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
	case MSG_RIGIDBODY_COMPOUND_SHAPE_GET:
		*static_cast<btCompoundShape**>(msg) = m_compound_shape;
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
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0,0,0));
	m_compound_shape = new btCompoundShape;
	m_compound_shape->addChildShape(transform, m_shape);

	if (def.body_type == DYNAMIC_BODY){
		btScalar mass = (btScalar)def.mass;
		btVector3 inertia;
		m_shape->calculateLocalInertia(mass, inertia);
		m_motion_state = new BtOgre::RigidBodyState(m_messenger);
		static_cast<BtOgre::RigidBodyState*>(m_motion_state)->UpdateOrientation(def.sync_orientation);
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
	if (m_shape){
		m_compound_shape->removeChildShape(m_shape);
		delete m_shape;
		m_shape = NULL;
		delete m_compound_shape;
		m_compound_shape = NULL;
	}

	if (m_rigidbody){
		m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
		delete m_rigidbody;
		m_rigidbody = NULL;
	}
	if (m_motion_state){
		delete m_motion_state;
		m_motion_state = NULL;
	}

	m_messenger->Unregister(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Unregister(MSG_SET_OBJECT_ORIENTATION, this);
	m_messenger->Unregister(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Unregister(MSG_RIGIDBODY_APPLY_IMPULSE, this);
	m_messenger->Unregister(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, this);
	m_messenger->Unregister(MSG_RIGIDBODY_POSITION_GET, this);
	m_messenger->Unregister(MSG_SET_OBJECT_ORIENTATION, this);
	m_messenger->Unregister(MSG_RIGIDBODY_COMPOUND_SHAPE_GET, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RIGIDBODY_GET_BODY, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Register(MSG_SET_OBJECT_ORIENTATION, this);
	m_messenger->Register(MSG_RIGIDBODY_GRAVITY_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_POSITION_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_APPLY_IMPULSE, this);
	m_messenger->Register(MSG_RIGIDBODY_COLLISION_FLAG_SET, this);
	m_messenger->Register(MSG_RIGIDBODY_COLLISION_FLAG_REMOVE, this);
	m_messenger->Register(MSG_RIGIDBODY_POSITION_GET, this);
	m_messenger->Register(MSG_SET_OBJECT_ORIENTATION, this);
	m_messenger->Register(MSG_RIGIDBODY_COMPOUND_SHAPE_GET, this);
}

void ShapeComponent::Notify(int type, void* msg){

}

void ShapeComponent::Init(const ShapeDef& def){
	m_messenger->Notify(MSG_RIGIDBODY_COMPOUND_SHAPE_GET, &m_compoound_shape, "body");
	switch (def.collider_type){
	case COLLIDER_BOX:
		m_shape = new btBoxShape(btVector3(def.x, def.y, def.z));
		break;
	case COLLIDER_CAPSULE:
		m_shape = new btCapsuleShape(def.radius, def.y);
		break;
	case COLLIDER_CYLINDER:
		m_shape = new btCylinderShape(btVector3(def.x, def.y, def.z));
		break;
	case COLLIDER_SPHERE:
		m_shape = new btSphereShape(def.radius);
		break;
	default:
		break;
	}
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(def.position);
	transform.setRotation(def.orientation);
	m_compoound_shape->addChildShape(transform, m_shape);
}

void ShapeComponent::Shut(){
	m_compoound_shape->removeChildShape(m_shape);
	delete m_shape;
	m_shape = NULL;
}

void ShapeComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void CharacterController::QueryRaycast(){
	IgnoreBodyCast ray_callback_bottom(m_rigidbody);
	btVector3 from = (m_rigidbody->getWorldTransform().getOrigin() + m_offset);
	btVector3 to = (m_rigidbody->getWorldTransform().getOrigin() + m_offset) - btVector3(0,m_y_bottom_offset + m_step_height,0);
	m_physics_engine->GetDynamicWorld()->rayTest(from, to, ray_callback_bottom);
	/*
	if (m_physics_engine->GetDebugDraw() != NULL){
		m_physics_engine->GetDebugDraw()->drawLine(from, to, btVector4(0,0,0,1));
	}
	*/
	if (ray_callback_bottom.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(ray_callback_bottom.m_collisionObject->getUserPointer());
		if (def.flag == COLLISION_FLAG_STATIC){
			m_messenger->Notify(MSG_RAYCAST_COLLISION_STATIC_ENVIRONMENT, NULL);
			m_on_ground = true;
		}
		else if (def.flag == COLLISION_FLAG_GAME_OBJECT){
			GameObject* go = static_cast<GameObject*>(def.data);
			m_messenger->Notify(MSG_RAYCAST_COLLISION_GAME_OBJECT, &go);
		}
		
		if (m_actual_direction == Ogre::Vector3::ZERO){
			if (m_current_friction != m_deceleration){
				m_current_friction = m_deceleration;
				m_rigidbody->setFriction(m_current_friction);
			}
		}
		else {
			if (m_current_friction != m_friction){
				m_current_friction = m_friction;
				m_rigidbody->setFriction(m_current_friction);
			}
		}
		
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
			m_actual_direction = *static_cast<Ogre::Vector3*>(msg);
			m_direction = *static_cast<Ogre::Vector3*>(msg);
			if (m_on_ground){
				m_direction *= 10.0f;
			}
			else {
				m_direction *= 2.0f;
			}
		}
		break;
	case MSG_CHARACTER_CONTROLLER_GET_FALL_VELOCITY:
		*static_cast<float*>(msg) = m_fall_velocity;
		break;
	case MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET:
		m_on_ground = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET:
		*static_cast<bool*>(msg) = m_on_ground;
		break;
	case MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED:
		m_limit_max_speed = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_APPLY_IMPULSE:
		{
			DirDT& dirdt = *static_cast<DirDT*>(msg);
			ApplyImpulse(dirdt.dir, dirdt.dt);
		}
		break;
	case MSG_CHARACTER_CONTROLLER_APPLY_ROTATION:
		{
			DirDT& dirdt = *static_cast<DirDT*>(msg);
			ApplyRotation(dirdt.dir, dirdt.dt);
		}
		break;
	default:
		break;
	}
}

void CharacterController::Update(float dt){
	btVector3 vel = m_rigidbody->getLinearVelocity();
	btVector3 pos = m_rigidbody->getWorldTransform().getOrigin();

	float speed = m_velocity * dt;
	float jump_strength = 0.0f;
	m_is_moving = false;
	if (m_direction.x != 0.0f || m_direction.z != 0.0f){
		m_is_moving = true;
	}
	if (m_on_ground && m_can_move){
		if (m_has_follow_cam){
			Ogre::Vector3 goal_dir = m_direction;
			m_messenger->Notify(MSG_FOLLOW_CAMERA_GET_ORIENTATION, &goal_dir);	// set direction from where the camera is looking
			ApplyRotation(goal_dir, dt);
			ApplyImpulse(goal_dir, dt);
		}
		else {
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
	}
}

void CharacterController::ApplyRotation(const Ogre::Vector3& dir, float dt){
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	Ogre::Vector3 goal_dir = dir.normalisedCopy();
	Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(goal_dir);
	Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
	Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;
	if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
	else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
	node->yaw(Ogre::Degree(yaw_to_goal));
}

void CharacterController::ApplyImpulse(const Ogre::Vector3& dir, float dt){
	float speed = m_velocity * dt;
	m_rigidbody->applyCentralImpulse(btVector3(dir.x * speed, 0.0f, dir.z * speed));
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
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_VELOCITY_SET, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_GET_FALL_VELOCITY, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_APPLY_ROTATION, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_APPLY_IMPULSE, this);
	}
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void CharacterController::SetMessenger(ComponentMessenger* messenger){
	RigidbodyComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_VELOCITY_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM_GET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_JUMP, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_SET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_IS_ON_GROUND_GET, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_LIMIT_MAX_SPEED, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_GET_FALL_VELOCITY, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_APPLY_ROTATION, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_APPLY_IMPULSE, this);
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
	m_max_fall_speed = def.max_fall_speed;
	m_fall_acceleration = def.fall_acceleration;
	m_friction = def.friction;
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
	m_on_ground = false;
	if (!m_is_jumping){
		QueryRaycast();
	}
	if (!m_on_ground){
		if (m_current_friction != 0.0f){
			m_current_friction = 0.0f;
			m_rigidbody->setFriction(m_current_friction);
		}
	}
	m_messenger->Notify(MSG_ON_GROUND, &m_on_ground);
	Ogre::Vector2 velXZ(m_rigidbody->getLinearVelocity().x(), m_rigidbody->getLinearVelocity().z());
	btScalar speedXZ = velXZ.length();
	if (speedXZ > 0.0f){
		if (m_actual_direction != Ogre::Vector3::ZERO){
			btScalar dir_speed = m_actual_direction.length();
			btScalar relative_max_speed = (m_max_speed * dir_speed);
			if (m_limit_max_speed){
				if (speedXZ > relative_max_speed){
					velXZ = velXZ / speedXZ * relative_max_speed;
					m_rigidbody->setLinearVelocity(btVector3(velXZ.x, m_rigidbody->getLinearVelocity().y(), velXZ.y));
				}
			}
		}
	}
	if (m_is_jumping){
		if (m_jump_timer >= m_max_jump_height){
			m_is_jumping = false;
			m_jump_timer = 0.0f;
		}
		float jump_strength = m_jump_pwr * time_step;
		btVector3 vel = m_rigidbody->getLinearVelocity();
		m_rigidbody->setLinearVelocity(btVector3(vel.x(), jump_strength, vel.z()));
	}
	/*if (m_on_ground){
		btScalar fall_speed = m_fall_acceleration * time_step;
		m_rigidbody->applyCentralImpulse(btVector3(0.0f, -fall_speed, 0.0f));
	}*/

	if (!m_on_ground && !m_is_jumping){
		btScalar fall_speed = m_fall_acceleration * time_step;
		m_rigidbody->applyCentralImpulse(btVector3(0.0f, -fall_speed, 0.0f));
		btVector3 vel = m_rigidbody->getLinearVelocity();
		if (vel.y() < -m_max_fall_speed){
			m_rigidbody->setLinearVelocity(btVector3(vel.x(), -m_max_fall_speed, vel.z()));
		}
		m_fall_velocity = vel.y();
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

void TriggerComponent::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, const TriggerDef& def){
	m_physics_engine = physics_engine;
	switch (def.collider_type)
	{
	case COLLIDER_BOX:
		m_shape = new btBoxShape(btVector3(def.x, def.y, def.z));
		break;
	case COLLIDER_SPHERE:
		m_shape = new btSphereShape(def.radius);
		break;
	case COLLIDER_CAPSULE:
		m_shape = new btCapsuleShape(def.radius, def.y);
		break;
	case COLLIDER_CYLINDER:
		m_shape = new btCylinderShape(btVector3(def.x, def.y, def.z));
		break;
	default:
		break;
	}
	m_compound_shape = new btCompoundShape;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0,0,0));
	m_compound_shape->addChildShape(transform, m_shape);
	if (def.body_type == DYNAMIC_BODY){
		btVector3 inertia;
		m_shape->calculateLocalInertia(def.mass, inertia);
		m_motion_state = new btDefaultMotionState;
		m_rigidbody = new btRigidBody(def.mass, m_motion_state, m_compound_shape, inertia);
	}
	else if (def.body_type == STATIC_BODY){
		m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		m_rigidbody = new btRigidBody(0, m_motion_state, m_shape, btVector3(0,0,0));
	}
	m_collision_def.flag = COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_rigidbody->setUserPointer(&m_collision_def);
	m_rigidbody->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(pos));
	m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_rigidbody, def.collision_filter.filter, def.collision_filter.mask);
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

void Generic6DofConstraintComponent::Init(PhysicsEngine* physics_engine, const Generic6DofDef& def){
	m_physics_engine = physics_engine;
	btTransform transform_a;
	transform_a.setIdentity();
	transform_a.setOrigin(def.pivot_a);
	btTransform transform_b;
	transform_b.setIdentity();
	transform_b.setOrigin(def.pivot_b);
	m_constraint = new btGeneric6DofConstraint(*def.body_a, *def.body_b, transform_a, transform_b, def.linear_reference);
	m_constraint->setLimit(0, 0, 0);
	m_constraint->setLimit(1, 0, 0);
	m_constraint->setLimit(2, 0, 0);
	m_physics_engine->GetDynamicWorld()->addConstraint(m_constraint, true);
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

void PlayerRaycastCollisionComponent::SetCustomVariables(float bounce_mod){
	m_bounce_mod = bounce_mod;
};

void PlayerRaycastCollisionComponent::Init(PhysicsEngine* physics_engine){
	RaycastCollisionComponent::Init(physics_engine);
	m_bounce_vel = VariableManager::GetSingletonPtr()->GetAsFloat("BounceVel");
	m_into_bubble_vel = VariableManager::GetSingletonPtr()->GetAsFloat("IntoBubbleVel");
	m_bounce_power = VariableManager::GetSingletonPtr()->GetAsFloat("BouncePower");
	m_num_bounces = 0;
}

void PlayerRaycastCollisionComponent::PlayerBubble(GameObject* go){
	int player_state = 0;
	m_messenger->Notify(MSG_PLAYER_INPUT_STATE_GET, &player_state);
	btRigidBody* body = NULL;
	m_messenger->Notify(MSG_RIGIDBODY_GET_BODY, &body, "body");
	float y_vel = 0.0f;
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_GET_FALL_VELOCITY, &y_vel);
	if (body){
		if (player_state == PLAYER_STATE_FALLING){
			if (y_vel < -m_bounce_vel && y_vel > -m_into_bubble_vel){   // bounce on bubble
				player_state = PLAYER_STATE_BOUNCE;
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				Ogre::Vector3 impulse(0.0f, y_vel * -m_bounce_power, 0.0f);
				m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse, "body");
				m_messenger->Notify(MSG_RIGIDBODY_APPLY_IMPULSE, &impulse);

				switch(m_num_bounces){
				case 0:
					m_messenger->Notify(MSG_SFX2D_PLAY,  &static_cast<PlayerInputComponent*>(m_owner->GetComponent(COMPONENT_PLAYER_INPUT))->m_bounce_1);
					break;
				case 1:
					m_messenger->Notify(MSG_SFX2D_PLAY,  &static_cast<PlayerInputComponent*>(m_owner->GetComponent(COMPONENT_PLAYER_INPUT))->m_bounce_2);
					break;
				case 2:
					m_messenger->Notify(MSG_SFX2D_PLAY,  &static_cast<PlayerInputComponent*>(m_owner->GetComponent(COMPONENT_PLAYER_INPUT))->m_bounce_3);
					break;
				case 3:
					m_messenger->Notify(MSG_SFX2D_PLAY,  &static_cast<PlayerInputComponent*>(m_owner->GetComponent(COMPONENT_PLAYER_INPUT))->m_bounce_4);
					break;
				default:
					//std::cout << "Something went wrong with number of bounces in PlayerRaycastCollisionComponent\n";
					break;
				}

				//m_messenger->Notify(MSG_SFX2D_PLAY,  &static_cast<PlayerInputComponent*>(m_owner->GetComponent(COMPONENT_PLAYER_INPUT))->m_bounce_sound);
				if (m_num_bounces < 3){
					m_num_bounces++;
				}
				std::cout << y_vel << std::endl;
			}
			else if (y_vel < -m_into_bubble_vel){   // go inside bubble
				player_state = PLAYER_STATE_INSIDE_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);	// make sure direction is set to zero
			}
			else {   // stand on bubble
				player_state = PLAYER_STATE_ON_BUBBLE;
				Ogre::Vector3 gravity(0,0,0);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_BUBBLE, &go);
				m_messenger->Notify(MSG_PLAYER_INPUT_SET_STATE, &player_state);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_GRAVITY_SET, &gravity);
				m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &gravity);
			}
		}
		else{
			m_num_bounces = 0;
		}
	}
}

void PlayerRaycastCollisionComponent::PlayerLandscape(){

}

void BobbingComponent::Shut(){
	m_messenger->Unregister(MSG_BOBBING_START_MOVING, this);
};

void BobbingComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_BOBBING_START_MOVING, this);
};

void BobbingComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_BOBBING_START_MOVING:
		{
			if (!m_start_moving){
				m_player_node = *static_cast<Ogre::SceneNode**>(msg);
				m_start_moving = true;
				m_up = true;
			}
		}
		break;
	default:
		break;
	}
}

void BobbingComponent::Init(Ogre::SceneNode* node){
	m_node = node;
	m_rotation_speed = VariableManager::GetSingletonPtr()->GetAsFloat("LeafRotationSpeed");
	m_current_time = 0.0f;
	m_bob_timer = 2.0f;
	m_y_distance = 3.0f;
	m_move_speed = 2.0f;
	m_up = true;
};

void BobbingComponent::Update(float dt){
	if (!m_start_moving){
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
	}
	else {
		float speed = m_move_speed * dt;
		if (m_up){
			m_node->translate(0.0f, speed, 0.0f);
			m_y_distance -= speed;
			if (m_y_distance <= 0.0f){
				m_up = false;
			}
		}
		else {
			Ogre::Vector3 dir = m_player_node->getPosition() - m_node->getPosition();
			dir.normalise();
			dir *= speed;
			m_node->translate(dir);
		}
	}
	float rot_speed = m_rotation_speed * dt;
	m_node->rotate(Ogre::Quaternion(1.0f, 0.0f, rot_speed, 0.0f));
};

void SyncedTriggerComponent::Init(const Ogre::Vector3& pos, PhysicsEngine* physics_engine, struct TriggerDef* def){
	TriggerComponent::Init(pos, physics_engine, *def);
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

void CameraRaycastCollisionComponent::Init(PhysicsEngine* physics_engine){
	RaycastCollisionComponent::Init(physics_engine);
}

void CameraRaycastCollisionComponent::Shut(){
	m_messenger->Unregister(MSG_CAMERA_RAYCAST_COLLISION_STATIC_ENVIRONMENT, this);
	m_messenger = NULL;
};

void CameraRaycastCollisionComponent::Notify(int type, void* msg){
	switch(type)
	{
		case MSG_CAMERA_RAYCAST_COLLISION_STATIC_ENVIRONMENT:
			m_messenger->Notify(MSG_CAMERA_ENV_COLLISION, NULL);
			break;
		default:
			break;
	};
};

void CameraRaycastCollisionComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CAMERA_RAYCAST_COLLISION_STATIC_ENVIRONMENT, this);
};


void TottController::Notify(int type, void* msg){
	CharacterController::Notify(type, msg);
	/*
	TOTT_STATE& new_state = *static_cast<TOTT_STATE*>(msg);

	//switch(type){
	//case MSG_TOTT_STATE_CHANGE:
		//new_state = static_cast<TOTT_STATE*>(msg);
		m_state = new_state;

		if (m_can_change_state){

		switch(m_state){
		case IDLING:
			m_messenger->Notify(MSG_WAYPOINT_START, NULL);
			if (m_anim_msg.id != m_walk_animation){
				//static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setVisible(false); // so this crashes
				m_anim_msg.id = m_walk_animation;
				m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
			}
			break;
		case CURIOUS:
			m_messenger->Notify(MSG_WAYPOINT_PAUSE, NULL);
			if (m_anim_msg.id != m_react_animation){
				m_anim_msg.id = m_react_animation;
				m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
			}
			break;
		case HAPPY:
			m_messenger->Notify(MSG_WAYPOINT_PAUSE, NULL);
			if (m_anim_msg.id != m_happy_animation){
				m_anim_msg.id = m_happy_animation;
				m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
			}
			break;
		default:
			break;
		};
		m_can_change_state = false;
		}
		*/
		/*
		break;
	default:
		break;
	};*/
};

void TottController::Shut(){
	CharacterController::Shut();
	m_messenger->Register(MSG_TOTT_STATE_CHANGE, this);
};

void TottController::SetMessenger(ComponentMessenger* messenger){
	CharacterController::SetMessenger(messenger);
	m_messenger->Register(MSG_TOTT_STATE_CHANGE, this);
};

void TottController::Init(const Ogre::Vector3& position, PhysicsEngine* physics_engine, const TottDef& def){
	CharacterController::Init(position, physics_engine, def.character_controller);
	
	m_def = def;

	m_idle_animation = def.idle_animation;
	m_walk_animation = def.walk_animation;
	m_run_animation = def.run_animation;
	m_react_animation = def.react_animation;
	m_happy_animation = def.happy_animation;
	
	//def.theme_music = Ogre::String("Hidehog_Theme");

	m_anim_msg.blend = false;
	m_anim_msg.full_body = true;
	m_anim_msg.id = "Idle";
	m_anim_msg.index = 0;
	m_anim_msg.loop = true;
	m_anim_msg.wait = false;
	m_anim_msg.blending_transition = AnimationBlender::BlendThenAnimate;

	m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	m_state = IDLING;

	m_quest_done = false;

	m_state_timer_counter = 0.0f;
	m_state_timer = 1.0f;
	m_can_change_state = true;

	m_music.m_attached = false;
	m_music.m_change_pitch = false;
	m_music.m_change_volume = false;
	m_music.m_name = "Hidehog_Theme";
	m_music.m_node_name = def.node_name;
	m_music.m_pitch = 1.0f;
	m_music.m_volume = 1.0f;
};

void TottController::Idling(){
	//m_messenger->Notify(MSG_MESH_RENDERER_GET_ENTITY
	m_anim_msg.id = m_idle_animation;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	
};
	
void TottController::Curious(){
	m_anim_msg.id = m_react_animation;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
};
	
void TottController::Happy(){
	m_anim_msg.id = m_happy_animation;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	if (!m_quest_done){
		//m_owner->GetGameObjectManager()->RemoveGameObject(m_speech_bubble); //or something like that
	}
};

void TottController::Update(float dt){
	CharacterController::Update(dt);

	m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponentMessenger()->Notify(MSG_MUSIC3D_PLAY, &m_music);

	//m_messenger->Notify(MSG_ANIMATION_PLAY, &m_anim_msg);
	/*
	Ogre::Quaternion rotation;
	Ogre::Vector3 tott_pos = static_cast<NodeComponent*>(m_owner->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition();
	Ogre::Quaternion tott_ori = static_cast<NodeComponent*>(m_owner->GetComponent(COMPONENT_NODE))->GetSceneNode()->getOrientation();

	Ogre::Vector3 player_pos = static_cast<NodeComponent*>(m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition();
	Ogre::Quaternion player_ori = static_cast<NodeComponent*>(m_owner->GetGameObjectManager()->GetGameObject("Player")->GetComponent(COMPONENT_NODE))->GetSceneNode()->getOrientation();

	Ogre::Quaternion quat = Ogre::Quaternion ((Degree(player_ori.getYaw())), Vector3::UNIT_X)*Quaternion ((Degree(player_ori.getPitch())), Vector3::UNIT_Y)*Quaternion ((Degree(player_ori.getRoll())), Vector3::UNIT_Z);
	Ogre::Quaternion quat_tott = Ogre::Quaternion ((Degree(tott_ori.getYaw())), Vector3::UNIT_X)*Quaternion ((Degree(tott_ori.getPitch())), Vector3::UNIT_Y)*Quaternion ((Degree(tott_ori.getRoll())), Vector3::UNIT_Z);

	if (!m_can_change_state){
		m_state_timer_counter += dt;

		if (m_state_timer_counter > m_state_timer){
			m_can_change_state = true;
			m_state_timer_counter = 0.0f;
		}
	}

	switch(m_state){
	case IDLING:
		Idling();
		break;
	case CURIOUS:
		//rotation = Ogre::Quaternion(0.5f, player_pos.x, player_pos.y, player_pos.z);
		//m_messenger->Notify(MSG_SET_OBJECT_ORIENTATION, &quat_tott); //so this made it party hard
		Curious();
		break;
	case HAPPY:
		Happy();
		break;
	default:
		break;
	};
	*/

	//Ogre::SceneNode* test = static_cast<NodeComponent*>(m_owner->GetComponent(COMPONENT_NODE))->GetSceneNode();

	//std::cout << "SB node at " << test->getPosition() << std::endl;

		//if (m_owner->GetId() == "TestSpeechBubble") { std::cout << "Speech Bubble at " << m_node->getPosition() << std::endl; }

	//Ogre::Vector3 bajs = test->getPosition();

	//m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &bajs);

	//static_cast<NodeComponent*>(m_owner->GetGameObjectManager()->GetGameObject("TestSpeechBubble")->GetComponent(COMPONENT_NODE))->GetSceneNode()->setPosition(static_cast<NodeComponent*>(m_owner->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition());
	//static_cast<TriggerComponent*>(m_owner->GetGameObjectManager()->GetGameObject("TestSpeechBubble")->GetComponent(COMPONENT_TRIGGER))->
	//m_owner->GetGameObjectManager()->GetGameObject("TestSpeechBubble")->GetComponentMessenger()->Notify(MSG_SET_OBJECT_POSITION, &static_cast<NodeComponent*>(m_owner->GetComponent(COMPONENT_NODE))->GetSceneNode()->getPosition());
};

void TottController::SimulationStep(btScalar time_step){
	CharacterController::SimulationStep(time_step);
};
