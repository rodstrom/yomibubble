#ifndef _PHYSICS_PREREQ_H_
#define _PHYSICS_PREREQ_H_

#define BIT(x) (1<<(x))
enum ECollisionFilters {
	COL_NOTHING = 0,
	COL_PLAYER = BIT(0),
	COL_TOTT = BIT(1),
	COL_BUBBLE = BIT(2),
	COL_BUBBLE_TRIG = BIT(3),
	COL_WORLD_TRIGGER = BIT(4),
	COL_WORLD_STATIC = BIT(5),
	COL_CAMERA = BIT(6),
	COL_QUESTITEM = BIT(7)
};

#endif // _PHYSICS_PREREQ_H_