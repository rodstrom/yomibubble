#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "InputPrereq.h"
#include <OGRE\SdkTrays.h>


class Game;
class InputSystem : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener, public Ogre::WindowEventListener
{
public:
	InputSystem(Game* bubble_adventure, Ogre::RenderWindow* render_window);
	~InputSystem(void);
	
	void Capture();
	void Init();
	void Shut();
	void Test();

	const bool IsKeyDown(OIS::KeyCode) const;		// check if a keyboard key is down
	const bool IsKeyPressed(OIS::KeyCode) const;	// check if a keybaord key is pressed
	const bool IsKeyReleased(OIS::KeyCode) const;	// check if a keyboard key is released

	const bool IsMouseButtonDown(OIS::MouseButtonID, OIS::MouseEvent& evt) const;
	const bool IsMouseButtonPressed(OIS::MouseButtonID, OIS::MouseEvent& evt) const;
	const bool IsMouseButtonReleased(OIS::MouseButtonID, OIS::MouseEvent& evt) const;

	const OIS::Keyboard*	GetKeyboard() const		{ return m_keyboard; }
	const OIS::Mouse*		GetMouse() const		{ return m_mouse; }
	
	const OIS::JoyStick*	GetJoystick(unsigned int) const;


	void SetWindowExtents(int p_width, int p_height);	// will set new window boundary for the mouse, call this after window has been resized.

	const MousePosition& GetMousePosition() const { return m_mouse_position; }
	OIS::Mouse* GetMouse() { return m_mouse; }
	

private:
	Game* m_game;

	virtual void windowResized(Ogre::RenderWindow* render_window);
	virtual void windowClosed(Ogre::RenderWindow* render_window);

	// 238 is the current amount of OIS::KeyCode objects available for keyboard keys
	// for more information check OISKeyboard.h
	bool m_keys[238];		
	bool m_last_keys[238];

	bool m_mouse_buttons[8];
	bool m_last_mouse_buttons[8];

	bool keyPressed(const OIS::KeyEvent&);
	bool keyReleased(const OIS::KeyEvent&);

	bool mouseMoved(const OIS::MouseEvent&);
	bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
	bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

	bool povMoved(const OIS::JoyStickEvent&, int);
	bool axisMoved(const OIS::JoyStickEvent&, int);
	bool sliderMoved(const OIS::JoyStickEvent&, int);
	bool buttonPressed(const OIS::JoyStickEvent&, int);
	bool buttonReleased(const OIS::JoyStickEvent&, int);

	OIS::Mouse*			m_mouse;
	OIS::Keyboard*		m_keyboard;
	OIS::InputManager*	m_ois_input_manager;
	
	std::vector<OIS::JoyStick*>				m_joysticks;

	std::vector<OIS::JoyStickListener*>		m_joystick_listeners;

	MousePosition m_mouse_position;
	Ogre::RenderWindow* m_render_window;
};

#endif // _INPUT_MANAGER_H_