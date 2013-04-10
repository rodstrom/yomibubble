/*
 * =====================================================================================
 *
 *       Filename:  BtOgrePG.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Bullet to
 *                  Ogre (like updating graphics object positions).
 *
 *        Version:  1.0
 *        Created:  27/12/2008 03:40:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _BtOgreGP_H_
#define _BtOgreGP_H_

//#include "Bullet\btBulletDynamicsCommon.h"
//#include "btBulletDynamicsCommon.h"
//#include "OgreSceneNode.h"
#include "BtOgreExtras.h"
#include "Components\ComponentMessenger.h"

namespace BtOgre {

//A MotionState is Bullet's way of informing you about updates to an object.
//Pass this MotionState to a btRigidBody to have your SceneNode updated automaticaly.
class RigidBodyState : public btMotionState
{
    protected:
        btTransform mTransform;
        btTransform mCenterOfMassOffset;
		ComponentMessenger* m_messenger;
        Ogre::SceneNode *mNode;

    public:
        RigidBodyState(ComponentMessenger* messenger, Ogre::SceneNode *node, const btTransform &transform, const btTransform &offset = btTransform::getIdentity())
            : mTransform(transform),
              mCenterOfMassOffset(offset),
              mNode(node),
			  m_messenger(messenger)
        {
        }

        RigidBodyState(ComponentMessenger* messenger, Ogre::SceneNode *node)
            : mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0,0,0,1)),
                         ((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition())    : btVector3(0,0,0))),
              mCenterOfMassOffset(btTransform::getIdentity()),
              mNode(node),
			  m_messenger(messenger)
        {
        }

        virtual void getWorldTransform(btTransform &ret) const
        {
            ret = mTransform;
        }

        virtual void setWorldTransform(const btTransform &in)
        {
            if (mNode == NULL)
                return;

            mTransform = in;
            btTransform transform = in * mCenterOfMassOffset;

            btQuaternion rot = transform.getRotation();
            btVector3 pos = transform.getOrigin();
			Ogre::Vector3 vec3 = BtOgre::Convert::toOgre(pos);
			Ogre::Quaternion q = BtOgre::Convert::toOgre(rot);
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				node->setPosition(vec3);
				node->setOrientation(q);
			}
        }

        void setNode(Ogre::SceneNode *node)
        {
            mNode = node;
        }
};

//Softbody-Ogre connection goes here!

}

#endif
