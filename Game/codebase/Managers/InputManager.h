#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "..\InputPrereq.h"

class InputManager;
class InputListener{
public:
	virtual void AddInputManager(InputManager* input_manager)		= 0;
	virtual void RemoveInputManager(InputManager* input_manager)	= 0;
	virtual void Update()											= 0;
};

class InputManager{
public:
	InputManager(void);
	~InputManager(void);

	void Update();
	bool IsButtonDown(int btn);
	bool IsButtonPressed(int btn);
	bool IsButtonReleased(int btn);

	void ButtonPressed(int btn);
	void ButtonReleased(int btn);
	void SetMouseState(const OIS::MouseState mouse_state);
	void SetPause(bool value) { m_pause = value; }

	const OIS::MouseState& GetMouseState() const { return m_mouse_state; }

private:
	bool			m_pause;
	bool			m_buttons[BTN_SIZE];
	bool			m_last_buttons[BTN_SIZE];
	OIS::MouseState m_mouse_state;
};

#endif // _CONTROLS_H_