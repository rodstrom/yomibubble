#include "stdafx.h"
#include "AnimationManager.h"
#include "..\Components\ComponentMessenger.h"

AnimationManager::AnimationManager(ComponentMessenger* messenger) : m_messenger(messenger){}
AnimationManager::~AnimationManager(void){}


void AnimationManager::AddAnimation(const AnimationDef& anim_def){
	m_animations.push_back(anim_def);
}

void AnimationManager::PlayAnimation(const Ogre::String& anim_id, bool loop, bool wait){
	for (unsigned int i = 0; i < m_animations.size(); i++){
		if (m_animations[i].id == anim_id){
			AnimationMsg msg;
			msg.index = m_animations[i].index;
			msg.id = m_animations[i].id;
			msg.loop = loop;
			msg.wait = wait;
			m_messenger->Notify(MSG_ANIMATION_PLAY, &msg);
			if (m_animations[i].full_body){
				int p = 1;
				m_messenger->Notify(MSG_ANIMATION_PAUSE, &p);
			}
			break;
		}
	}
}

void AnimationManager::QueueAnimation(const Ogre::String& anim_id, bool loop){
	for (unsigned int i = 0; i < m_animations.size(); i++){
		if (m_animations[i].id == anim_id){
			AnimationMsg msg;
			msg.index = m_animations[i].index;
			msg.id = m_animations[i].id;
			msg.loop = loop;
			if (m_animations[i].full_body){
				msg.full_body = true;
			}
			m_messenger->Notify(MSG_ANIMATION_QUEUE, &msg);
			break;
		}
	}
}