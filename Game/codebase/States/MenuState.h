#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include "State.h"

#include "..\Components\GameObject.h"

class FadeInFadeOut;
class MenuState : public State
{
public:
	MenuState(void);
	~MenuState(void);

	bool Update(float dt);
	void Enter();
	void Exit();
	void ChangeStateToPlayState();
	void ChangeStateToOptions();
	void ChangeStateToCredits();
	void ChangeStateToExit();

	//DECLARE_STATE_CLASS(MenuState);

private:
	GameObjectManager*			m_game_object_manager;
	PhysicsEngine*				m_physics_engine;
	SoundManager*				m_sound_manager;
	bool						m_quit;
	int							m_current_selected_button;
	GameObject*					m_buttons[4];
	OverlayDef					m_overlayDef;
	Ogre::SceneNode*			m_house_node;
	Ogre::SceneNode*			m_yomi_node;
	Ogre::SceneNode*			m_mushroom_node;
	Ogre::SceneNode*			m_tree_node;
	Ogre::AnimationState*		m_animation_state;
};



#endif // _MENU_STATE_H_