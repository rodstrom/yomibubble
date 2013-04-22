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
	CharControllerDef(void) : collider_type(0), step_height(0.0f), turn_speed(0.0f), velocity(0.0f) {}
	CharControllerDef(int p_collider_type, float p_step_height, float p_turn_speed , float p_velocity) : 
		collider_type(p_collider_type), step_height(p_step_height), turn_speed(p_turn_speed), velocity(p_velocity){}
	int collider_type;
	float step_height;
	float turn_speed;
	float velocity;
};

struct RigidbodyDef{
	RigidbodyDef(void) : collider_type(0.0f), mass(0.0f) {}
	RigidbodyDef(int p_collider_type, float p_mass) : collider_type(p_collider_type), mass(p_mass) {} 
	int collider_type;
	float mass;
};

struct PlaneDef{
	PlaneDef(void) : plane_name(Ogre::StringUtil::BLANK), material_name(Ogre::StringUtil::BLANK){}
	PlaneDef(const Ogre::String& p_plane_name, const Ogre::String& p_material_name) :
		plane_name(p_plane_name), material_name(p_material_name){}
	Ogre::String plane_name;
	Ogre::String material_name;
};

#endif // _N_GAME_OBJECT_PREREQ_H_