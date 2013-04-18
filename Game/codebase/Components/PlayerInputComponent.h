#ifndef _N_PLAYER_CONTROLLER_H_
#define _N_PLAYER_CONTROLLER_H_

#include "ComponentsPrereq.h"
#include "..\Managers\SoundManager.h"

class InputManager;
class PlayerInputComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	PlayerInputComponent(void) : m_input_manager(NULL), m_current_bubble(NULL), m_is_creating_bubble(false), m_bubble_type(0),
		m_max_scale(2.0f), m_current_scale(0.0f){ m_type = COMPONENT_PLAYER_INPUT; }
	virtual ~PlayerInputComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(InputManager* input_manager, SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	enum EBubbleType{
		BUBBLE_TYPE_BLUE = 0,
		BUBBLE_TYPE_PINK
	};
	InputManager* m_input_manager;
	GameObject* m_current_bubble;
	int m_bubble_type;
	bool m_is_creating_bubble;
	float m_max_scale;
	float m_current_scale;
	SoundData2D m_walk_sound;
	SoundData2D m_def_music;
	SoundData3D m_3D_music_data;
};

#endif // _N_PLAYER_CONTROLLER_H_