#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include "..\States\State.h"

class MessageSystem;
class InputListener;
class StateManager : public StateListener
{
public:
	typedef struct state_info
	{
		state_info();
		state_info(Ogre::String id, State* state){_id = id; _state = state;}
		Ogre::String _id;
		State* _state;
	};

	StateManager(Ogre::RenderWindow* render_window, InputListener* input_listener, MessageSystem* message_system);
	~StateManager(void);

	bool Update(float dt);
	State* FindById(const Ogre::String& id);
	void ManageState(const Ogre::String& id, State* state);
	void Start(State* state);
	void ChangeState(State*);
	bool PushState(State*);
	void PopState();
	void Shut();

protected:
	void Init(State* state);
	void Cleanup(State* state);
	
	std::vector<State*>			m_state_stack;
	std::vector<state_info>		m_states;
	Ogre::RenderWindow*			m_render_window;
	InputListener*				m_input_listener;
	MessageSystem*				m_message_system;
};

#endif // _STATE_MANAGER_H_