#include "BubbleAdventure.h"
#include "Managers\InputManager.h"
#include "Managers\StateManager.h"
#include "States\MenuState.h"
#include "States\PlayState.h"

BubbleAdventure::BubbleAdventure(void) : m_state_manager(NULL){}
BubbleAdventure::~BubbleAdventure(void){}

bool BubbleAdventure::Init(Ogre::RenderWindow* render_window, MessageSystem* message_system){
	m_state_manager = new StateManager(render_window, this, message_system);
	PlayState::Create<PlayState>(m_state_manager, "PlayState");
	MenuState::Create<MenuState>(m_state_manager, "MenuState");
	m_state_manager->ChangeState(m_state_manager->FindById("PlayState"));
	return true;
}

void BubbleAdventure::Update(){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->Update();
		}
	}
}

void BubbleAdventure::Shut(){
	if (m_state_manager){
		m_state_manager->Shut();
		delete m_state_manager;
		m_state_manager = NULL;
	}
}

void BubbleAdventure::SetMousePosition(int x, int y, int rel_x, int rel_y){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->SetMousePosition(x, y, rel_x, rel_y);
		}
	}
}

void BubbleAdventure::ButtonPressed(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->ButtonPressed(btn);
		}
	}
}

void BubbleAdventure::ButtonReleased(int btn){
	if (!m_input_managers.empty()){
		for (unsigned int i = 0; i < m_input_managers.size(); i++){
			m_input_managers[i]->ButtonReleased(btn);
		}
	}
}

void BubbleAdventure::AddInputManager(InputManager* input_manager){
	m_input_managers.push_back(input_manager);
}

void BubbleAdventure::RemoveInputManager(InputManager* input_manager){
	std::vector<InputManager*>::iterator it = std::find(m_input_managers.begin(), m_input_managers.end(), input_manager);
	if (it != m_input_managers.end()){
		m_input_managers.erase(it);
	}
}
