#ifndef _PLAYER_STATE_H_
#define _PLAYER_STATE_H_

#include "ComponentsPrereq.h"
#include "..\Managers\SoundManager.h"

class PhysicsEngine;
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
	static void SetBubbleProperties(const BubblePropertiesDef& free_blue_bubble_def, const BubblePropertiesDef& stand_on_blue_bubble_def, const BubblePropertiesDef& inside_blue_bubble_def, const BubblePropertiesDef& free_pink_bubble_def, const BubblePropertiesDef& stand_on_pink_bubble_def, const BubblePropertiesDef& inside_pink_bubble_def){
		s_free_blue_bubble_def = free_blue_bubble_def;
		s_stand_on_blue_bubble_def = stand_on_blue_bubble_def;
		s_inside_blue_bubble_def = inside_blue_bubble_def;
		s_free_pink_bubble_def = free_pink_bubble_def;
		s_stand_on_pink_bubble_def = stand_on_pink_bubble_def;
		s_inside_pink_bubble_def = inside_pink_bubble_def;
	}
protected:
	short m_type;
	static ComponentMessenger* s_messenger;
	static AnimationManager* s_animation;
	static PlayerInputComponent* s_input_component;
	static PlayerStateManager* s_manager;
	static SoundManager* s_sound_manager;
	static BubblePropertiesDef s_free_blue_bubble_def;
	static BubblePropertiesDef s_stand_on_blue_bubble_def;
	static BubblePropertiesDef s_inside_blue_bubble_def;
	static BubblePropertiesDef s_free_pink_bubble_def;
	static BubblePropertiesDef s_stand_on_pink_bubble_def;
	static BubblePropertiesDef s_inside_pink_bubble_def;
};

class PlayerIdle : public PlayerState{
public:
	PlayerIdle(void);
	~PlayerIdle(void){}
	void Enter();
	void Exit();
	void Update(float dt);
private:
	Ogre::String m_current_idle_base;
	Ogre::String m_current_idle_top;
	float m_timer;
	float m_target_time;
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
	PlayerBlowBubble(PhysicsEngine* physics_engine);
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
	bool m_hit_wall;
	float m_min_bubble_size;
	float m_max_bubble_size;
	float m_current_scale;
	float m_bubble_gravity;
	Ogre::SceneNode* m_child_node;
	Ogre::SceneNode* m_player_node;
	btRigidBody* m_player_body;
	btRigidBody* m_trigger_body;
	PhysicsEngine* m_physics_engine;
};

class PlayerJump : public PlayerState{
public:
	PlayerJump(void);
	~PlayerJump(void){}
	void Enter();
	void Exit();
	void Update(float dt);

	SoundData2D m_jump_sfx_1;
	SoundData2D m_jump_sfx_2;
	SoundData2D m_jump_sfx_3;
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
	PlayerLand(void);
	~PlayerLand(void){}
	void Enter();
	void Exit();
	void Update(float dt);

	SoundData2D m_land_sfx_1;
	SoundData2D m_land_sfx_2;

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
	void ChangeTargetTime(float low, float high);
	MessageSystem* m_message_system;
	float m_on_bubble_y_offset;
	GameObject* m_bubble;
	Ogre::String m_current_idle;
	Ogre::String m_current_walk;
	float m_timer;			// when to change animation
	float m_target_time;
};

class PlayerInsideBubble : public PlayerState{
public:
	PlayerInsideBubble(PhysicsEngine* physics_engine, MessageSystem* message_system);
	~PlayerInsideBubble(void);
	void Enter();
	void Exit();
	void Update(float dt);
	GameObject* GetBubbleObject() const { return m_bubble; }
private:
	void ChangeBubbleType();
	void BubbleRemoved(IEvent* evt);
	PhysicsEngine* m_physics_engine;
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

class PlayerHoldObject : public PlayerState{
public:
	PlayerHoldObject(PhysicsEngine* physics_engine, MessageSystem* message_system);
	~PlayerHoldObject(void);
	void Enter();
	void Exit();
	void Update(float dt);
private:
	void ItemRemoved(IEvent* evt);
	GameObject* m_object;		// the object we are holding
	MessageSystem* m_message_system;
	PhysicsEngine* m_physics_engine;
	float m_bubble_gravity;
};

class PlayerLeafCollect : public PlayerState{
public:
	PlayerLeafCollect(MessageSystem* message_system);
	~PlayerLeafCollect(void);
	void Enter();
	void Exit();
	void Update(float dt);

	void GetLeaf(IEvent* evt);

private:
	bool m_is_dancing;
	MessageSystem* m_message_system;
	GameObject* m_leaf_object;
	Ogre::SceneNode* m_leaf_node;
	Ogre::SceneNode* m_player_node;
};

#endif // _PLAYER_STATE_H_