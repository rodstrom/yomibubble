#ifndef AI_COMPONENTS_H
#define AI_COMPONENTS_H

#include "ComponentsPrereq.h"
#include "../Waypoint.h"

class WayPointComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	WayPointComponent(void){ m_type = COMPONENT_AI; };
	virtual ~WayPointComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(Ogre::SceneNode *m_node, float walk_speed);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);
	
	void AddWayPoint(Ogre::Vector3 way_point);
	void AddWayPoint(Ogre::SceneNode* scene_node);

	void AddWayPoint(Ogre::Vector3 way_point);

private:
	WayPoint* m_way_point;
};

#endif //AI_COMPONENTS_H