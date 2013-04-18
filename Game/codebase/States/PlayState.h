#ifndef _PLAY_STATE_H_
#define _PLAY_STATE_H_

#include "State.h"
#include "..\PhysicsEngine.h"
#include "..\Artifex\Loader\ArtifexLoader.h"
#include "..\Functor.h"

class GameObjectManager;
class PhysicsEngine;
class PlayState : public State
{
public:
	PlayState(void);
	~PlayState(void);

	//bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	bool Update(float dt);
	void Enter();
	void Exit();

	void test(void* data);

private:
	/*Ogre::Entity*				m_penguin;
	Ogre::SceneNode*			m_penguin_node;
	btRigidBody*				m_penguin_body;
	btCollisionShape*			m_penguin_shape;
	BtOgre::RigidBodyState*		m_penguin_state;*/

	/*Ogre::AnimationState*		m_penguin_anim_state;
	GameObject*					m_sinbad;
	ComponentMessenger*			m_messenger;*/
	Ogre::Entity*				m_plane;
	btRigidBody*				m_plane_body;
	btBvhTriangleMeshShape*		m_plane_shape;
	btDefaultMotionState*		m_ground_motion_state;
	PhysicsEngine*				m_physics_engine;
	GameObjectManager*			m_game_object_manager;

	Ogre::SceneNode* m_cam_node;
	ArtifexLoader* mArtifexLoader;

	Functor<PlayState> m_func;
};

#endif // _PLAY_STATE_H_