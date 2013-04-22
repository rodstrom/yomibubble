#ifndef _N_GAME_OBJECT_PREREQ_H_
#define _N_GAME_OBJECT_PREREQ_H_

enum EGameObject{
	GAME_OBJECT_PLAYER = 0,
	GAME_OBJECT_TOTT,
	GAME_OBJECT_PINK_BUBBLE,
	GAME_OBJECT_BLUE_BUBBLE,
	GAME_OBJECT_OVERLAY,
	GAME_OBJECT_PLANE,
	GAME_OBJECT_LEAF,
	GAME_OBJECT_SIZE
};

struct CharControllerDef{
	CharControllerDef(void) : step_height(0.0f), turn_speed(0.0f), velocity(0.0f), max_jump_height(0.0f), friction(0.0f), restitution(0.0f), jump_power(0.0f) {}
	CharControllerDef(float p_step_height, float p_turn_speed , float p_velocity, float p_max_jump_height, float p_friction, float p_restitution, float p_jump_power) : 
		step_height(p_step_height), turn_speed(p_turn_speed), velocity(p_velocity), max_jump_height(p_max_jump_height),
		friction(p_friction), restitution(p_restitution), jump_power(p_jump_power){}
	float step_height;
	float turn_speed;
	float velocity;
	float max_jump_height;
	float jump_power;
	float friction;
	float restitution;
};

struct RigidbodyDef{
	RigidbodyDef(void) : collider_type(0.0f), mass(0.0f) {}
	RigidbodyDef(int p_collider_type, float p_mass) : collider_type(p_collider_type), mass(p_mass) {} 
	int collider_type;
	float mass;
};

struct PlaneDef{
	PlaneDef(void) : plane_name(Ogre::StringUtil::BLANK), material_name(Ogre::StringUtil::BLANK), friction(0.0f), restitution(0.0f){}
	PlaneDef(const Ogre::String& p_plane_name, const Ogre::String& p_material_name, float p_friction, float p_restitution) :
		plane_name(p_plane_name), material_name(p_material_name), friction(p_friction), restitution(p_restitution){}
	Ogre::String plane_name;
	Ogre::String material_name;
	float friction;
	float restitution;
};

#endif // _N_GAME_OBJECT_PREREQ_H_