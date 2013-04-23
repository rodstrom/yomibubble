#include "stdafx.h"
#include "Waypoint.h"

WayPoint::WayPoint(){

};

WayPoint::~WayPoint(){
	m_node = NULL;
};

void WayPoint::Init(Ogre::SceneNode* node, float walk_speed){
	m_node = node;
	m_walk_speed = walk_speed;
	m_direction = Ogre::Vector3::ZERO;
	m_destination = Ogre::Vector3::ZERO;

	m_follow_node = NULL;
	m_old_destination = Ogre::Vector3::ZERO;

	//AddWayPoint(Ogre::Vector3(25.0f, -10.0f, 0.0f));
	//AddWayPoint(Ogre::Vector3(1.0f, -10.0f, 21.0f));
};

bool WayPoint::NextLocation(){
	if (m_walk_list.empty())
	{ m_direction = Ogre::Vector3::ZERO; return false; }
	
	m_destination = m_walk_list.front();  // this gets the front of the deque
    m_walk_list.pop_front();             // this removes the front of the deque
 
	m_direction = m_destination - m_node->getPosition();

	return true;
};

void WayPoint::AddWayPoint(Ogre::Vector3 way_point){
	m_walk_list.push_back(way_point);
}

void WayPoint::AddWayPoint(Ogre::SceneNode* scene_node){
	m_follow_node = scene_node;
	m_destination = m_follow_node->getPosition();
	m_direction = m_destination - m_node->getPosition();
	m_walk_list.push_front(m_follow_node->getPosition());
}

float WayPoint::getSpeed(){
	if(m_follow_node_moving){
		int i;
	}
	if(m_follow_node != NULL) {
		if(withinDistance(1.0f)) return 0.0f;
		else if(m_follow_node_moving) return m_walk_speed;
		else if(withinDistance(3.0f)) return m_walk_speed * 0.3f;
		else if(withinDistance(5.0f)) return m_walk_speed * 0.8f;
		else if(withinDistance(10.0f)) return m_walk_speed;
		else if(withinDistance(30.0f)) return m_walk_speed * 1.5f;
		else return m_walk_speed * 2.0f;
	}
	return m_walk_speed;
}

bool WayPoint::withinDistance(float meters){
	if (m_node->getPosition().x >= m_destination.x - meters
		&& m_node->getPosition().x <= m_destination.x + meters)
	{
		if (m_node->getPosition().y >= m_destination.y - meters
		&& m_node->getPosition().y <= m_destination.y + meters)
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
		m_destination = m_old_destination;
	}

	if (m_direction == Ogre::Vector3::ZERO) 
        {
            if (NextLocation()) { }
        }
		else
		{
			//do naaathing
			//std::cout << "Deque empty: " << m_walk_list.empty() << std::endl;
		}

	if (withinDistance(1.0f)){
		if(m_follow_node != NULL) {
			//do nothing for now
		}
		else {
			m_direction = Ogre::Vector3::ZERO;
		}
	}
};