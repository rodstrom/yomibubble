#ifndef _N_GAME_INSTANCE_H_
#define _N_GAME_INSTANCE_H_

#include "Managers\InputManager.h"

class MessageSystem;
class StateManager;
class SoundManager;

class Game : public InputListener
{
public:
	Game(void);
	~Game(void);

	bool Update(float dt);
	bool Init(Ogre::RenderWindow* render_window, MessageSystem* message_system, SoundManager* sound_manager);
	void Shut();

	void InjectPressedButton(int btn);
	void InjectReleasedButton(int btn);
	void InjectRelativeMovement(float x, float z);
	void InjectRelativeMovementX(float x);
	void InjectRelativeMovementZ(float z);
	void InjectRelativeCameraAxis(float x, float y, float z);
	void InjectRelativeCameraAxisX(float x);
	void InjectRelativeCameraAxisY(float y);
	void InjectRelativeCameraAxisZ(float z);
	void InjectMouseState(const OIS::MouseState mouse_state);
	void InjectMousePosition(int x, int y);

	void AddInputManager(InputManager* input_manager);
	void RemoveInputManager(InputManager* input_manager);
	void UpdateInput();

private:
	std::vector<InputManager*>	m_input_managers;
	StateManager*				m_state_manager;
};

#endif _GAME_H