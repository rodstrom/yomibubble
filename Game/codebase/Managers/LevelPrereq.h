#ifndef _LEVEL_PREREQ_H_
#define _LEVEL_PREREQ_H_

struct LevelDef{
	Ogre::String next_level;
	Ogre::String filepath;
	Ogre::String loading_screen_name;	// This is the name of the loading screen (Ogre::Overlay) that wil be shown for the next level;
};

#endif // _LEVEL_PREREQ_H_