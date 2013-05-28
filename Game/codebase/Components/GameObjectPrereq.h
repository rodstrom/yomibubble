#ifndef _N_GAME_OBJECT_PREREQ_H_
#define _N_GAME_OBJECT_PREREQ_H_

enum EGameObject{
	GAME_OBJECT_PLAYER = 0,
	GAME_OBJECT_TOTT,
	GAME_OBJECT_PINK_BUBBLE,
	GAME_OBJECT_BLUE_BUBBLE,
	GAME_OBJECT_OVERLAY,
	GAME_OBJECT_GUI,
	GAME_OBJECT_PLANE,
	GAME_OBJECT_LEAF,
	GAME_OBJECT_TRIGGER_TEST,
	GAME_OBJECT_BUTTON,
	GAME_OBJECT_COMPANION,
	GAME_OBJECT_TERRAIN,
	GAME_OBJECT_GATE,
	GAME_OBJECT_LOADING_SCREEN,
	GAME_OBJECT_NEXT_LEVEL,
	GAME_OBJECT_CAMERA,
	GAME_OBJECT_PARTICLE,
	GAME_OBJECT_QUEST_ITEM,
	GAME_OBJECT_SPEECH_BUBBLE,
	GAME_OBJECT_ROCK_SLIDE,
	GAME_OBJECT_SIZE
};

struct CollisionFilter {
	CollisionFilter(void) : filter(0), mask(0) {}
	int filter;		// Collision identifier, what the object "is" (eg: Player)
	int mask;		// What the collision shape will collide with (must go both ways)
};

struct TriggerDef{
	TriggerDef(void) : body_type(0), x(0.0f), y(0.0f), radius(0.0f), collider_type(0), mass(0.0f), offset(Ogre::Vector3::ZERO){}
	int body_type;
	int collider_type;
	float x;
	float y;
	float z;
	float radius;
	float mass;
	Ogre::Vector3 offset;
	CollisionFilter collision_filter;
};

struct PlaneDef{
	PlaneDef(void) : plane_name(Ogre::StringUtil::BLANK), material_name(Ogre::StringUtil::BLANK), friction(0.0f), restitution(0.0f){}
	PlaneDef(const Ogre::String& p_plane_name, const Ogre::String& p_material_name, float p_friction, float p_restitution) :
		plane_name(p_plane_name), material_name(p_material_name), friction(p_friction), restitution(p_restitution){}
	Ogre::String plane_name;
	Ogre::String material_name;
	float friction;
	float restitution;
	CollisionFilter collision_filter;
};

struct CameraDataDef{
	Ogre::Vector3 player_direction;
};

struct CollisionDef {
	short flag;
	void* data;
};

enum ECollisionTypes{
	COLLISION_FLAG_STATIC = 0,
	COLLISION_FLAG_GAME_OBJECT
};

struct CharacterControllerDef{
	CharacterControllerDef(void) : step_height(0.0f), turn_speed(0.0f), velocity(0.0f), max_jump_height(0.0f), friction(0.0f), mass(0.0f),
		restitution(0.0f), jump_power(0.0f), max_speed(0.0f), deceleration(0.0f), air_deceleration(0.0f), radius(0.0f), height(0.0f), offset(Ogre::Vector3::ZERO), mesh("") {}
	float step_height;
	float turn_speed;
	float velocity;
	float max_jump_height;
	float jump_power;
	float friction;
	float restitution;
	float max_speed;
	float deceleration;
	float air_deceleration;
	float radius;
	float height;
	float mass;
	Ogre::String mesh;
	float fall_acceleration;
	float max_fall_speed;
	Ogre::Vector3 offset;
	CollisionFilter collision_filter;
};

struct TottDef{
	TottDef(void) : mesh_name(""), quest_object_mesh_name(""), type_name(""), sb_node_name(""), theme_music(""), play_music(false),
					sfx_happy(""), sfx_curious(""), idle_animation(""), walk_animation(""), run_animation(""), react_animation(""),
					happy_animation(""){}
	CharacterControllerDef	character_controller;
	Ogre::String			mesh_name;
	Ogre::String			quest_object_mesh_name;
	Ogre::String			type_name;
	Ogre::String			sb_node_name;
	Ogre::String			theme_music;
	bool					play_music;
	Ogre::String			sfx_happy;
	Ogre::String			sfx_curious;
	Ogre::String			idle_animation;
	Ogre::String			walk_animation;
	Ogre::String			run_animation;
	Ogre::String			react_animation;
	Ogre::String			happy_animation;
	Ogre::String			mesh;
};

struct PlayerDef{
	CharacterControllerDef* character_contr;
	Ogre::String level_id;
	float camera_speed;
};

struct RigidBodyDef{
	RigidBodyDef(void) : collider_type(0.0f), mass(0.0f), body_type(0), restitution(0.0f), friction(0.0f), rolling_friction(0.0f) {}
	RigidBodyDef(int p_collider_type, float p_mass) : collider_type(p_collider_type), mass(p_mass) {} 
	int body_type;
	int collider_type;
	float mass;
	float restitution;
	float friction;
	float rolling_friction;
	CollisionFilter collision_filter;
};

#endif //GAME_OBJECT_PREREQ_H
