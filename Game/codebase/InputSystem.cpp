#include "stdafx.h"
#include "InputSystem.h"
#include "Game.h"
#include "MessageSystem.h"
#include <iostream>

InputSystem::InputSystem(Game* game, Ogre::RenderWindow* render_window, MessageSystem* message_system) : 
m_game(game),
m_render_window(render_window),
m_mouse(nullptr),
m_keyboard(nullptr),
m_ois_input_manager(nullptr),
m_message_system(message_system),
m_last_x(0.0f), m_last_z(0.0f), 
m_delta_zoom(0.0f),
m_movement_dead_zone(0.2f),
m_camera_dead_zone(0.2f){}

InputSystem::~InputSystem(void){}

void InputSystem::Init(){
	Ogre::WindowEventUtilities::addWindowEventListener(m_render_window, this);
	if (!m_ois_input_manager){
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		m_render_window->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#ifndef NO_CONSOLE
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#endif
		m_ois_input_manager = OIS::InputManager::createInputSystem(pl);
		
		if (m_ois_input_manager->getNumberOfDevices(OIS::OISKeyboard) > 0){
			m_keyboard = static_cast<OIS::Keyboard*>(m_ois_input_manager->createInputObject(OIS::OISKeyboard, true));
			m_keyboard->setEventCallback(this);
		}

		if (m_ois_input_manager->getNumberOfDevices(OIS::OISMouse) > 0){
			m_mouse = static_cast<OIS::Mouse*>(m_ois_input_manager->createInputObject(OIS::OISMouse, true));
			m_mouse->setEventCallback(this);
		}

		if (m_ois_input_manager->getNumberOfDevices(OIS::OISJoyStick) > 0){
			m_joysticks.resize(m_ois_input_manager->getNumberOfDevices(OIS::OISJoyStick));
			for (auto it = m_joysticks.begin(); it != m_joysticks.end(); it++){
				(*it) = static_cast<OIS::JoyStick*>(m_ois_input_manager->createInputObject(OIS::OISJoyStick, true));
				(*it)->setEventCallback(this);
			}
		}	
	}
	windowResized(m_render_window);
}

void InputSystem::Shut(){
	if (m_ois_input_manager){
		if (m_keyboard){
			m_ois_input_manager->destroyInputObject(m_keyboard);
			m_keyboard = nullptr;
		}

		if (m_mouse){
			m_ois_input_manager->destroyInputObject(m_mouse);
			m_mouse = nullptr;
		}

		if (m_joysticks.size() > 0){
			for (auto it = m_joysticks.begin(); it != m_joysticks.end(); it++){
				m_ois_input_manager->destroyInputObject((*it));
			}
			m_joysticks.clear();
		}
		OIS::InputManager::destroyInputSystem(m_ois_input_manager);
		m_ois_input_manager = nullptr;
	}
	Ogre::WindowEventUtilities::removeWindowEventListener(m_render_window, this);
}

void InputSystem::Capture(){
	bool mouse_change = false;
	if (m_mouse){
		m_mouse->capture();
		OIS::MouseState mouse_state = m_mouse->getMouseState();
		m_game->InjectMouseState(mouse_state);
		m_game->InjectMousePosition(mouse_state.X.abs, mouse_state.Y.abs);
		m_game->InjectRelativeCameraAxis(-0.1f * mouse_state.X.rel, -0.1f * mouse_state.Y.rel, (mouse_state.Z.rel + m_delta_zoom));

		if (mouse_state.X.rel + mouse_state.Y.rel + mouse_state.Z.rel + m_delta_zoom != 0) { mouse_change = true; }
		//m_delta_zoom = 0.0f;
	}
	/*
	if (m_delta_zoom != 0.0f)
	{ m_game->InjectRelativeCameraAxis(mouse_state.X.reö  m_delta_zoom); mouse_change = true; }
	*/
	bool changeXmove = false;
	bool changeZmove = false;
	if (m_keyboard){
		m_keyboard->capture();
		if (m_keyboard->isKeyDown(OIS::KC_W)){
			//m_game->InjectRelativeMovementZ(-0.002f);
			m_game->InjectRelativeMovementZ(-1.0f);
			changeZmove = true;
		}
		else if (m_keyboard->isKeyDown(OIS::KC_S)){
			m_game->InjectRelativeMovementZ(1.0f);
			changeZmove = true;
		}
		else{
			m_game->InjectRelativeMovementZ(0.0f);
			changeZmove = true;
		}
		if (m_keyboard->isKeyDown(OIS::KC_A)){
			m_game->InjectRelativeMovementX(-1.0f);
			changeXmove = true;
		}
		else if (m_keyboard->isKeyDown(OIS::KC_D)){
			m_game->InjectRelativeMovementX(1.0f);
			changeXmove = true;
		}
		else{
			m_game->InjectRelativeMovementX(0.0f);
			changeXmove = true;
		}
	}
	if (m_joysticks.size() > 0){
		for (auto it = m_joysticks.begin(); it != m_joysticks.end(); it++){
			(*it)->capture();
			
			float move_x = (float)(*it)->getJoyStickState().mAxes[1].abs / 32767.0f;
			float move_z = (float)(*it)->getJoyStickState().mAxes[0].abs / 32767.0f;
			float camera_x = (float)(*it)->getJoyStickState().mAxes[3].abs / 32767.0f;
			float camera_y = (float)(*it)->getJoyStickState().mAxes[2].abs / 32767.0f;

			if (move_x < m_movement_dead_zone &&  move_x > -m_movement_dead_zone){
				move_x = 0.0f;
			}
			if (move_z < m_movement_dead_zone &&  move_z > -m_movement_dead_zone){
				move_z = 0.0f;
			}
			if (camera_x < m_camera_dead_zone &&  camera_x > -m_camera_dead_zone){
				camera_x = 0.0f;
			}
			if (camera_y < m_camera_dead_zone &&  camera_y > -m_camera_dead_zone){
				camera_y = 0.0f;
			}

			m_game->InjectRelativeMovementZ(move_z);
			m_game->InjectRelativeMovementX(move_x);
			m_game->InjectRelativeCameraAxisX(camera_x);
			m_game->InjectRelativeCameraAxisY(camera_y);

		}
	}
}

void InputSystem::SetWindowExtents(int width, int height){
	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	mouseState.width	= width;
	mouseState.height	= height;
}

const OIS::JoyStick* InputSystem::GetJoystick(unsigned int index) const {
	if (index < m_joysticks.size()) {
		return m_joysticks[index];
	}
	return nullptr;
}

// Methods inherited from OIS Key, Mouse & JoyStick listeners
bool InputSystem::keyPressed(const OIS::KeyEvent& e){
	switch (e.key){
	case OIS::KC_UP:
		m_game->InjectPressedButton(BTN_ARROW_UP);
		break;
	case OIS::KC_DOWN:
		m_game->InjectPressedButton(BTN_ARROW_DOWN);
		break;
	case OIS::KC_RIGHT:
		m_game->InjectPressedButton(BTN_ARROW_RIGHT);
		break;
	case OIS::KC_LEFT:
		m_game->InjectPressedButton(BTN_ARROW_LEFT);
		break;
	case OIS::KC_W:
		break;
	case OIS::KC_S:
		break;
	case OIS::KC_A:
		break;
	case OIS::KC_D:
		break;
	case OIS::KC_SPACE:
		m_game->InjectPressedButton(BTN_A);
		break;
	case OIS::KC_ESCAPE:
		m_game->InjectPressedButton(BTN_BACK);
		break;
	case OIS::KC_V:
		m_game->InjectPressedButton(BTN_INVERT_VERTICAL);
		break;
	case OIS::KC_H:
		m_game->InjectPressedButton(BTN_INVERT_HORIZONTAL);
		break;
	default:
		break;
	};
	return true;
}

bool InputSystem::keyReleased(const OIS::KeyEvent& e){
	switch (e.key){
	case OIS::KC_UP:
		m_game->InjectReleasedButton(BTN_ARROW_UP);
		break;
	case OIS::KC_DOWN:
		m_game->InjectReleasedButton(BTN_ARROW_DOWN);
		break;
	case OIS::KC_RIGHT:
		m_game->InjectReleasedButton(BTN_ARROW_RIGHT);
		break;
	case OIS::KC_LEFT:
		m_game->InjectReleasedButton(BTN_ARROW_LEFT);
		break;
	case OIS::KC_W:
		m_game->InjectRelativeMovementZ(0.0f);
		break;
	case OIS::KC_S:
		m_game->InjectRelativeMovementZ(0.0f);
		break;
	case OIS::KC_A:
		m_game->InjectRelativeMovementX(0.0f);
		break;
	case OIS::KC_D:
		m_game->InjectRelativeMovementX(0.0f);
		break;
	case OIS::KC_SPACE:
		m_game->InjectReleasedButton(BTN_A);
		break;
	case OIS::KC_ESCAPE:
		m_game->InjectReleasedButton(BTN_BACK);
		break;
	case OIS::KC_V:
		m_game->InjectReleasedButton(BTN_INVERT_VERTICAL);
		break;
	case OIS::KC_H:
		m_game->InjectReleasedButton(BTN_INVERT_HORIZONTAL);
		break;
	default:
		break;
	};
	return true;
}

bool InputSystem::mouseMoved(const OIS::MouseEvent& e){
	//m_bouncy->SetMousePosition(e.state.X.abs, e.state.Y.abs, e.state.X.rel, e.state.Y.rel);
	return true;
}

bool InputSystem::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id){
	switch (id){
	case 1: //OIS::MouseButtonID::MB_Right: (1 == OIS::MouseButtonID::MB_Right, writing "OIS::MouseButtonID::MB_Right" instead of "1" causes build warning)
		m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left:
		m_game->InjectPressedButton(BTN_LEFT_MOUSE);
		break;
	default:
		break;
	}
	return true;
}

bool InputSystem::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id){
	switch (id){
	case 1: //OIS::MouseButtonID::MB_Right:
		m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left:
		m_game->InjectReleasedButton(BTN_LEFT_MOUSE);
		break;
	case 6: 
		m_game->InjectPressedButton(BTN_BACK);
		break;
	case 7: 
		m_game->InjectPressedButton(BTN_START);
		break;
	default:
		break;
	}
	return true;
}

bool InputSystem::povMoved(const OIS::JoyStickEvent& e, int pov){
	
	return true;
}

bool InputSystem::axisMoved(const OIS::JoyStickEvent& e, int axis){
	if(e.state.mAxes[0].abs > 16384 || e.state.mAxes[0].abs < -16384) {
		float movement = e.state.mAxes[0].abs * 0.50000005f;
		//m_game->InjectRelativeMovementZ(movement);
		if(e.state.mAxes[0].abs < 0) {
			m_game->InjectPressedButton(BTN_UP);
		}
		else {			
			m_game->InjectPressedButton(BTN_DOWN);
		}
	}
	else {
		//m_game->InjectRelativeMovementZ(0.0f);
		m_game->InjectReleasedButton(BTN_UP);
		m_game->InjectReleasedButton(BTN_DOWN);
	}

	if(e.state.mAxes[1].abs > 16384 || e.state.mAxes[1].abs < -16384) {
		float movement = e.state.mAxes[1].abs * 0.50000005f;
		//m_game->InjectRelativeMovementX(movement);
	}
	else {
		//m_game->InjectRelativeMovementX(0.0f);
	}
	
	float camX = 0.0f; 
	float camY = 0.0f;
	if(e.state.mAxes[2].abs > 16384 || e.state.mAxes[2].abs < -16384) {
		camY = e.state.mAxes[2].abs * 0.0005f;
	}
	if(e.state.mAxes[3].abs > 16384 || e.state.mAxes[3].abs < -16384) {
		camX = e.state.mAxes[3].abs * 0.0005f;
	}
	//m_game->InjectRelativeCameraAxis(camX, camY, 0.0f);

	//std::cout << e.state.mAxes[4].abs << std::endl;

	if (e.state.mAxes[4].abs > 0){
		m_game->InjectPressedButton(BTN_LEFT_MOUSE);
		m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
	}
	else if (e.state.mAxes[4].abs < 0){
		m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		m_game->InjectReleasedButton(BTN_LEFT_MOUSE);
	}
	else{
		m_game->InjectReleasedButton(BTN_LEFT_MOUSE);
		m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
	}

	return true;
}

bool InputSystem::sliderMoved(const OIS::JoyStickEvent& e, int sliderID){
	
	return true;
}

bool InputSystem::buttonPressed(const OIS::JoyStickEvent& e, int button){
	switch (button){
	case 13:
		//m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 12:
		//m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 11:
		//m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 10:
		//m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 9:
		//m_game->InjectPressedButton(BTN_RIGHT_MOUSE);
		break;
	case 8: //?

		break;
	case 7: //Start Button
		
		break;
	case 6: //Back Button
		m_game->InjectPressedButton(BTN_BACK);
		break;
	case 5: //Right Button
		m_delta_zoom += 30.00005f;
		//m_game->InjectRelativeCameraAxisZ(m_delta_zoom);
		//m_game->InjectRelativeCameraAxisZ(1000.0);
		break;
	case 4: //Left Button
		m_delta_zoom -= 30.00005f;
		//m_game->InjectRelativeCameraAxisZ(m_delta_zoom);
		//m_game->InjectRelativeCameraAxisZ(-1000.0);
		break;
	case 3: //Y button
		
		break;
	case 2: //X button
		m_game->InjectPressedButton(BTN_X);
		break;
	case 1: //OIS::MouseButtonID::MB_Right: //or GamePad B
		//m_game->InjectPressedButton(BTN_A);
		//m_game->InjectPressedButton(BTN_LEFT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left: //or GamePad A
		m_game->InjectPressedButton(BTN_A);
		break;
	default:
		break;
	}
	return true;
}

bool InputSystem::buttonReleased(const OIS::JoyStickEvent& e, int button){
	switch (button){
	case 13:
		//m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 12:
		//m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 11:
		//m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 10:
		//m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 9:
		//m_game->InjectReleasedButton(BTN_RIGHT_MOUSE);
		break;
	case 8: //?
		
		break;
	case 7: //Start Button
		
		break;
	case 6: //Back Button
		m_game->InjectReleasedButton(BTN_BACK);
		break;
	case 5: //Right Button
		m_delta_zoom = 0.0f;
		break;
	case 4: //Left Button
		m_delta_zoom = 0.0f;
		break;
	case 3: //Y button
		
		break;
	case 2: //X button
		m_game->InjectReleasedButton(BTN_X);
		break;
	case 1: //OIS::MouseButtonID::MB_Right: //or GamePad B
		//m_game->InjectReleasedButton(BTN_A);
		//m_game->InjectReleasedButton(BTN_LEFT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left: //or GamePad A
		m_game->InjectReleasedButton(BTN_A);
		break;
	default:
		break;
	}
	return true;
}

void InputSystem::windowResized(Ogre::RenderWindow* rw){
	SetWindowExtents((int)rw->getWidth(), (int)rw->getHeight());
}

void InputSystem::windowClosed(Ogre::RenderWindow* rw){
	IEvent evt;
	evt.m_type = EVT_QUIT;
	m_message_system->Notify(&evt);
}