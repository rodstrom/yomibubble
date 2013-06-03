#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include "..\States\State.h"

class MessageSystem;
class InputListener;
class FadeInFadeOut;
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
	void Init(State* state);
	bool Update(float dt);
	State* FindById(const Ogre::String& id);
	void ManageState(const Ogre::String& id, State* state);
	void Start(State* state);
	void ChangeState(State*);
	bool PushState(State*);
	void PopState();
	void Shut();
	void SecondLoading();
	void ExitTest();
	void ShowLoadingScreen(const Ogre::String& overlay_name);
	void HideLoadingScreen();
	void PauseUpdate(bool value) { m_pause_update = value; }

protected:
	void Cleanup(State* state);

	std::vector<State*>			m_state_stack;
	std::vector<state_info>		m_states;
	Ogre::RenderWindow*			m_render_window;
	InputListener*				m_input_listener;
	MessageSystem*				m_message_system;
	State*						m_next_state;
	State*						m_loading;
	FadeInFadeOut*				m_fade;

	bool						m_pop_on_update;
	bool						m_fading;
	bool						m_loading_fading;
	bool						m_pause_update;
	std::function<void()>		func;
	std::function<void()>		func2;
	Ogre::Overlay*				m_loading_overlay;		// an overlay that we will display while the level is loading
};

#endif // _STATE_MANAGER_H_