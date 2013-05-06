#include "stdafx.h"
#include "TimerComponent.h"
#include "ComponentMessenger.h"
#include "GameObject.h"


TimerComponent::TimerComponent(void) : m_timer(0.0f), m_start_timer(false), m_time_to_live(0.0f), m_destroy(false){}


TimerComponent::~TimerComponent(void){}

void TimerComponent::Init(float p_time, bool p_destroy, std::function<void()> p_func){
	m_func = p_func;
	m_time_to_live = p_time;
	m_destroy = p_destroy;
	m_messenger->Notify(MSG_START_TIMER, NULL);
}

void TimerComponent::Notify(int type, void* message){
	switch(type){
	case MSG_START_TIMER:
		m_start_timer = true;
		break;
	default:
		break;
	}

}

void TimerComponent::Update(float dt){
	if(m_start_timer){
		m_timer += dt;
		std::cout << m_timer << std::endl;
		if(m_timer >= m_time_to_live){
			if(m_destroy){
				m_func();
				//m_owner->RemoveComponent(this);
			}
			else{
				m_func();
				m_timer = 0.0f;
				m_start_timer = false;
			}
				
		}
	}
}

void TimerComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_START_TIMER, this);
}

void TimerComponent::Shut(){
	m_messenger->Unregister(MSG_START_TIMER, this);
}
