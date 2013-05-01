#ifndef _N_CAMERA_COMPONENTS_H_
#define _N_CAMERA_COMPONENTS_H_

#include "ComponentsPrereq.h"

class CameraComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	CameraComponent(void) : m_scene_manager(NULL), m_camera(NULL), m_viewport(NULL), m_camera_id(Ogre::StringUtil::BLANK) { m_type = COMPONENT_CAMERA; m_update = true; }
	virtual ~CameraComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate = false, const Ogre::String& camera_id = Ogre::StringUtil::BLANK);
	virtual void Update(float dt);

	const Ogre::String& GetCameraId() const { return m_camera_id; }
	Ogre::Camera* GetCamera() const { return m_camera; }
	void ActivateCamera();
protected:
	Ogre::SceneManager* m_scene_manager;
	Ogre::Camera*		m_camera;
	Ogre::Viewport*		m_viewport;
	Ogre::String		m_camera_id;
};

class FollowCameraComponent : public CameraComponent{
public:
	FollowCameraComponent(void) : m_camera_goal(NULL), m_camera_pivot(NULL), m_camera_node(NULL), m_pivot_pitch(0) { m_type = COMPONENT_FOLLOW_CAMERA; }
	virtual ~FollowCameraComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Viewport* viewport, bool activate = false, const Ogre::String& camera_id = Ogre::StringUtil::BLANK);
	virtual void Update(float dt);

protected:
	void UpdateCameraGoal(Ogre::Real delta_yaw, Ogre::Real delta_pitch, Ogre::Real delta_zoom);
	Ogre::SceneNode*	m_camera_pivot;
	Ogre::SceneNode*	m_camera_goal;
	Ogre::SceneNode*	m_camera_node;
	Ogre::Real			m_pivot_pitch;
};

#endif //_N_CAMERA_COMPONENTS_H_