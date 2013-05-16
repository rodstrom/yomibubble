#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "..\InputPrereq.h"

class InputManager;
class InputListener{
public:
	virtual void AddInputManager(InputManager* input_manager)		= 0;
	virtual void RemoveInputManager(InputManager* input_manager)	= 0;
	virtual void UpdateInput()										= 0;
};

class InputManager{
public:
	InputManager(void);
	~InputManager(void);

	void Update(); // Internal method called from the game class, do not call directly

	// Use these methods to get input feedback
	bool IsButtonDown(int btn);
	bool IsButtonPressed(int btn);
	bool IsButtonReleased(int btn);

	// These methods are used by the InputSystem, do not call them directly.
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
	void SetPause(bool value) { m_pause = value; }

	const MousePosition& GetMousePosition() const { return m_mouse_position; }
	const MovementAxis& GetMovementAxis() const { return m_movement_axis; }
	const CameraAxis& GetCameraAxis() const { return m_camera_axis; }
	const OIS::MouseState& GetMouseState() const { return m_mouse_state; }

private:
	bool			m_pause;
	bool			m_buttons[BTN_SIZE];
	bool			m_last_buttons[BTN_SIZE];
	OIS::MouseState m_mouse_state;
	MousePosition	m_mouse_position;
	MovementAxis	m_movement_axis;
	CameraAxis		m_camera_axis;
};

#endif // _CONTROLS_H_