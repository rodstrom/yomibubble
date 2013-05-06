#ifndef TIMER_COMPONENT_H
#define TIMER_COMPONENT_H

#include "ComponentsPrereq.h"
#include <functional>

class TimerComponent : public Component, public IComponentObserver, public IComponentUpdateable {
public:
	TimerComponent(void);
	~TimerComponent(void);
	virtual void Notify(int type, void* message);
	virtual void Init(float p_time, bool p_destroy, std::function<void()> p_func);
	virtual void Update(float dt);
	virtual void SetMessenger(ComponentMessenger * messenger);
	virtual void Shut();
protected:
	float			m_timer;
	float			m_time_to_live;
	bool			m_start_timer;
	bool			m_destroy;
	std::function<void()>		m_func;
};

#endif //TIMER_COMPONENT_H