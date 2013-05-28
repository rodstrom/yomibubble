#ifndef _ANIM_PREREQ_H_
#define _ANIM_PREREQ_H_

struct AnimationDef{
	AnimationDef(void) : id(Ogre::StringUtil::BLANK), index(0), duration(0.0f), id_2(Ogre::StringUtil::BLANK){}
	AnimationDef(const Ogre::String& p_id, int p_index, float p_duration, const Ogre::String& p_id_2 = Ogre::StringUtil::BLANK) : id(p_id), index(p_index), duration(p_duration), id_2(p_id_2){}
	Ogre::String id;
	Ogre::String id_2;	// used for full body animations when the object has "top" and "bottom" animations
	int index;
	float duration;
};

#endif // _ANIM_PREREQ_H_