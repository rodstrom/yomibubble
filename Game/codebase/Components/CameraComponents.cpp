#include "stdafx.h"
#include "CameraComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "GameObject.h"
#include "GameObjectPrereq.h"
#include "VisualComponents.h"
#include "..\Managers\VariableManager.h"

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
	case MSG_ON_GROUND:
		m_on_ground = *static_cast<bool*>(msg);
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
	m_messenger->Unregister(MSG_ON_GROUND, this);
	m_physics_engine->RemoveObjectSimulationStep(this);
}

void FollowCameraComponent::SetMessenger(ComponentMessenger* messenger){
	CameraComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_CAMERA_GET_CAMERA_NODE, this);
	m_messenger->Register(MSG_DEFAULT_CAMERA_POS, this);
	m_messenger->Register(MSG_FOLLOW_CAMERA_GET_ORIENTATION, this);
	m_messenger->Register(MSG_CAMERA_ENV_COLLISION, this);
	m_messenger->Register(MSG_ON_GROUND, this);
}

void FollowCameraComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate, const Ogre::String& camera_id){
	CameraComponent::Init(scene_manager, viewport, activate, camera_id);
	m_camera_pivot = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_goal = m_camera_pivot->createChildSceneNode(Ogre::Vector3(0,0,2));
	m_camera_node = m_camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	m_camera_node->setPosition(m_camera_goal->_getDerivedPosition());
	m_camera_pivot->setFixedYawAxis(true);
	m_camera_goal->setFixedYawAxis(true);
	m_camera_node->setFixedYawAxis(true);
	m_camera_node->attachObject(m_camera);
	m_getting_input = false;
	m_check_cam = false;
	m_env_collision = false;
	m_player_direction = Ogre::Vector3::ZERO;
	
	m_camera_pivot->setPosition(Ogre::Real(158.070892),Ogre::Real(72.4587402),Ogre::Real(252.214386));
	m_camera_node->setPosition(Ogre::Real(160.070892),Ogre::Real(73.4587402),Ogre::Real(255.214386));

	//m_camera->lookAt(Ogre::Vector3(145.872f,73.6166f,244.42f));
	//m_camera->rotate(Ogre::Vector3(0.0491129,0.92081,0.123328), Ogre::Degree(-0.366698));

	m_camera_pivot->yaw(Ogre::Degree(200), Ogre::Node::TS_WORLD);

	m_left_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_left_ray.length = btVector3(-0.2f, 0, 0);
	m_right_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_right_ray.length = btVector3(1, 0, 0);
	m_top_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z); 
	m_top_ray.length = btVector3(0, 1, 0);
	m_bot_ray.origin = btVector3(m_bot_ray.node->getPosition().x, m_bot_ray.node->getPosition().y, m_bot_ray.node->getPosition().z);
	m_bot_ray.length = btVector3(0, -1, 0);

	m_on_ground = true;

	bool inv_contr = VariableManager::GetSingletonPtr()->GetAsInt("Camera_Inverted_Controller_Sticks");
	if (inv_contr == 0){
		m_inverted_controller = false;
	}
	else{
		m_inverted_controller = true;
	}

	//m_env_coll_Xp = false;
	//m_env_coll_Xn = false;
	//m_env_coll_Yp = false;
	//m_env_coll_Yn = false;
	//m_env_coll_Zp = false;
	//m_env_coll_Zn = false;

	//m_min_pitch_angle = 140;
	//m_max_pitch_angle = 160;

	m_terrain_group = NULL;
	//GameObject* terrain = static_cast<GameObject*>(m_owner->GetGameObject("Terrain"));
	//TerrainComponent* terrain_component = static_cast<TerrainComponent*>(terrain->GetComponent(EComponentType::COMPONENT_TERRAIN));
	//ArtifexLoader* terrain_artifex = terrain_component->GetArtifex();
	//m_terrain_group = terrain_artifex->mTerrainGroup;

}

void FollowCameraComponent::Update(float dt){
	std::cout << "DT: " << dt << std::endl;
	//m_camera->lookAt(Ogre::Vector3(150.872f,75.6166f,244.42f));
	//m_camera->rotate(Ogre::Vector3(0.0491129,0.92081,0.123328), Ogre::Radian(-0.366698));
	//m_camera->setOrientation(Ogre::Quaternion(0.0491129,0.92081,0.123328,-0.366698));
	//m_camera->setOrientationMode(Ogre::OrientationMode::OR_DEGREE_180); //this crashes :(

	//QueryRaycast();

	//if(!QueryRaycast()){
		InputManager* input = NULL;
		m_messenger->Notify(MSG_INPUT_MANAGER_GET, &input);
		if (input){
			CameraAxis axis = input->GetCameraAxis();
			//UpdateCameraGoal(-0.1f * axis.x, -0.1f * axis.y, -0.0005f * axis.z);
			Ogre::Real speed = 20;
			UpdateCameraGoal(axis.x * m_movement_speed * dt * speed, axis.y * m_movement_speed * dt * speed, -0.0005f * axis.z * dt * speed);
		}
	//}
		
	//QueryRaycast();

	Ogre::SceneNode* node = NULL;
	m_messenger->Notify(MSG_NODE_GET_NODE, &node);
	if (node){
		//Ogre::Vector3 old_position = m_camera_node->getPosition();
		
		//if(m_on_ground){
		//	m_camera_pivot->setPosition(node->getPosition() + Ogre::Vector3::UNIT_Y);
		//}
		//else {
		Ogre::Vector3 delta_position = m_camera_pivot->getPosition() - node->getPosition();
		delta_position.y--;
		m_camera_pivot->setPosition(m_camera_pivot->getPosition().x - delta_position.x * dt * 10.0f, 
			m_camera_pivot->getPosition().y - delta_position.y * dt * 3.0f, 
			m_camera_pivot->getPosition().z - delta_position.z * dt * 10.0f);
		//m_camera_pivot->translate(m_camera_pivot->getPosition() - node->getPosition() * dt * 5.0f);
		//}
		Ogre::Vector3 goal_offset = m_camera_goal->_getDerivedPosition() - m_camera_node->getPosition();
		m_camera_node->translate(goal_offset * dt * 10.0f);
		m_camera_node->lookAt(m_camera_pivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
		
		//Ogre::Vector3 new_position = m_camera_node->getPosition();
		
		//QueryRaycast();

		//if(!m_env_coll_Xp) new_position.x = old_position.x;
		//if(!m_env_coll_Xn) new_position.x = old_position.x;
		//if(m_env_coll_Yp) new_position.y = old_position.y;
		////if(!m_env_coll_Yn) new_position.y = old_position.y;
		//if(!m_env_coll_Zp) new_position.z = old_position.z;
		//if(!m_env_coll_Zn) new_position.z = old_position.z;
		//m_camera_node->setPosition(new_position);
		
		//if(m_env_coll_Yp) old_position.y += 0.1;
		//if(m_env_coll_Yp) m_camera_node->setPosition(old_position);

		//m_default_position = Ogre::Vector3(node->getPosition().x + 0.002, node->getPosition().y + 0.002, node->getPosition().z + 0.002);
		//m_camera_goal->setPosition(m_default_position);
	}

	//m_trigger->GetCollisionDef
	//QueryRaycast(); //bugfix?

	QueryRaycast();

	Ogre::Vector3 goal_pos = m_camera_goal->getPosition();
	if(goal_pos.z < Ogre::Real(2)) m_camera_goal->setPosition(goal_pos.x, goal_pos.y, Ogre::Real(2));
	//if(goal_pos.z > Ogre::Real(15)) m_camera_goal->setPosition(goal_pos.x, goal_pos.y, Ogre::Real(15));
	if(m_default_distance > 15.0f) m_default_distance = 15.0f;
	goal_pos = m_camera_goal->_getDerivedPosition();

	//if(goal_pos.y - m_camera_pivot->getPosition().y < 0) {		//bugfix, camera n ot clipping ground
	//	//m_camera_pivot->pitch(Ogre::Radian(-m_camera_pivot->getOrientation().getPitch()));
	//	m_camera_goal->_setDerivedPosition(Ogre::Vector3(goal_pos.x, m_camera_pivot->getPosition().y, goal_pos.z));
	//}

	//goal_pos = m_camera_goal->getPosition();
	//if(m_camera_goal->_getDerivedPosition().y - m_camera_pivot->getPosition().y > Ogre::Math::Tan((Ogre::Radian(0.5f))) * goal_pos.z ) {
	//	//std::cout << "tan(0.5r) * cam.z: " << Ogre::Math::Tan((Ogre::Radian(0.5f))) * goal_pos.z << "\n";
	//	Ogre::Real new_y = (Ogre::Math::Tan(Ogre::Radian(0.5f)) * goal_pos.z) + m_camera_pivot->getPosition().y;
	//	goal_pos = m_camera_goal->_getDerivedPosition();
	//	m_camera_goal->_setDerivedPosition(Ogre::Vector3(goal_pos.x, new_y, goal_pos.z));
	//}
	//
	//std::cout << "Pitch: " << m_camera_pivot->getOrientation().getPitch() << "\n";
	//std::cout << " Roll: " << m_camera_pivot->getOrientation().getRoll() << "\n";
	//std::cout << "  Yaw: " << m_camera_pivot->getOrientation().getYaw() << "\n";

	if(m_camera_pivot->getPosition().distance(m_camera_goal->_getDerivedPosition()) < Ogre::Real(2)) {
		Ogre::Ray distance(m_camera_pivot->getPosition(), m_camera_goal->_getDerivedPosition() - m_camera_pivot->getPosition());
		m_camera_goal->_setDerivedPosition(distance.getPoint(Ogre::Real(2)));
	}
	//else if(m_camera_pivot->getPosition().distance(m_camera_goal->_getDerivedPosition()) > Ogre::Real(15)) {
	//	Ogre::Ray distance(m_camera_pivot->getPosition(), m_camera_goal->_getDerivedPosition() - m_camera_pivot->getPosition());
	//	m_camera_goal->_setDerivedPosition(distance.getPoint(Ogre::Real(15)));
	//}

}

void FollowCameraComponent::SimulationStep(btScalar time_step){

};

void FollowCameraComponent::SetCustomVariables(int inverted_camera, float camera_zoom_speed, float stick_rotation_acceleration, float change_angle_after_player, float default_distance, float default_pitch){
	if (inverted_camera == 1) { m_inverted_controller = true; }
	else { m_inverted_controller = false; }
	m_camera_zoom_speed = camera_zoom_speed;
	m_camera_stick_rotation_acceleration = stick_rotation_acceleration;
	m_camera_change_angle_after_player = change_angle_after_player;
	m_default_distance = default_distance;
	m_default_pitch = default_pitch;

	m_camera_goal->setPosition(0, m_default_distance * 0.8, m_default_distance);
	m_camera_pivot->pitch(Ogre::Degree(m_default_pitch), Ogre::Node::TS_LOCAL);
};

void FollowCameraComponent::UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom){
//	std::cout << "Pitch degrees: " << m_pivot_pitch << std::endl;
	
	if (delta_yaw != 0.0f
			|| delta_pitch != 0.0f
			|| delta_zoom != 0.0f){
		m_getting_input = true;
		//m_check_cam = false;
	}
	else{
		m_getting_input = false;
		m_check_cam = true;
	}
	
	//if (!m_env_collision && !m_getting_input){
	//	return;
	//}

//if (!m_env_collision){

	if (!m_getting_input){
		//m_camera_goal->setPosition(0, m_default_distance * 0.2, m_default_distance);		//now set in queryraycast
		m_pivot_pitch = m_default_pitch;
		m_camera_pivot->yaw(Ogre::Degree(m_player_direction.x * -2.15 * m_camera_change_angle_after_player), Ogre::Node::TS_WORLD);
	}
	
	if (m_getting_input){
		if (!m_inverted_controller) { m_camera_pivot->yaw(Ogre::Degree(delta_yaw * m_camera_stick_rotation_acceleration), Ogre::Node::TS_WORLD); }
		else { m_camera_pivot->yaw(Ogre::Degree(-delta_yaw* m_camera_stick_rotation_acceleration), Ogre::Node::TS_WORLD); }
	}

	if (!(m_pivot_pitch + delta_pitch > -10 && delta_pitch > 0) && 
		!(m_pivot_pitch + delta_pitch < -40 && delta_pitch < 0)
		&& m_getting_input){
			if (!m_inverted_controller) { m_camera_pivot->pitch(Ogre::Degree(delta_pitch * m_camera_stick_rotation_acceleration), Ogre::Node::TS_LOCAL); }
			else { m_camera_pivot->pitch(Ogre::Degree(-delta_pitch * m_camera_stick_rotation_acceleration), Ogre::Node::TS_LOCAL); }
			m_pivot_pitch += delta_pitch;
			//cout << m_pivot_pitch << "\n";
			m_default_pitch = m_pivot_pitch;
	}
	//if (m_getting_input){
		Ogre::Real dist = m_camera_goal->_getDerivedPosition().distance(m_camera_pivot->_getDerivedPosition());
		Ogre::Real dist_change = (delta_zoom * dist * m_camera_zoom_speed);
		m_default_distance += (delta_zoom * dist * m_camera_zoom_speed);

		if (!(dist + dist_change < 2 && dist_change < 0) &&
		!(dist + dist_change > 25 && dist_change > 0)){
			m_camera_goal->translate(0, dist_change * 0.2, dist_change, Ogre::Node::TS_LOCAL);
		}
	//}
	
}

bool FollowCameraComponent::QueryRaycast(){
	//if(!m_check_cam) return false;
	//cout << "QueryRaycast() - " << m_camera_goal->_getDerivedPosition() << "\n";
	m_env_collision = false;

	if(m_terrain_group == NULL){
		GameObject* terrain = static_cast<GameObject*>(m_owner->GetGameObject("Terrain"));
		TerrainComponent* terrain_component = static_cast<TerrainComponent*>(terrain->GetComponent(COMPONENT_TERRAIN));
		ArtifexLoader* terrain_artifex = terrain_component->GetArtifex();
		m_terrain_group = terrain_artifex->mTerrainGroup;
	}
	
	//Ogre::Vector3 cam_pos = m_camera_node->getPosition();
	Ogre::Vector3 cam_pos = m_camera_goal->_getDerivedPosition();
	Ogre::Real min_distance = m_default_distance;
	Ogre::Real max_y = m_camera_goal->getPosition().z * 0.2;
	cam_pos.y -= Ogre::Real(0.9);
	Ogre::Ray camera_ray(cam_pos, Ogre::Vector3::UNIT_Y);
	Ogre::TerrainGroup::RayResult collision = m_terrain_group->rayIntersects(camera_ray);
	//Ogre::Real new_y = 0;
	if(collision.hit){
		m_env_collision = true;
		m_camera_goal->_setDerivedPosition(collision.position + Ogre::Vector3::UNIT_Y * Ogre::Real(0.9));
	}

	Ogre::Vector3 pivot_pos = m_camera_pivot->getPosition();
	pivot_pos.y += Ogre::Real(0.5);
	Ogre::Real hit_distance = collision.position.distance(cam_pos);
	cam_pos = m_camera_goal->_getDerivedPosition();
	cam_pos.y -= max_y;
	camera_ray.setOrigin(cam_pos);
	camera_ray.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
	collision = m_terrain_group->rayIntersects(camera_ray);
	if(hit_distance < max_y * Ogre::Real(1.25)){
		m_env_collision = true;
		m_camera_goal->_setDerivedPosition(collision.position + Ogre::Vector3::UNIT_Y * max_y * Ogre::Real(0.8));
	}

	for(int i = -1; i < 2; i++){
		cam_pos = m_camera_goal->_getDerivedPosition();
		cam_pos.x += i;
		cam_pos.y -= max_y;
		camera_ray.setOrigin(pivot_pos);
		camera_ray.setDirection(cam_pos - pivot_pos);
		Ogre::TerrainGroup::RayResult collision = m_terrain_group->rayIntersects(camera_ray);
		if(collision.hit){
			Ogre::Real hit_distance = collision.position.distance(pivot_pos);
			if(hit_distance < m_default_distance * 1.5){
				m_env_collision = true;
				//std::cout << "Collision: " << collision.position << "\n" << "Ray direction: " << camera_ray.getDirection() << "\n" << "Distance: " << hit_distance << "\n";
				if(hit_distance * 0.66666666 < min_distance) min_distance = hit_distance * 0.66666666;
				//m_camera_goal->setPosition(0, 0 /*hit_distance * 0.8*/, hit_distance * 0.8);
				//m_camera_node->setPosition(collision.position);
			}
		}
	}

	m_camera_goal->setPosition(0, 0, min_distance);


	return m_env_collision;
}

//void CameraCollisionComponent::Notify(int type, void* msg){
//};
//
//void CameraCollisionComponent::Shut(){
//	m_messenger->Unregister(MSG_CAMERA_COLL_UPDATE, this);
//};
//
//void CameraCollisionComponent::SetMessenger(ComponentMessenger* messenger){
//	m_messenger = messenger;
//	m_messenger->Register(MSG_CAMERA_COLL_UPDATE, this);
//};
//
//void CameraCollisionComponent::Init(GameObject* player){
//	m_player = player;
//};
//
//void CameraCollisionComponent::Update(float dt){
//	//Ogre::Vector3 new_position = static_cast<FollowCameraComponent*>(m_player->GetComponent(COMPONENT_FOLLOW_CAMERA))->m_node->getPosition();
//	//new_position;
//	//m_messenger->Notify(MSG_RIGIDBODY_POSITION_SET, &new_position);
//
//	//m_player->GetComponentMessenger()->Notify(MSG_CAMERA_COLL_UPDATE, NULL); //så skickar man med en position typ
//
//	//trigger : MSG_SET_OBJECT_POSITION
//};
