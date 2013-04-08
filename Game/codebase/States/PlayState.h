#ifndef _PLAY_STATE_H_
#define _PLAY_STATE_H_

#include "State.h"
#include "..\PhysicsEngine.h"

#include "..\Artifex\ArtifexLoader.h"
class ArtifexLoader;

class ComponentMessenger;
class GameObject;
class PhysicsEngine;
class InputManager;
class Entity;
class PlayState : public State
{
public:
	PlayState(void);
	~PlayState(void);

	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	void Enter();
	void Exit();

private:
	Ogre::Entity*				m_penguin;
	Ogre::SceneNode*			m_penguin_node;
	btRigidBody*				m_penguin_body;
	btCollisionShape*			m_penguin_shape;
	BtOgre::RigidBodyState*		m_penguin_state;

	Ogre::Entity*				m_plane;
	btRigidBody*				m_plane_body;
	btBvhTriangleMeshShape*		m_plane_shape;
	btDefaultMotionState*		m_ground_motion_state;

	Ogre::AnimationState*		m_penguin_anim_state;
	PhysicsEngine*				m_physics_engine;
	GameObject*					m_sinbad;
	ComponentMessenger*			m_messenger;

	Ogre::SceneNode* m_cam_node;
	ArtifexLoader* mArtifexLoader;
};

#endif // _PLAY_STATE_H_