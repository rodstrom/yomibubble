#ifndef _N_BUBBLE_ADVENTURE_H_
#define _N_BUBBLE_ADVENTURE_H_

#include "Managers\InputManager.h"

class MessageSystem;
class StateManager;
class BubbleAdventure : public InputListener
{
public:
	BubbleAdventure(void);
	~BubbleAdventure(void);

	bool Init(Ogre::RenderWindow* render_window, MessageSystem* message_system);
	void Shut();
	void SetMouseState(const OIS::MouseState mouse_state);
	void ButtonPressed(int btn);
	void ButtonReleased(int btn);

	void AddInputManager(InputManager* input_manager);
	void RemoveInputManager(InputManager* input_manager);
	void Update();

private:
	std::vector<InputManager*>	m_input_managers;
	StateManager*				m_state_manager;
};

#endif // _N_BUBBLE_ADVENTURE_H_