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
	void PlayAnimation(const Ogre::String& anim_id);

private:
	std::vector<AnimationDef> m_animations;
	//std::vector<AnimationDef> m_current_animation;
	ComponentMessenger* m_messenger;
};

#endif // _ANIMATION_MANAGER_H_