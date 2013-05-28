#ifndef _N_PLAYER_CONTROLLER_H_
#define _N_PLAYER_CONTROLLER_H_

#include "ComponentsPrereq.h"
#include "GameObjectPrereq.h"
#include "..\Managers\SoundManager.h"

class MessageSystem;
class PlayerStateManager;
class AnimationManager;
class PhysicsEngine;
class InputManager;
class PlayerInputComponent : public Component, public IComponentUpdateable, public IComponentObserver, public IComponentSimulationStep{
public:
	PlayerInputComponent(void) : m_input_manager(NULL), m_current_bubble(NULL), m_is_creating_bubble(false), m_bubble_type(0), m_bubble_create_position(Ogre::Vector3::ZERO),
		m_max_scale(2.0f), m_current_scale(0.0f), m_player_state(0), m_physics_engine(NULL), m_animation_manager(NULL), m_direction(Ogre::Vector3::ZERO), m_can_blow_pink(false)
	{ m_type = COMPONENT_PLAYER_INPUT; m_update = true; }
	virtual ~PlayerInputComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(InputManager* input_manager, SoundManager* sound_manager, PhysicsEngine* physics_engine, MessageSystem* message_system);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void SimulationStep(btScalar time_step);
	int GetPlayerState() { return m_player_state; }
	bool CanBlowPink() const { return m_can_blow_pink; }

	SoundData2D m_leaf_sfx;
	//AnimationMsg m_anim_msg;
	SoundData2D m_bounce_sound;

	void SetCustomVariables(float min_bubble_size, float max_bubble_size, float on_bubble_mod, float in_bubble_mod, Ogre::String level);

	float m_min_bubble_size;
	float m_max_bubble_size;
	float m_on_bubble_speed_mod;
	float m_in_bubble_speed_mod;

	AnimationManager* GetAnimationManager() const { return m_animation_manager; }
	InputManager* GetInputManager() const { return m_input_manager; }
	const Ogre::Vector3& GetDirection() const { return m_direction; }
	bool IsOnGround() const { return m_on_ground; }
	GameObject* GetBubble() const { return m_current_bubble; }

	SoundData2D m_bounce_1;
	SoundData2D m_bounce_2;
	SoundData2D m_bounce_3;
	SoundData2D m_bounce_4;

protected:
	Ogre::Vector3 m_direction;
	AnimationManager* m_animation_manager;
	PlayerStateManager* m_player_state_manager;
	void Jump(float dt);
	void BlowBubble(float dt);

	void CreateTriggerForBubble();
	void CreateBlueBubble(const Ogre::Vector3& pos, const BubbleDef& bubble_def);
	void CreatePinkBubble(const Ogre::Vector3& pos, const BubbleDef& bubble_def);

	void Normal(float dt);
	void OntoBubbleTransition(float dt);
	void IntoBubbleTransition(float dt);
	void OnBubble(float dt);
	void InsideBubble(float dt);
	void Bouncing(float dt);

	int m_player_action;

	enum EBubbleType{
		BUBBLE_TYPE_BLUE = 0,
		BUBBLE_TYPE_PINK
	};
	typedef void (PlayerInputComponent::*ControllerFptr)(float);

	PhysicsEngine* m_physics_engine;
	InputManager* m_input_manager;
	GameObject* m_current_bubble;
	Ogre::Vector3 m_bubble_create_position;
	int m_bubble_type;
	bool m_is_creating_bubble;
	float m_max_scale;
	float m_current_scale;

	SoundData2D m_walk_sound;
	SoundData2D m_jump_sound;
	SoundData2D m_bubble_burst_sound;
	SoundData2D m_bubble_blow_sound;

	SoundData2D m_test_sfx;
	
	SoundData2D m_start_music;
	SoundData2D m_day_music;
	SoundData2D m_night_music;

	SoundData3D m_3D_music_data;
	int m_player_state;
	ControllerFptr m_states[PLAYER_STATE_SIZE];
	bool m_on_ground;
	bool m_can_blow_pink;

	int m_current_level;
	Ogre::String m_level;

	CameraDataDef m_camera_data_def;
};

class BubbleController : public Component, public IComponentObserver, public IComponentSimulationStep, public IComponentUpdateable{
public:
	BubbleController(void) :  m_velocity(0.0f), m_max_velocity(0.0f), m_impulse(Ogre::Vector3::ZERO), m_apply_impulse(false),
		m_run_timer(false), m_life_timer(0.0f), m_ready(false)
	{ m_type = COMPONENT_BUBBLE_CONTROL; m_update = true; }
	virtual ~BubbleController(void){}

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(PhysicsEngine* physics_engine, MessageSystem* message_system, float velocity, float max_velocity);
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);
	virtual void SimulationStep(btScalar time_step);
	void SetVelocity(float value) { m_velocity = value; }
	void SetMaxVelocity(float value) { m_max_velocity = value; }

protected:
	MessageSystem* m_message_system;
	PhysicsEngine* m_physics_engine;
	bool m_apply_impulse;
	Ogre::Vector3 m_impulse;
	float m_velocity;
	float m_max_velocity;
	float m_life_timer;
	float m_max_life_time;
	bool m_run_timer;
	bool m_ready;
};

#endif // _N_PLAYER_CONTROLLER_H_