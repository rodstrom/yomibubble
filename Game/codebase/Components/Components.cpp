#include "..\stdafx.h"
#include "Components.h"
#include "ComponentMessenger.h"
#include "..\BtOgrePG.h"
#include "..\BtOgreGP.h"
#include "..\PhysicsEngine.h"
#include "GameObject.h"
#include "..\Managers\InputManager.h"

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->attachObject(m_entity);
}

void MeshRenderComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_NODE_GET_NODE:
			*static_cast<Ogre::SceneNode**>(msg) = m_node;
		break;
	default:
		break;
	}
}

void MeshRenderComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_NODE_GET_NODE, this);
}

void MeshRenderComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_NODE_GET_NODE, this);
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

void AnimationComponent::Update(float dt){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		if (m_animation_states[i] != NULL){
			if (m_animation_states[i]->getEnabled()){
				m_animation_states[i]->addTime(dt);
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
	m_physics_engine->GetDynamicWorld()->removeRigidBody(m_rigidbody);
	m_rigidbody->getMotionState();
	delete m_rigidbody;
	m_rigidbody = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_rigidbody_state;
	m_rigidbody_state = NULL;
	m_messenger->Unregister(MSG_RIGIDBODY_GET_BODY, this);
}

void RigidbodyComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_RIGIDBODY_GET_BODY, this);
}


void CharacterController::Notify(int type, void* msg){
	switch (type){
	case MSG_CHARACTER_CONROLLER_VELOCITY_SET:
		m_velocity = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONROLLER_TURN_SPEED_SET:
		m_turn_speed = *static_cast<float*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_MOVE_FORWARD:
		m_move_forward = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS:
		m_move_backwards = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_MOVE_LEFT:
		m_move_left = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_MOVE_RIGHT:
		m_move_right = *static_cast<bool*>(msg);
		break;
	case MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM:
		m_has_follow_cam = *static_cast<bool*>(msg);
		break;
	default:
		break;
	}
}

void CharacterController::Update(float dt){
	btVector3 walk_direction = btVector3(0.0,0.0,0.0);
	btScalar walk_speed = m_velocity * dt;
	
	if (m_move_left){
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
	}
	if (m_move_backwards || m_move_forward || m_move_left || m_move_right){
		Ogre::SceneNode* node = NULL;
		Ogre::SceneNode* camera_node = NULL;
		m_messenger->Notify(MSG_NODE_GET_NODE, &node);
		m_messenger->Notify(MSG_CAMERA_GET_CAMERA_NODE, &camera_node);
		if (node && camera_node){
			Ogre::Vector3 dir = BtOgre::Convert::toOgre(walk_direction);
			Ogre::Vector3 goal_dir = Ogre::Vector3::ZERO;
			goal_dir += dir.z * camera_node->getOrientation().zAxis();
			goal_dir += dir.x * camera_node->getOrientation().xAxis();
			goal_dir.y = 0.0f;
			goal_dir.normalise();
			Ogre::Quaternion goal = node->getOrientation().zAxis().getRotationTo(goal_dir);
			Ogre::Real yaw_to_goal = goal.getYaw().valueDegrees();
			Ogre::Real yaw_at_speed = yaw_to_goal / Ogre::Math::Abs(yaw_to_goal) * dt * m_turn_speed;

			if (yaw_to_goal < 0) yaw_to_goal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yaw_to_goal, yaw_at_speed));
			else if (yaw_to_goal > 0) yaw_to_goal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yaw_to_goal, yaw_at_speed));
			node->yaw(Ogre::Degree(yaw_to_goal));
			m_controller->setWalkDirection(BtOgre::Convert::toBullet(goal_dir * (float)walk_speed));
		}	
	}
	else{
		m_controller->setWalkDirection(walk_direction * walk_speed);
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
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_FORWARD, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_LEFT, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_MOVE_RIGHT, this);
		m_messenger->Unregister(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM, this);
	}
}

void CharacterController::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_FORWARD, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_BACKWARDS, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_LEFT, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_MOVE_RIGHT, this);
	m_messenger->Register(MSG_CHARACTER_CONTROLLER_HAS_FOLLOW_CAM, this);
}

void CharacterController::Init(PhysicsEngine* physics_engine){
	m_physics_engine = physics_engine;
	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(btVector3(0.0, 10.0, 0.0));
	m_ghost_object = new btPairCachingGhostObject;
	m_ghost_object->setWorldTransform(start_transform);
	btScalar char_height = 6.0;
	btScalar char_width = 1.75;
	m_shape = new btCapsuleShape(char_width, char_height);
	m_ghost_object->setCollisionShape(m_shape);
	m_ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	btScalar step_height = btScalar(0.35);
	m_controller = new btKinematicCharacterController(m_ghost_object, m_shape, step_height);
	m_physics_engine->GetDynamicWorld()->addCollisionObject(m_ghost_object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	m_physics_engine->GetDynamicWorld()->addAction(m_controller);
}

void CameraComponent::Notify(int type, void* msg){
	switch(type){
	case MSG_CAMERA_GET_CAMERA:
		*static_cast<Ogre::Camera**>(msg) = m_camera;
		break;
	case MSG_CAMERA_SET_ACTIVE:
		m_viewport->setCamera(m_camera);
		break;
	}
}

void CameraComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_CAMERA_GET_CAMERA, this);
		m_messenger->Unregister(MSG_CAMERA_SET_ACTIVE, this);
	}
}

void CameraComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CAMERA_GET_CAMERA, this);
	m_messenger->Register(MSG_CAMERA_SET_ACTIVE, this);
}

void CameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	m_scene_manager = scene_manager;
	m_viewport = viewport;
	if (camera_id == Ogre::StringUtil::BLANK){
		m_camera_id = m_owner->GetId() + "Camera";
	}
	else{
		m_camera_id = camera_id;
	}
	m_camera = m_scene_manager->createCamera(m_camera_id);
	if (activate){
		m_viewport->setCamera(m_camera);
	}
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
}

void CameraComponent::Update(float dt){

}

void CameraComponent::ActivateCamera(){
	m_viewport->setCamera(m_camera);
}

void FollowCameraComponent::Notify(int type, void* msg){
	CameraComponent::Notify(type, msg);
	switch (type){
	case MSG_CAMERA_GET_CAMERA_NODE:
		*static_cast<Ogre::SceneNode**>(msg) = m_camera_node;
		break;
	default:
		break;
	}
}

void FollowCameraComponent::Shut(){
	CameraComponent::Shut();
	m_messenger->Unregister(MSG_CAMERA_GET_CAMERA_NODE, this);
}

void FollowCameraComponent::SetMessenger(ComponentMessenger* messenger){
	CameraComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CAMERA_GET_CAMERA_NODE, this);
}

void FollowCameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	CameraComponent::Init(scene_manager, viewport, activate, camera_id);
	m_camera_pivot = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_goal = m_camera_pivot->createChildSceneNode(Ogre::Vector3(0,0,20));
	m_camera_node = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_node->setPosition(m_camera_pivot->getPosition() + m_camera_goal->getPosition());
	m_camera_pivot->setFixedYawAxis(true);
	m_camera_goal->setFixedYawAxis(true);
	m_camera_node->setFixedYawAxis(true);
	m_camera_node->attachObject(m_camera);
}

void FollowCameraComponent::Update(float dt){
	InputManager* input = NULL;
	m_messenger->Notify(MSG_INPUT_MANAGER_GET, &input);
	if (input){
		OIS::MouseState ms = input->GetMouseState();
		UpdateCameraGoal(-0.05f * ms.X.rel, -0.05f * ms.Y.rel, -0.0005f * ms.Z.rel);
	}
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		m_camera_pivot->setPosition(node->getPosition() + Ogre::Vector3::UNIT_Y * 2);
		Ogre::Vector3 goal_offset = m_camera_goal->_getDerivedPosition() - m_camera_node->getPosition();
		m_camera_node->translate(goal_offset * dt * 9.0f);
		m_camera_node->lookAt(m_camera_pivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
	}
}

void FollowCameraComponent::UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom){
	m_camera_pivot->yaw(Ogre::Degree(delta_yaw), Ogre::Node::TS_WORLD);
	if (!(m_pivot_pitch + delta_pitch > 15 && delta_pitch > 0) && 
		!(m_pivot_pitch + delta_pitch < -60 && delta_pitch < 0)){
			m_camera_pivot->pitch(Ogre::Degree(delta_pitch), Ogre::Node::TS_LOCAL);
			m_pivot_pitch += delta_pitch;
	}
	Ogre::Real dist = m_camera_goal->_getDerivedPosition().distance(m_camera_pivot->_getDerivedPosition());
	Ogre::Real dist_change = delta_zoom * dist;

	if (!(dist + dist_change < 8 && dist_change < 0) &&
		!(dist + dist_change > 25 && dist_change > 0)){
			m_camera_goal->translate(0,0, dist_change, Ogre::Node::TS_LOCAL);
	}
}

void Overlay2DComponent::Init(const Ogre::String& p_overlay_name){
	Ogre::OverlayManager* overlay_manager = Ogre::OverlayManager::getSingletonPtr();
	m_id = p_overlay_name;

	m_overlay = overlay_manager->getByName(m_id);	
	m_overlay->show();
}

void Overlay2DComponent::Shut()
{
	if (m_messenger){
		m_messenger->Unregister(MSG_GET_2D_OVERLAY_CONTAINER, this);
	}
	Ogre::OverlayManager& overlay_mrg = Ogre::OverlayManager::getSingleton();
	overlay_mrg.destroy(m_overlay);
}

void Overlay2DComponent::Notify(int type, void* msg){
	switch(type){
	case MSG_GET_2D_OVERLAY_CONTAINER:
		*static_cast<Ogre::OverlayContainer**>(msg) = m_overlay->getChild("MyOverlayElements/TestPanel");
		break;
	}
}

void Overlay2DComponent::SetMessenger(ComponentMessenger* messenger) {
	m_messenger = messenger;
	m_messenger->Register(MSG_GET_2D_OVERLAY_CONTAINER, this);
}

void OverlayCollisionCallback::Init(InputManager* p_input_manager, Ogre::Viewport* p_view_port){
	m_input_manager = p_input_manager;
	m_view_port = p_view_port;

}

void OverlayCollisionCallback::Update(float dt){
	Ogre::OverlayContainer* overlay_container = NULL;

	m_messenger->Notify(MSG_GET_2D_OVERLAY_CONTAINER, &overlay_container);
	if(overlay_container){
		float mouseX = m_input_manager->GetMouseState().X.abs / (float)m_view_port->getActualWidth();
		float mouseY = m_input_manager->GetMouseState().Y.abs / (float)m_view_port->getActualHeight(); 
		std::cout << mouseX << "   " << mouseY << std::endl;  
		float x = overlay_container->getLeft();
		float y = overlay_container->getTop();
		float w = overlay_container->getWidth();
		float h = overlay_container->getHeight();

		if(mouseX <= x) return;
		if(mouseX >= x + w) return;
		if(mouseY <= y) return;
		if(mouseY >= y + h) return;

		std::cout << "collision" << std::endl;
		
	}

}


void OverlayCollisionCallback::Shut(){
}

void OverlayCollisionCallback::Notify(int type, void* msg){
}

void OverlayCollisionCallback::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

