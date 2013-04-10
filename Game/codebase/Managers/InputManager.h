#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "..\InputPrereq.h"
#include "..\InputSystem.h"

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
	void SetMousePosition(int x, int y, int rel_x, int rel_y);
	void SetPause(bool value) { m_pause = value; }

	const MousePosition& GetMousePosition() const { return m_mouse_position; }
	InputSystem* GetInputSystem() { return m_input_system; }

private:
	bool			m_pause;
	bool			m_buttons[BTN_SIZE];
	bool			m_last_buttons[BTN_SIZE];
	MousePosition	m_mouse_position;
	InputSystem*    m_input_system;
};

#endif // _CONTROLS_H_