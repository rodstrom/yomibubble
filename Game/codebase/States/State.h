#ifndef _STATE_H_
#define _STATE_H_

#include "..\Managers\InputManager.h"

class MessageSystem;
class SoundManager;
class State;
class StateListener
{
public:
	StateListener(void){}
	virtual ~StateListener(void){}
	virtual void ManageState(const Ogre::String& id, State* state) = 0;
	virtual State* FindById(const Ogre::String& id) = 0;
	virtual void ChangeState(State* state) = 0;
	virtual bool PushState(State* state) = 0;
	virtual void PopState() = 0;
};

//class State : public Ogre::FrameListener
class State
{
public:
	template <typename T>
	static void Create(StateListener* p_parent, const Ogre::String p_id){
		T* state = new T;
		state->m_parent = p_parent;
		p_parent->ManageState(p_id, state);
	}
	void Destroy(void) { delete this; }
	void Init(Ogre::RenderWindow* render_window, MessageSystem* message_system) { m_render_window = render_window; m_message_system = message_system; }
	InputManager* GetInputManager() const { return m_input_manager; }
	virtual bool Update(float dt) = 0;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual bool Pause() { return false; }
	virtual void Resume(){}

protected:
	State(void){ m_input_manager = new InputManager; }
	virtual ~State(void){ delete m_input_manager; }

	State* FindByName(const Ogre::String& id) { return m_parent->FindById(id); }
	void ChangeState(State* state) { m_parent->ChangeState(state); }
	bool PushState(State* state) { return m_parent->PushState(state); }
	void PopState() { m_parent->PopState(); }

	StateListener*		m_parent;
	Ogre::SceneManager* m_scene_manager;
	Ogre::RenderWindow* m_render_window;
	Ogre::Camera*		m_camera;
	Ogre::Viewport*		m_viewport;
	InputManager*		m_input_manager;
	MessageSystem*		m_message_system;
	SoundManager*		m_sound_manager;
};


//#define DECLARE_STATE_CLASS(T) static void create(StateListener* parent, const Ogre::String id)    { T *myGameState = new T(); myGameState->m_parent=parent; parent->manageState(id,myGameState); }

#endif // _STATE_H_