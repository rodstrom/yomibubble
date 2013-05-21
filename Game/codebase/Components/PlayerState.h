#ifndef _PLAYER_STATE_H_
#define _PLAYER_STATE_H_

#include "ComponentsPrereq.h"
#include "..\Managers\SoundManager.h"

struct IEvent;
class MessageSystem;
class GameObject;
class PlayerStateManager;
class PlayerInputComponent;
class AnimationManager;
class ComponentMessenger;
class PlayerState
{
public:
	virtual ~PlayerState(void){}
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float dt) = 0;
	short GetType() const { return m_type; }
	static void Init(ComponentMessenger* messenger, AnimationManager* animation, PlayerInputComponent* input_component, PlayerStateManager* manager, SoundManager* sound_manager){ 
		s_messenger = messenger; s_animation = animation; s_input_component = input_component;  s_manager = manager; s_sound_manager = sound_manager; }
protected:
	short m_type;
	static ComponentMessenger* s_messenger;
	static AnimationManager* s_animation;
	static PlayerInputComponent* s_input_component;
	static PlayerStateManager* s_manager;
	static SoundManager* s_sound_manager;
};

class PlayerIdle : public PlayerState{
public:
	PlayerIdle(void);
	~PlayerIdle(void){}
	void PlayTopIdle();
	void Enter();
	void Exit();
	void Update(float dt);
};

class PlayerStateMove : public PlayerState{
public:
	PlayerStateMove(void);
	~PlayerStateMove(void){}
	void PlayTopAnimation();
	void Enter();
	void Exit();
	void Update(float dt);
private:
	Ogre::String m_current_top;
	Ogre::String m_current_base;
	SoundData2D m_walk_sound;
};

class PlayerBlowBubble : public PlayerState{
public:
	PlayerBlowBubble(void);
	~PlayerBlowBubble(void){}
	void Enter();
	void Exit();
	void Update(float dt);
private:
	enum EBubbleType{
		BUBBLE_TYPE_BLUE = 0,
		BUBBLE_TYPE_PINK
	};
	short m_bubble_type;
    void CreateTriggerForBubble();
	GameObject* m_bubble;	// the bubble that we will create
	SoundData2D m_bubble_blow_sound;
	float m_min_bubble_size;
	float m_max_bubble_size;
	float m_current_scale;
	float m_bubble_gravity;
};

class PlayerJump : public PlayerState{
public:
	PlayerJump(void);
	~PlayerJump(void){}
	void Enter();
	void Exit();
	void Update(float dt);
};

class PlayerFalling : public PlayerState{
public:
	PlayerFalling(void){ m_type = PLAYER_STATE_FALLING; }
	~PlayerFalling(void){}
	void Enter();
	void Exit();
	void Update(float dt);
};

class PlayerLand : public PlayerState{
public:
	PlayerLand(void){ m_type = PLAYER_STATE_LAND; }
	~PlayerLand(void){}
	void Enter();
	void Exit();
	void Update(float dt);
private:
	void Proceed();
};

class PlayerOnBubble : public PlayerState{
public:
	PlayerOnBubble(MessageSystem* message_system);
	~PlayerOnBubble(void);
	void Enter();
	void Exit();
	void Update(float dt);
private:
	void BubbleRemoved(IEvent* evt);
	MessageSystem* m_message_system;
	float m_on_bubble_y_offset;
	GameObject* m_bubble;
};

class PlayerInsideBubble : public PlayerState{
public:
	PlayerInsideBubble(MessageSystem* message_system);
	~PlayerInsideBubble(void);
	void Enter();
	void Exit();
	void Update(float dt);
private:
	void ChangeBubbleType();
	void BubbleRemoved(IEvent* evt);
	MessageSystem* m_message_system;
	float m_on_bubble_y_offset;
	float m_bubble_gravity;
	GameObject* m_bubble;
};

class PlayerBounce : public PlayerState{
public:
	PlayerBounce(void){ m_type = PLAYER_STATE_BOUNCE; }
	~PlayerBounce(void){}
	void Enter();
	void Exit();
	void Update(float dt);
private:
};


#endif // _PLAYER_STATE_H_