#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include "..\AnimPrereq.h"

class ComponentMessenger;
class AnimationManager {
public:
	AnimationManager(ComponentMessenger* messenger);
	~AnimationManager(void);

	void RegisterCallback(Functor<AnimationManager>* callback);
	void AddAnimation(const AnimationDef& anim_def);
	void PlayAnimation(const Ogre::String& anim_id, bool loop = true, int blending_transition = 1);

private:
	std::vector<AnimationDef> m_animations;
	ComponentMessenger* m_messenger;
};

#endif // _ANIMATION_MANAGER_H_