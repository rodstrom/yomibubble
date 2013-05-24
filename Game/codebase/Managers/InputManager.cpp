#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager(void) : m_pause(false) {
	std::fill(m_buttons, m_buttons + BTN_SIZE, false);
	std::fill(m_last_buttons, m_last_buttons + BTN_SIZE, false);
}

InputManager::~InputManager(void){}

bool InputManager::IsButtonDown(int p_button){
	if (m_pause) return false;
	return m_buttons[p_button];
}

bool InputManager::IsButtonPressed(int p_button){
	if (m_pause) return false;
	if (m_buttons[p_button] && !m_last_buttons[p_button])
		return true;
	return false;
}

bool InputManager::IsButtonReleased(int p_button){
	if (m_pause) return false;
	if (!m_buttons[p_button] && m_last_buttons[p_button])
		return true;
	return false;
}

void InputManager::InjectPressedButton(int btn){
	m_buttons[btn] = true;
}

void InputManager::InjectReleasedButton(int btn){
	m_buttons[btn] = false;
}

void InputManager::Update(){
	memcpy(m_last_buttons, m_buttons, sizeof(bool) * BTN_SIZE);
}

void InputManager::InjectMouseState(const OIS::MouseState mouse_state){
	m_mouse_state = mouse_state;
}

void InputManager::InjectRelativeMovement(float x, float z){
	m_movement_axis.x = x;
	m_movement_axis.z = z;
}

void InputManager::InjectRelativeCameraAxis(float x, float y, float z){
	m_camera_axis.x = x;
	m_camera_axis.y = y;
	m_camera_axis.z = z;
}

void InputManager::InjectRelativeMovementX(float x){
	m_movement_axis.x = x;
}

void InputManager::InjectRelativeMovementZ(float z){
	m_movement_axis.z = z;
}

void InputManager::InjectRelativeCameraAxisX(float x){
	m_camera_axis.x = x;
}

void InputManager::InjectRelativeCameraAxisY(float y){
	m_camera_axis.y = y;
}

void InputManager::InjectRelativeCameraAxisZ(float z){
	m_camera_axis.z = z;
}

void InputManager::InjectMousePosition(int x, int y){
	m_mouse_position.x = x;
	m_mouse_position.y = y;
}