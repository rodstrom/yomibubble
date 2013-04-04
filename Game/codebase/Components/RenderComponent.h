#ifndef _N_RENDER_COMPONENT_H_
#define _N_RENDER_COMPONENT_H_

#include "ComponentsPrereq.h"

class RenderComponent : public Component, public IComponentObserver {
public:
	RenderComponent(void) : m_node(NULL), m_entity(NULL), m_scene_manager(NULL){}
	virtual ~RenderComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Send(int type, void* message);
	virtual void Init(const std::string& filename, const Ogre::Vector3& position, Ogre::SceneManager* scene_manager);
	Ogre::SceneNode* GetSceneNode() const { return m_node; }
	Ogre::Entity* GetEntity() const { return m_entity; }

protected:
	Ogre::SceneNode*	m_node;
	Ogre::Entity*		m_entity;
	Ogre::SceneManager* m_scene_manager;
};

#endif // _N_RENDER_COMPONENT_H_