#ifndef _N_GAME_OBJECT_PREREQ_H_
#define _N_GAME_OBJECT_PREREQ_H_

enum EGameObject{
	GAME_OBJECT_PLAYER = 0,
	GAME_OBJECT_TOTT,
	GAME_OBJECT_PINK_BUBBLE,
	GAME_OBJECT_BLUE_BUBBLE,
	GAME_OBJECT_OVERLAY,
	GAME_OBJECT_SIZE
};

struct Overlay2DDef{
	Overlay2DDef(void) : dimension(Ogre::Vector2::ZERO), material_name(Ogre::StringUtil::BLANK){}
	Overlay2DDef(const Ogre::Vector2& dim, const Ogre::String& on, const Ogre::String& mn) : dimension(dim), overlay_name(on), material_name(mn){}
	Ogre::Vector2 dimension;
	Ogre::String material_name;
	Ogre::String overlay_name;
	
};

#endif // _N_GAME_OBJECT_PREREQ_H_