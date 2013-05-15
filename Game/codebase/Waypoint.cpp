#include "stdafx.h"
#include "Waypoint.h"

WayPoint::WayPoint(){

};

WayPoint::~WayPoint(){
	m_node = NULL;
};

void WayPoint::Init(Ogre::SceneNode* node){
	m_node = node;
	m_direction = Ogre::Vector3::ZERO;
	m_destination = Ogre::Vector3::ZERO;

	m_follow_node = NULL;
	m_game_object = NULL;
	m_old_destination = Ogre::Vector3::ZERO;
	m_follow_node_moving = false;
	m_loop_waypoints = false;
	m_current_waypoint = 0;
};

bool WayPoint::NextLocation(){
	if ( m_walk_list.empty() ) { 
		m_direction = Ogre::Vector3::ZERO; return false; 
	}
	
	m_destination = m_walk_list.front();  // this gets the front of the deque
	if (m_loop_waypoints) m_walk_list.push_back(m_destination); //if looping keep the list rotating so that it never ends
	m_walk_list.pop_front();             // this removes the front of the deque
 
	m_direction = m_destination - m_node->getPosition();

	return true;
};

void WayPoint::AddWayPoint(Ogre::Vector3 way_point){
	m_walk_list.push_back(way_point);
}

void WayPoint::AddWayPoint(Ogre::SceneNode* scene_node){
	m_follow_node = scene_node;
	m_current_waypoint = -1;
}


void WayPoint::setLoopable(const Ogre::String& loop){
	m_loop_waypoints = (loop == "true");
	
}

bool WayPoint::withinDistance(float meters){
	if (m_node->getPosition().x >= m_destination.x - meters
		&& m_node->getPosition().x <= m_destination.x + meters)
	{
		if (m_node->getPosition().z >= m_destination.z - meters
		&& m_node->getPosition().z <= m_destination.z + meters)
		{
			return true;
		}
	}

	return false;
}

void WayPoint::Update(float dt){
		if(m_follow_node != NULL) {
		m_destination = m_follow_node->getPosition();
		m_direction = m_destination - m_node->getPosition();

		m_follow_node_moving = (m_destination != m_old_destination);
		m_old_destination = m_destination;
	}

	if (m_direction == Ogre::Vector3::ZERO) {
        NextLocation();
    }
	else {
		//do naaathing
		//std::cout << "Deque empty: " << m_walk_list.empty() << std::endl;
	}

	if (withinDistance(5.0f)){
		if(m_follow_node != NULL) {
			//do nothing for now
		}
		else {
			std::cout << "Hit" << std::endl;
			m_direction = Ogre::Vector3::ZERO;
		}
	}
};