#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include "..\AnimPrereq.h"
#include "..\Functor.h"

class ComponentMessenger;
class AnimationManager {
public:
	AnimationManager(ComponentMessenger* messenger);
	~AnimationManager(void);

	void RegisterCallback(Functor<AnimationManager>* callback);
	void AddAnimation(const AnimationDef& anim_def);
	void PlayAnimation(const Ogre::String& anim_id, bool loop = true, bool wait = false);
	void QueueAnimation(const Ogre::String& anim_id, bool loop = true);	// will queue the animation and play it after the current has ended (will not work if the current animation is looping)

private:
	std::vector<AnimationDef> m_animations;
	ComponentMessenger* m_messenger;
};

#endif // _ANIMATION_MANAGER_H_