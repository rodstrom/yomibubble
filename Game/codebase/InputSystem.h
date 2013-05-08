#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "InputPrereq.h"

class Game;
class InputSystem : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener, public Ogre::WindowEventListener
{
public:
	InputSystem(Game* bubble_adventure, Ogre::RenderWindow* render_window);
	~InputSystem(void);
	
	void Capture();
	void Init();
	void Shut();

	const OIS::Keyboard*	GetKeyboard() const		{ return m_keyboard; }
	const OIS::Mouse*		GetMouse() const		{ return m_mouse; }
	
	const OIS::JoyStick*	GetJoystick(unsigned int) const;


	void SetWindowExtents(int p_width, int p_height);	// will set new window boundary for the mouse, call this after window has been resized.

	OIS::Mouse* GetMouse() { return m_mouse; }
	

private:
	Game* m_game;

	virtual void windowResized(Ogre::RenderWindow* render_window);
	virtual void windowClosed(Ogre::RenderWindow* render_window);

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

	float m_last_z;
	float m_last_x;
	float m_delta_zoom;
	float m_camera_dead_zone;
	float m_movement_dead_zone;

	OIS::Mouse*			m_mouse;
	OIS::Keyboard*		m_keyboard;
	OIS::InputManager*	m_ois_input_manager;
	
	std::vector<OIS::JoyStick*>				m_joysticks;

	//std::vector<OIS::JoyStickListener*>		m_joystick_listeners;

	Ogre::RenderWindow* m_render_window;
};

#endif // _INPUT_MANAGER_H_