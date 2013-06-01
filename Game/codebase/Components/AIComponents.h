#ifndef AI_COMPONENTS_H
#define AI_COMPONENTS_H

#include "ComponentsPrereq.h"
#include "../Waypoint.h"

class WayPointComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	WayPointComponent(void){ m_type = COMPONENT_AI; m_update = true; };
	virtual ~WayPointComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(Ogre::SceneNode *m_node, float walk_speed);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);
	
	void AddWayPoint(Ogre::Vector3 way_point);
	void AddWayPoint(Ogre::SceneNode* scene_node);
	void SetLoopable(Ogre::String loop);

private:
	WayPoint* m_way_point;
	AnimationMsg m_anim_msg;
	bool m_pause;
};

class AIState;
class TottAIComponent : public Component, public IComponentUpdateable, public IComponentObserver {
public:
	TottAIComponent(void) : m_current_index(0){}
	virtual ~TottAIComponent(void){}

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(const std::vector<AIState*>& ai_states);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);

protected:
	bool m_pause;
	int m_current_index;
	AIState*	m_current_ai_state;
	std::vector<AIState*> m_ai_states;
};

#endif //AI_COMPONENTS_H