#ifndef _N_GAME_INSTANCE_H_
#define _N_GAME_INSTANCE_H_

#include "Managers\InputManager.h"

class MessageSystem;
class StateManager;
class Game : public InputListener
{
public:
	Game(void);
	~Game(void);

	bool Update(float dt);
	bool Init(Ogre::RenderWindow* render_window, MessageSystem* message_system);
	void Shut();
	void SetMouseState(const OIS::MouseState mouse_state);
	void ButtonPressed(int btn);
	void ButtonReleased(int btn);

	void AddInputManager(InputManager* input_manager);
	void RemoveInputManager(InputManager* input_manager);
	void UpdateInput();

private:
	std::vector<InputManager*>	m_input_managers;
	StateManager*				m_state_manager;
};

#endif // _N_GAME_INSTANCE_H_