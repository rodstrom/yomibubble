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
	GAME_OBJECT_SIZE
};

struct TriggerDef{
	TriggerDef(void) : body_type(0), x(0.0f), y(0.0f), radius(0.0f), collider_type(0), mass(0.0f){}
	int body_type;
	int collider_type;
	float x;
	float y;
	float z;
	float radius;
	float mass;
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

struct CollisionDef {
	short flag;
	void* data;
};

enum ECollisionTypes{
	COLLISION_FLAG_STATIC = 0,
	COLLISION_FLAG_GAME_OBJECT
};

#endif // _N_GAME_OBJECT_PREREQ_H_