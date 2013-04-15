#include "stdafx.h"
#include "InputSystem.h"
#include "Game.h"

InputSystem::InputSystem(Game* game, Ogre::RenderWindow* render_window) : 
m_game(game),
m_render_window(render_window),
m_mouse(nullptr),
m_keyboard(nullptr),
m_ois_input_manager(nullptr){}

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
		std::fill(m_keys, m_keys + 238, false);
		std::fill(m_last_keys, m_last_keys + 238, false);
		std::fill(m_mouse_buttons, m_mouse_buttons + 8, false);
		std::fill(m_last_mouse_buttons, m_last_mouse_buttons + 8, false);
	}
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
	if (m_mouse){
		m_mouse->capture();
		m_game->SetMouseState(m_mouse->getMouseState());
	}
	if (m_keyboard){
		m_keyboard->capture();
	}
	if (m_joysticks.size() > 0){
		for (auto it = m_joysticks.begin(); it != m_joysticks.end(); it++){
			(*it)->capture();
		}
	}
}

void InputSystem::SetWindowExtents(int width, int height){
	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	mouseState.width	= width;
	mouseState.height	= height;
}

const bool InputSystem::IsKeyDown(OIS::KeyCode key) const{
	if (m_keys[key]){
		return true;
	}
	return false;
}

const bool InputSystem::IsKeyPressed(OIS::KeyCode key) const{
	if (!m_last_keys[key] && m_keys[key])
		return true;
	return false;
}

const bool InputSystem::IsKeyReleased(OIS::KeyCode key) const{
	if (m_last_keys[key] && !m_keys[key]){
		return true;
	}
	return false;
}

const bool InputSystem::IsMouseButtonDown(OIS::MouseButtonID id) const{
	return m_mouse_buttons[id];
}

const bool InputSystem::IsMouseButtonPressed(OIS::MouseButtonID id) const{
	if (!m_last_mouse_buttons[id] && m_mouse_buttons[id]){
		return true;
	}
	return false;
}

const bool InputSystem::IsMouseButtonReleased(OIS::MouseButtonID id) const{
	if (m_last_mouse_buttons[id] && !m_mouse_buttons[id]){
		return true;
	}
	return false;
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
		m_game->ButtonPressed(BTN_UP);
		break;
	case OIS::KC_DOWN:
		m_game->ButtonPressed(BTN_DOWN);
		break;
	case OIS::KC_RIGHT:
		m_game->ButtonPressed(BTN_RIGHT);
		break;
	case OIS::KC_LEFT:
		m_game->ButtonPressed(BTN_LEFT);
		break;
	case OIS::KC_W:
		m_game->ButtonPressed(BTN_W);
		break;
	case OIS::KC_S:
		m_game->ButtonPressed(BTN_S);
		break;
	case OIS::KC_A:
		m_game->ButtonPressed(BTN_A);
		break;
	case OIS::KC_D:
		m_game->ButtonPressed(BTN_D);
		break;
	case OIS::KC_SPACE:
		m_game->ButtonPressed(BTN_START);
		break;
	case OIS::KC_ESCAPE:
		m_game->ButtonPressed(BTN_BACK);
		break;
	default:
		break;
	};
	return true;
}

bool InputSystem::keyReleased(const OIS::KeyEvent& e){
	switch (e.key){
	case OIS::KC_UP:
		m_game->ButtonReleased(BTN_UP);
		break;
	case OIS::KC_DOWN:
		m_game->ButtonReleased(BTN_DOWN);
		break;
	case OIS::KC_RIGHT:
		m_game->ButtonReleased(BTN_RIGHT);
		break;
	case OIS::KC_LEFT:
		m_game->ButtonReleased(BTN_LEFT);
		break;
	case OIS::KC_W:
		m_game->ButtonReleased(BTN_W);
		break;
	case OIS::KC_S:
		m_game->ButtonReleased(BTN_S);
		break;
	case OIS::KC_A:
		m_game->ButtonReleased(BTN_A);
		break;
	case OIS::KC_D:
		m_game->ButtonReleased(BTN_D);
		break;
	case OIS::KC_SPACE:
		m_game->ButtonReleased(BTN_START);
		break;
	case OIS::KC_ESCAPE:
		m_game->ButtonReleased(BTN_BACK);
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
		m_game->ButtonPressed(BTN_RIGHT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left:
		m_game->ButtonPressed(BTN_LEFT_MOUSE);
		break;
	default:
		break;
	}
	return true;
}

bool InputSystem::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id){
	switch (id){
	case 1: //OIS::MouseButtonID::MB_Right:
		m_game->ButtonReleased(BTN_RIGHT_MOUSE);
		break;
	case 0: //OIS::MouseButtonID::MB_Left:
		m_game->ButtonReleased(BTN_LEFT_MOUSE);
		break;
	default:
		break;
	}
	return true;
}

bool InputSystem::povMoved(const OIS::JoyStickEvent& e, int pov){
	for (auto it = m_joystick_listeners.begin(); it != m_joystick_listeners.end(); it++){
		if (!(*it)->povMoved(e, pov)) { break; }
	}
	return true;
}

bool InputSystem::axisMoved(const OIS::JoyStickEvent& e, int axis){
	for (auto it = m_joystick_listeners.begin(); it != m_joystick_listeners.end(); it++){
		if (!(*it)->axisMoved(e, axis)) { break; }
	}
	return true;
}

bool InputSystem::sliderMoved(const OIS::JoyStickEvent& e, int sliderID){
	for (auto it = m_joystick_listeners.begin(); it != m_joystick_listeners.end(); it++){
		if (!(*it)->sliderMoved(e, sliderID)) { break; }
	}
	return true;
}

bool InputSystem::buttonPressed(const OIS::JoyStickEvent& e, int button){
	for (auto it = m_joystick_listeners.begin(); it != m_joystick_listeners.end(); it++){
		if (!(*it)->buttonPressed(e, button)) { break; }
	}
	return true;
}

bool InputSystem::buttonReleased(const OIS::JoyStickEvent& e, int button){
	for (auto it = m_joystick_listeners.begin(); it != m_joystick_listeners.end(); it++){
		if (!(*it)->buttonReleased(e, button)) { break; }
	}
	return true;
}

void InputSystem::windowResized(Ogre::RenderWindow* rw){
	SetWindowExtents((int)rw->getWidth(), (int)rw->getHeight());
}

void InputSystem::windowClosed(Ogre::RenderWindow* rw){
	
}