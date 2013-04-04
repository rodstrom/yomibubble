#ifndef _N_ANIMATION_COMPONENT_H_
#define _N_ANIMATION_COMPONENT_H_

#include "RenderComponent.h"

class AnimationComponent : public RenderComponent, public IComponentUpdateable{
public:
	AnimationComponent(void) : m_anim_state(NULL){}
	virtual ~AnimationComponent(void){}
	virtual void Update(float deltatime);
	virtual void Notify(int type, void* message);
	virtual void Send(int type, void* message);
	virtual void Init(const std::string& filename, const Ogre::Vector3& position, Ogre::SceneManager* scene_manager);

protected:
	Ogre::AnimationState* m_anim_state;
};

#endif // _N_ANIMATION_COMPONENT_H_