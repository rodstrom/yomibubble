#include "..\stdafx.h"
#include "StateManager.h"
#include "..\InputSystem.h"
#include "InputManager.h"

StateManager::StateManager(Ogre::RenderWindow* render_window, InputListener* input_listener, MessageSystem* message_system, OgreBites::SdkTrayManager* p_tray_manager) 
	: m_render_window(render_window), m_input_listener(input_listener), m_message_system(message_system), m_tray_manager(p_tray_manager){}

StateManager::~StateManager(void){}

void StateManager::ManageState(const Ogre::String& id, State* state){
	state_info nsi(id, state);
	nsi._state->Init(m_render_window, m_message_system, m_tray_manager);
	m_states.push_back(nsi);
}

State* StateManager::FindById(const Ogre::String& id){
	std::vector<state_info>::iterator it;
	for (it = m_states.begin(); it != m_states.end(); it++){
		if (it->_id == id)
			return it->_state;
	}
	return NULL;
}

void StateManager::ChangeState(State* state){
	if (!m_state_stack.empty()){
		Cleanup(m_state_stack.back());
		m_state_stack.back()->Exit();
		m_state_stack.pop_back();
	}
	m_state_stack.push_back(state);
	Init(state);
	m_state_stack.back()->Enter();
}

bool StateManager::PushState(State* state){
	if (!m_state_stack.empty()){
		if (!m_state_stack.back()->Pause())
			return false;
		Cleanup(m_state_stack.back());
	}
	m_state_stack.push_back(state);
	Init(state);
	m_state_stack.back()->Enter();
	return true;
}

void StateManager::PopState(){
	if (!m_state_stack.empty()){
		Cleanup(m_state_stack.back());
		m_state_stack.back()->Exit();
		m_state_stack.pop_back();
	}

	if (!m_state_stack.empty()){
		Init(m_state_stack.back());
		m_state_stack.back()->Resume();
	}
	else{
		//TODO send MSG to quit
	}
}

void StateManager::Init(State* state){
	Ogre::Root::getSingleton().addFrameListener(state);
	m_input_listener->AddInputManager(state->GetInputManager());
}

void StateManager::Cleanup(State* state){
	Ogre::Root::getSingleton().removeFrameListener(state);
	m_input_listener->RemoveInputManager(state->GetInputManager());
}

void StateManager::Shut(){
	while (!m_state_stack.empty()){
		Cleanup(m_state_stack.back());
		m_state_stack.back()->Exit();
		m_state_stack.pop_back();
	}
	while (!m_states.empty()){
		m_states.back()._state->Destroy();
		m_states.pop_back();
	}
}