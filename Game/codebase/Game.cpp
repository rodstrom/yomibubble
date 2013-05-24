#include "stdafx.h"
#include "Game.h"
#include "Managers\StateManager.h"
#include "States\MenuState.h"
#include "States\PlayState.h"
#include "States\PauseState.h"
#include "States\LoadingState.h"
#include "States\IntroLogoState.h"
#include "States\IntroMovieState.h"

Game::Game(void) : m_state_manager(NULL){}
Game::~Game(void){}

bool Game::Update(float dt){
	return m_state_manager->Update(dt);
}

bool Game::Init(Ogre::RenderWindow* render_window, MessageSystem* message_system, SoundManager* sound_manager){
	m_state_manager = new StateManager(render_window, this, message_system, sound_manager);
	PlayState::Create<PlayState>(m_state_manager, "PlayState");
	MenuState::Create<MenuState>(m_state_manager, "MenuState");
	PauseState::Create<PauseState>(m_state_manager, "PauseState");
	LoadingState::Create<LoadingState>(m_state_manager, "LoadingState");
	//IntroLogoState::Create<IntroLogoState>(m_state_manager, "LogoState");
	IntroMovieState::Create<IntroMovieState>(m_state_manager, "IntroMovieState");
	m_state_manager->ChangeState(m_state_manager->FindById("MenuState"), true);

	//ShowCursor(false);

	return true;
}

void Game::UpdateInput(){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->Update();
		}
	}
}

void Game::Shut(){
	if (m_state_manager){
		m_state_manager->Shut();
		delete m_state_manager;
		m_state_manager = NULL;
	}
}

void Game::InjectMouseState(const OIS::MouseState mouse_state){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectMouseState(mouse_state);
		}
	}
}

void Game::InjectPressedButton(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectPressedButton(btn);
		}
	}
}

void Game::InjectReleasedButton(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectReleasedButton(btn);
		}
	}
}

void Game::InjectRelativeMovement(float x, float z){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeMovement(x, z);
		}
	}
}

void Game::InjectRelativeMovementX(float x){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeMovementX(x);
		}
	}
}

void Game::InjectRelativeMovementZ(float z){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeMovementZ(z);
		}
	}
}

void Game::InjectRelativeCameraAxis(float x, float y, float z){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeCameraAxis(x, y, z);
		}
	}
}

void Game::InjectRelativeCameraAxisX(float x){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeCameraAxisX(x);
		}
	}
}

void Game::InjectRelativeCameraAxisY(float y){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeCameraAxisY(y);
		}
	}
}

void Game::InjectRelativeCameraAxisZ(float z){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectRelativeCameraAxisZ(z);
		}
	}
}

void Game::InjectMousePosition(int x, int y){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->InjectMousePosition(x,y);
		}
	}
}

void Game::AddInputManager(InputManager* input_manager){
	m_input_managers.push_back(input_manager);
}

void Game::RemoveInputManager(InputManager* input_manager){
	std::vector<InputManager*>::iterator it = std::find(m_input_managers.begin(), m_input_managers.end(), input_manager);
	if (it != m_input_managers.end()){
		m_input_managers.erase(it);
	}
}