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

void WayPoint::Update(float dt){
	if (m_direction == Ogre::Vector3::ZERO) 
        {
            if (NextLocation()) { }
        }
		else
		{
			//do naaathing
			//std::cout << "Deque empty: " << m_walk_list.empty() << std::endl;
		}

	int tolerance = 2;

	if (m_node->getPosition().x >= m_destination.x - tolerance
		&& m_node->getPosition().x <= m_destination.x + tolerance)
	{
		if (m_node->getPosition().y >= m_destination.y - tolerance
		&& m_node->getPosition().y <= m_destination.y + tolerance)
		{
			m_direction = Ogre::Vector3::ZERO;
		}
	}
};