#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <deque>
#include "Components\GameObject.h"

class WayPoint{
public:
	WayPoint();
	~WayPoint();

	bool NextLocation();
	void Init(Ogre::SceneNode* node, float walk_speed);
	void Update(float dt);
	
	void AddWayPoint(Ogre::Vector3 way_point);
	void AddWayPoint(Ogre::SceneNode* scene_node);

    Ogre::Vector3 m_direction;              // The direction the object is moving
    Ogre::Vector3 m_destination;            // The destination the object is moving towards

    Ogre::SceneNode* m_node;                // The SceneNode that the Entity is attached to
	std::deque<Ogre::Vector3> m_walk_list;   // The list of points we are walking to
 
    Ogre::Real m_walk_speed;                 // The speed at which the object is moving 

	Ogre::SceneNode* m_follow_node;

	void setLoopable(Ogre::String loop);
	float getSpeed();
private:
	bool withinDistance(float meters);
	bool m_follow_node_moving;
	bool m_loop_waypoints;
	int m_current_waypoint;

	Ogre::Vector3 m_old_destination;
};

#endif //WAYPOINT_H