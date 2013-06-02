#include "stdafx.h"
#include "TottAIStates.h"
#include "..\Components\ComponentsPrereq.h"
#include "..\Components\ComponentMessenger.h"

TottAIStateMove::TottAIStateMove(const TottMoveAIDef& def){
	m_state = AI_STATE_MOVE;
	m_animation = def.animation;
	m_target_position = def.target_position;
}

void TottAIStateMove::Enter(){
	AnimationMsg msg;
	msg.id = m_animation;
	msg.loop = true;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
}

void TottAIStateMove::Exit(){
	Ogre::Vector3 dir(0,0,0);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &dir);
}

bool TottAIStateMove::Update(float dt){
	AnimationMsg anim_msg;
	anim_msg.id = m_animation;
	anim_msg.loop = true;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &anim_msg);
	m_messenger->Notify(MSG_NODE_GET_POSITION, &m_current_position);
	Ogre::Vector2 pos(m_current_position.x, m_current_position.z);
	float distance = pos.distance(m_target_position);
	if (distance <= 1.0f){
		return true;
	}
	Ogre::Vector2 dir = m_target_position - pos;
	dir.normalise();
	Ogre::Vector3 new_dir(dir.x, 0.0f, dir.y);
	m_messenger->Notify(MSG_CHARACTER_CONTROLLER_SET_DIRECTION, &new_dir);
	return false;
}

TottAIStateWait::TottAIStateWait(const TottWaitAIDef& def){
	m_state = AI_STATE_WAIT;
	m_animation = def.animation;
	m_target_time = def.target_time;
}

void TottAIStateWait::Enter(){
	AnimationMsg msg;
	msg.id = m_animation;
	msg.loop = true;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
	m_timer = 0.0f;
}

void TottAIStateWait::Exit(){

}

bool TottAIStateWait::Update(float dt){
	AnimationMsg anim_msg;
	anim_msg.id = m_animation;
	anim_msg.loop = true;
	m_messenger->Notify(MSG_ANIMATION_PLAY, &anim_msg);
	m_timer += dt;
	if (m_timer >= m_target_time){
		return true;
	}
	return false;
}