#ifndef _N_PLAYER_CONTROLLER_H_
#define _N_PLAYER_CONTROLLER_H_

#include "ComponentsPrereq.h"
#include "..\Managers\SoundManager.h"

class InputManager;
class PlayerInputComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	PlayerInputComponent(void) : m_input_manager(NULL), m_current_bubble(NULL), m_is_creating_bubble(false), m_bubble_type(0),
		m_max_scale(2.0f), m_current_scale(0.0f), m_player_state(0), m_acc_x(0.0f), m_acc_z(0.0f), m_max_velocity(0.0f), m_deacc(0.0f), m_velocity(0.0f)
	{ m_type = COMPONENT_PLAYER_INPUT; }
	virtual ~PlayerInputComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(InputManager* input_manager, SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);
	int GetPlayerState() { return m_player_state; }
	void SetVelocity(float value) { m_velocity = value; }
	void SetMaxVelocity(float value) { m_max_velocity = value; }
	void SetDeacceleration(float value) { m_deacc = value; }

protected:
	void Normal(float dt);
	void OnBubble(float dt);
	void InsideBubble(float dt);
	void Bouncing(float dt);
	void Acceleration(Ogre::Vector3& dir, Ogre::Vector3& acc, float dt);

	enum EBubbleType{
		BUBBLE_TYPE_BLUE = 0,
		BUBBLE_TYPE_PINK
	};
	typedef void (PlayerInputComponent::*ControllerFptr)(float);

	InputManager* m_input_manager;
	GameObject* m_current_bubble;
	int m_bubble_type;
	bool m_is_creating_bubble;
	float m_max_scale;
	float m_current_scale;
	SoundData2D m_walk_sound;
	SoundData2D m_test_sfx;
	SoundData2D m_def_music;
	SoundData3D m_3D_music_data;
	int m_player_state;
	ControllerFptr m_states[PLAYER_STATE_SIZE];
	float m_velocity;
	float m_deacc;
	float m_acc_x;
	float m_acc_z;
	float m_max_velocity;

};

#endif // _N_PLAYER_CONTROLLER_H_