#include "..\stdafx.h"
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

void InputManager::ButtonPressed(int btn){
	m_buttons[btn] = true;
}

void InputManager::ButtonReleased(int btn){
	m_buttons[btn] = false;
}

void InputManager::Update(){
	memcpy(m_last_buttons, m_buttons, sizeof(bool) * BTN_SIZE);
}

void InputManager::SetMousePosition(int x, int y, int rel_x, int rel_y){
	m_mouse_position.x = x;
	m_mouse_position.y = y;
	m_mouse_position.rel_x = rel_x;
	m_mouse_position.rel_y = rel_y;
}
