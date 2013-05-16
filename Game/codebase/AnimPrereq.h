#ifndef _ANIM_PREREQ_H_
#define _ANIM_PREREQ_H_

struct AnimationDef{
	AnimationDef(void) : id(Ogre::StringUtil::BLANK), index(0), full_body(false){}
	AnimationDef(const Ogre::String& p_id, int p_index, bool p_full_body) : id(p_id), index(p_index), full_body(p_full_body){}
	Ogre::String id;
	int index;
	bool full_body;	// full body will disable other animations (if any) when played
};

#endif // _ANIM_PREREQ_H_