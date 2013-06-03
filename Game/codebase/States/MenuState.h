#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include "State.h"

#include "..\Components\GameObject.h"
#include "PagedGeometry.h"
#include "..\Waypoint.h"

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
	void RemoveWeights(std::vector<std::string>& list, Ogre::Animation* anim);
	void FixYomiWeights();
	bool NextLocation();
	bool WithinDistance(float meters);

	//DECLARE_STATE_CLASS(MenuState);

private:
	WayPoint*					m_way_point;
	GameObjectManager*			m_game_object_manager;
	PhysicsEngine*				m_physics_engine;
	SoundManager*				m_sound_manager;
	bool						m_quit;
	int							m_current_selected_button;
	GameObject*					m_buttons[4];
	OverlayDef					m_overlayDef;
	Ogre::SceneNode*			m_yomi_node;
	Ogre::SceneNode*			m_tott_node;
	Ogre::SceneNode*			m_scene_node;
	Ogre::SceneNode*			m_menu_grass;
	Ogre::SceneNode*			m_menu_bush;
	Ogre::SceneNode*			m_follow_node;
	Ogre::SceneNode*			m_blue_bubble_node;
	Ogre::SceneNode*			m_blue_bubble_node1;
	Ogre::SceneNode*			m_pink_bubble_node;
	Ogre::AnimationState*		m_animation_state;
	Ogre::Entity*				m_yomi_ent;
	Ogre::Entity*				m_tott;
	Ogre::AnimationState*		m_yomi_base;
	Ogre::AnimationState*		m_yomi_top;
	Ogre::AnimationState*		m_tott_animation;
	float						m_anim_timer;
	float						m_target_time;
	Forests::PagedGeometry*		m_paged_geometry;
	Ogre::Vector3				m_destination;
	Ogre::Vector3				m_direction;
	float						m_speed;

};



#endif // _MENU_STATE_H_