#include "stdafx.h"
#include "PlayerStateManager.h"
#include "..\Components\PlayerState.h"

PlayerStateManager::PlayerStateManager(void) : m_blow_bubbles(false), m_holding_object(false), m_active(NULL), m_blow_bubble_state(NULL), m_holding_object_state(NULL){}
PlayerStateManager::~PlayerStateManager(void){}

void PlayerStateManager::Update(float dt){
	if (m_active){
		m_active->Update(dt);
	}
	if (m_blow_bubbles){
		m_blow_bubble_state->Update(dt);
	}
	if (m_holding_object){
		m_holding_object_state->Update(dt);
	}
}

PlayerState* PlayerStateManager::GetPlayerState(int type){
	for (unsigned int i = 0; i < m_player_states.size(); i++){
		if (m_player_states[i]->GetType() == type){
			return m_player_states[i];
		}
	}
	return NULL;
}

void PlayerStateManager::SetPlayerState(PlayerState* state){
	if (m_active){
		m_active->Exit();
	}
	m_active = state;
	m_active->Enter();
}

void PlayerStateManager::BlowBubble(bool value){
	m_blow_bubbles = value;
	if (value){
		m_blow_bubble_state->Enter();
	}
	else {
		m_blow_bubble_state->Exit();
	}
}

void PlayerStateManager::HoldObject(bool value){
	m_holding_object = value;
	if (value){
		m_holding_object_state->Enter();
	}
	else {
		m_holding_object_state->Exit();
	}
}

int PlayerStateManager::GetCurrentType(){
	if (m_active){
		return m_active->GetType();
	}
	return 0;
}

void PlayerStateManager::Init(){
	m_blow_bubble_state = GetPlayerState(PLAYER_STATE_BLOW_BUBBLE);
	m_holding_object_state = GetPlayerState(PLAYER_STATE_HOLD_OBJECT);
}