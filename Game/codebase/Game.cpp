#include "stdafx.h"
#include "Game.h"
#include "Managers\StateManager.h"
#include "States\MenuState.h"
#include "States\PlayState.h"

Game::Game(void) : m_state_manager(NULL){}
Game::~Game(void){}

bool Game::Update(float dt){
	return m_state_manager->Update(dt);
}

bool Game::Init(Ogre::RenderWindow* render_window, MessageSystem* message_system){
	m_state_manager = new StateManager(render_window, this, message_system);
	PlayState::Create<PlayState>(m_state_manager, "PlayState");
	MenuState::Create<MenuState>(m_state_manager, "MenuState");
	m_state_manager->ChangeState(m_state_manager->FindById("PlayState"));
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

void Game::SetMouseState(const OIS::MouseState mouse_state){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->SetMouseState(mouse_state);
		}
	}
}

void Game::ButtonPressed(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->ButtonPressed(btn);
		}
	}
}

void Game::ButtonReleased(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->ButtonReleased(btn);
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
