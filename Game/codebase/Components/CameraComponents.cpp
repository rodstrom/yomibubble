#include "stdafx.h"
#include "CameraComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"
#include "GameObjectPrereq.h"
#include "VisualComponents.h"

#include <iostream> //for debugging, wohoo

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
	m_camera = m_scene_manager->getCamera("MainCamera");
	/*if (camera_id == Ogre::StringUtil::BLANK){
		m_camera_id = m_owner->GetId() + "Camera";
	}
	else{
		m_camera_id = camera_id;
	}
	m_camera = m_scene_manager->createCamera(m_camera_id);
	if (activate){
		m_viewport->setCamera(m_camera);
	}
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));*/
}

void CameraComponent::Update(float dt){

}

void CameraComponent::ActivateCamera(){
	//m_viewport->setCamera(m_camera);
}

void FollowCameraComponent::Notify(int type, void* msg){
	CameraDataDef def;
	m_env_collision = false;

	switch (type){
	case MSG_CAMERA_GET_CAMERA_NODE:
		CameraComponent::Notify(type, msg);
		*static_cast<Ogre::SceneNode**>(msg) = m_camera_node;
		break;
	case MSG_DEFAULT_CAMERA_POS:
		def = *static_cast<CameraDataDef*>(msg);
		m_player_direction = def.player_direction;
		break;
	case MSG_FOLLOW_CAMERA_GET_ORIENTATION:
		{
			Ogre::Vector3 goal = Ogre::Vector3::ZERO;
			Ogre::Vector3 dir = *static_cast<Ogre::Vector3*>(msg);
			goal += dir.z * m_camera_node->getOrientation().zAxis();
			goal += dir.x * m_camera_node->getOrientation().xAxis();
			goal.y = 0.0f;
			*static_cast<Ogre::Vector3*>(msg) = goal;	
		}
		break;
	case MSG_CAMERA_ENV_COLLISION:
		m_env_collision = true;
		break;
	default:
		break;
	}
}

void FollowCameraComponent::Shut(){
	CameraComponent::Shut();
	m_camera_node->detachAllObjects();
	m_scene_manager->destroySceneNode(m_camera_pivot);
	m_scene_manager->destroySceneNode(m_camera_node);
	m_messenger->Unregister(MSG_CAMERA_GET_CAMERA_NODE, this);
	m_messenger->Unregister(MSG_DEFAULT_CAMERA_POS, this);
	m_messenger->Unregister(MSG_FOLLOW_CAMERA_GET_ORIENTATION, this);
	m_messenger->Unregister(MSG_CAMERA_ENV_COLLISION, this);
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void FollowCameraComponent::SetMessenger(ComponentMessenger* messenger){
	CameraComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CAMERA_GET_CAMERA_NODE, this);
	m_messenger->Register(MSG_DEFAULT_CAMERA_POS, this);
	m_messenger->Register(MSG_FOLLOW_CAMERA_GET_ORIENTATION, this);
	m_messenger->Register(MSG_CAMERA_ENV_COLLISION, this);
}

void FollowCameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	CameraComponent::Init(scene_manager, viewport, activate, camera_id);
	m_camera_pivot = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_goal = m_camera_pivot->createChildSceneNode(Ogre::Vector3(0,0,2));
	m_camera_node = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_node->setPosition(m_camera_pivot->getPosition() + m_camera_goal->getPosition());
	m_camera_pivot->setFixedYawAxis(true);
	m_camera_goal->setFixedYawAxis(true);
	m_camera_node->setFixedYawAxis(true);
	m_camera_node->attachObject(m_camera);
	m_getting_input = false;
	m_check_cam = false;
	m_env_collision = false;
	m_player_direction = Ogre::Vector3::ZERO;
	m_camera_goal->setPosition(0,0,12);
	m_default_distance = 12.0;
	m_default_pitch = -25.7;
	m_camera_pivot->pitch(Ogre::Degree(m_default_pitch), Ogre::Node::TS_LOCAL);

	m_left_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_left_ray.length = btVector3(-0.2, 0, 0);
	m_right_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_right_ray.length = btVector3(1, 0, 0);
	m_top_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z); 
	m_top_ray.length = btVector3(0, 1, 0);
	m_bot_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_bot_ray.length = btVector3(0, -1, 0);

	m_env_coll_left = false;
	m_env_coll_right = false;
	m_env_coll_up = false;
	m_env_coll_down = false;
}

void FollowCameraComponent::Update(float dt){
	//QueryRaycast();
	InputManager* input = NULL;
	m_messenger->Notify(MSG_INPUT_MANAGER_GET, &input);
	if (input){
		CameraAxis axis = input->GetCameraAxis();
		//UpdateCameraGoal(-0.1f * axis.x, -0.1f * axis.y, -0.0005f * axis.z);
		
		UpdateCameraGoal(axis.x * m_movement_speed, axis.y * m_movement_speed, -0.0005f * axis.z);
	}
	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		m_camera_pivot->setPosition(node->getPosition() + Ogre::Vector3::UNIT_Y * 0.2f);
		Ogre::Vector3 goal_offset = m_camera_goal->_getDerivedPosition() - m_camera_node->getPosition();
		m_camera_node->translate(goal_offset * dt * 2.0f);
		m_camera_node->lookAt(m_camera_pivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
		//m_default_position = Ogre::Vector3(node->getPosition().x + 0.002, node->getPosition().y + 0.002, node->getPosition().z + 0.002);
		//m_camera_goal->setPosition(m_default_position);
	}

	//m_trigger->GetCollisionDef

	//std::cout << "kiss " << m_camera->getDerivedPosition().x << std::endl;
	m_camera->getDerivedRight(); //x-led
	m_camera->getDerivedUp(); //y-led
	/*
	m_camera->getViewport()->getActualLeft();
	m_camera->getViewport()->getActualTop();
	m_camera->getViewport()->getActualHeight();
	m_camera->getViewport()->getActualWidth();
	*/
	/*
	m_camera->getViewport()->getLeft();
	m_camera->getViewport()->getTop();
	*/

	int x_distance_from_cam = (m_camera->getViewport()->getWidth() * 0.5); //so this did not work then, fucktard values
	int y_distance_from_cam = (m_camera->getViewport()->getHeight() * 0.5);

	int testbajs = m_camera->getViewport()->getHeight();
	testbajs;

	m_node->setPosition(m_camera->getDerivedPosition().x, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);
	m_left_ray.origin = btVector3(m_camera->getDerivedPosition().x, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);
	m_left_ray.length = btVector3(m_camera->getDerivedPosition().x - 1.5, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);

	m_right_ray.origin = btVector3(m_camera->getDerivedPosition().x, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);
	m_right_ray.length = btVector3(m_camera->getDerivedPosition().x + 1.5, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);

	m_bot_ray.origin = btVector3(m_camera->getDerivedPosition().x, m_camera->getDerivedPosition().y, m_camera->getDerivedPosition().z);
	m_bot_ray.length = btVector3(m_camera->getDerivedPosition().x, m_camera->getDerivedPosition().y - 1.5, m_camera->getDerivedPosition().z);

	//std::cout << "Node Pos: " << m_node->convertLocalToWorldPosition(m_node->getPosition()) << std::endl;
	//std::cout << "Node Pos: " << m_node->getPosition() << std::endl; //so this gives local space
	//std::cout << "ViewPort left: " << m_camera->getViewport()->getLeft() << std::endl;
	//std::cout << "ViewPort up: " << m_camera->getViewport()->getTop() << std::endl;
	//std::cout << "Viewport derived pos: " << m_camera->getDerivedPosition() << std::endl;

	//std::cout << "bajs" << m_camera->getBoundingBox().getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP).x << std::endl;


	
	//Assume world->stepSimulation or world->performDiscreteCollisionDetection has been called
	

			
	/*
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
	
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
	*/
}

class BajsCallBack : btCollisionWorld::ContactResultCallback {
public:
	BajsCallBack() : btCollisionWorld::ContactResultCallback() {}
	~BajsCallBack(){}

private:

};

void FollowCameraComponent::SimulationStep(btScalar time_step){
	//btPersistentManifold* contactManifold = m_physics_engine->GetDynamicWorld()->getDispatcher()->getManifoldByIndexInternal(0);
	//btCollisionObject* obA = static_cast<btCollisionObject*>(static_cast<TerrainComponent*>(m_owner->GetGameObject("Terrain")->GetComponent(COMPONENT_TERRAIN))->GetRigidBody());
	//btCollisionObject* obB = static_cast<btCollisionObject*>(static_cast<CharacterController*>(m_owner->GetComponent(COMPONENT_CHARACTER_CONTROLLER))->GetRigidbody());
	
	btCollisionObject* obA = static_cast<btCollisionObject*>(static_cast<TerrainComponent*>(m_owner->GetGameObject("Terrain")->GetComponent(COMPONENT_TERRAIN))->GetRigidBody());
	btCollisionObject* obB = static_cast<btCollisionObject*>(static_cast<TriggerComponent*>(m_owner->GetGameObject("CameraTrig")->GetComponent(COMPONENT_TRIGGER))->GetRigidbody());
	/*
	btManifoldPoint& pt = contactManifold->getContactPoint(0);
			if (pt.getDistance()<0.f)
			{
				std::cout << "Collision!!\n";
			}
			*/
	//std::cout << "Cam pos: " << BtOgre::Convert::toOgre(obB->getWorldTransform().getOrigin()) << std::endl;

	bool bajsmacka = obB->checkCollideWith(obA);

	//btCollisionWorld::contactPairTest(
	/*
	BajsCallBack test = BajsCallBack();
	m_physics_engine->GetDynamicWorld()->contactPairTest(obA, obB, test);
	*/
//	static_cast<TerrainComponent*>(m_owner->GetGameObject("Terrain")->GetComponent(COMPONENT_TERRAIN))->

	//btCollisionWorld::contactPairTest(obA, obB, test);

	


	//bajsmacka = obA->

	if (bajsmacka)
	{
		//std::cout << "Terrain coll\n";
		//m_env_collision = true;
	}
	else
	{
		//std::cout << "NOT Terrain coll\n";
	//	m_env_collision = false;
	}
};

void FollowCameraComponent::UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom){
	if (delta_yaw != 0.0f
			|| delta_pitch != 0.0f
			|| delta_zoom != 0.0f){
				m_getting_input = true;
				m_check_cam = false;
		}
		else{
			m_getting_input = false;
			m_check_cam = true;
		}
	
	if (!m_env_collision){
		

	//if (!m_env_collision){

		if (!m_getting_input){
			m_camera_goal->setPosition(0, 0, m_default_distance);
			m_pivot_pitch = m_default_pitch;
			m_camera_pivot->yaw(Ogre::Degree(m_player_direction.x * -2.15), Ogre::Node::TS_WORLD);
		}
	
			if (m_getting_input){
				m_camera_pivot->yaw(Ogre::Degree(delta_yaw), Ogre::Node::TS_WORLD);
			}
			if (!(m_pivot_pitch + delta_pitch > 15 && delta_pitch > 0) && 
				!(m_pivot_pitch + delta_pitch < -60 && delta_pitch < 0)
				&& m_getting_input){
					m_camera_pivot->pitch(Ogre::Degree(delta_pitch), Ogre::Node::TS_LOCAL);
					m_pivot_pitch += delta_pitch;
					m_default_pitch = m_pivot_pitch;
			}
			if (m_getting_input){
				Ogre::Real dist = m_camera_goal->_getDerivedPosition().distance(m_camera_pivot->_getDerivedPosition());
				Ogre::Real dist_change = delta_zoom * dist;
				m_default_distance += (delta_zoom * dist);

				if (!(dist + dist_change < 2 && dist_change < 0) &&
				!(dist + dist_change > 25 && dist_change > 0)){
					m_camera_goal->translate(0,0, dist_change, Ogre::Node::TS_LOCAL);
				}
			}
		//}
	}
	else { //if environment collision
		
		//magical repositioning goes here (check which ray that hits)
		if (m_env_coll_left){
			m_camera_pivot->yaw(Ogre::Degree(2.15), Ogre::Node::TS_WORLD);
		}
		else if (m_env_coll_right){
			m_camera_pivot->yaw(Ogre::Degree(-2.15), Ogre::Node::TS_WORLD);
		}
		else if (m_env_coll_up){
			//magic
		}
		else if (m_env_coll_down){
			m_camera_pivot->pitch(Ogre::Degree(-2.15), Ogre::Node::TS_WORLD);
		}
	}
	std::cout << "Pitch degrees: " << m_pivot_pitch << std::endl;
	//std::cout << "Camera goal: " << m_camera_goal->getPosition() << std::endl; //0,0,12
}

void FollowCameraComponent::QueryRaycast(){
	if (!m_check_cam) { return; }

	////////////////////  LEFT RAY  ////////////////////////

	btCollisionWorld::ClosestRayResultCallback call_back = btCollisionWorld::ClosestRayResultCallback(m_left_ray.origin, m_left_ray.length);
	m_physics_engine->GetDynamicWorld()->rayTest(m_left_ray.origin, m_left_ray.length, call_back);
	
	if (call_back.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(call_back.m_collisionObject->getUserPointer());

		if (def.flag == COLLISION_FLAG_STATIC){
			std::cout << "Hit terrain left lol!\n";
			m_env_coll_left = true;
		}
	}
	else{
		m_env_coll_left = false;
	}

	////////////////////  RIGHT RAY  ////////////////////////

	call_back = btCollisionWorld::ClosestRayResultCallback(m_right_ray.origin, m_right_ray.length);
	m_physics_engine->GetDynamicWorld()->rayTest(m_right_ray.origin, m_right_ray.length, call_back);
	
	if (call_back.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(call_back.m_collisionObject->getUserPointer());

		if (def.flag == COLLISION_FLAG_STATIC){
			std::cout << "Hit terrain right lol!\n";
			m_env_coll_right = true;
		}
	}
	else{
		m_env_coll_right = false;
	}

	////////////////////  UP RAY  ////////////////////////

	//meh, kinda don't use it

	////////////////////  BOT RAY  ////////////////////////

	call_back = btCollisionWorld::ClosestRayResultCallback(m_bot_ray.origin, m_bot_ray.length);
	m_physics_engine->GetDynamicWorld()->rayTest(m_bot_ray.origin, m_bot_ray.length, call_back);
	
	if (call_back.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(call_back.m_collisionObject->getUserPointer());

		if (def.flag == COLLISION_FLAG_STATIC){
			std::cout << "Hit terrain bottom lol!\n";
			m_env_coll_down = true;
		}
	}
	else{
		m_env_coll_down = false;
	}

	if (m_env_coll_down
		|| m_env_coll_left
		|| m_env_coll_right){
			m_env_collision = true;
	}
	else{
		m_env_collision = false;
	}

	/*
	if (call_back.hasHit()){
		CollisionDef& def = *static_cast<CollisionDef*>(call_back.m_collisionObject->getUserPointer());

		if (def.flag == COLLISION_FLAG_STATIC){
			std::cout << "Hit terrain left lol!\n";
			m_env_collision = true;
		}
	}
	else{
		m_env_collision = false;
	}	
	*/
}

void CameraCollisionComponent::Notify(int type, void* msg){
};

void CameraCollisionComponent::Shut(){
	m_messenger->Unregister(MSG_CAMERA_COLL_UPDATE, this);
};

void CameraCollisionComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CAMERA_COLL_UPDATE, this);
};

void CameraCollisionComponent::Init(GameObject* player){
	m_player = player;
};

void CameraCollisionComponent::Update(float dt){
	Ogre::Vector3 new_position = static_cast<FollowCameraComponent*>(m_player->GetComponent(COMPONENT_FOLLOW_CAMERA))->m_node->getPosition();
	//new_position;
	m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &new_position);

	//m_player->GetComponentMessenger()->Notify(MSG_CAMERA_COLL_UPDATE, NULL); //så skickar man med en position typ

	//trigger : MSG_SET_OBJECT_POSITION
};
