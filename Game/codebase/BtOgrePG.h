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
		bool m_update_orientation;
		bool m_update_position;

		bool m_hack_pivot;

    public:
        RigidBodyState(ComponentMessenger* messenger, const btTransform &transform, const btTransform &offset = btTransform::getIdentity())
            : mTransform(transform),
              mCenterOfMassOffset(offset),
			  m_messenger(messenger),
			  m_update_orientation(true),
			  m_hack_pivot(false),
			  m_update_position(true){}

        RigidBodyState(ComponentMessenger* messenger) :
              mCenterOfMassOffset(btTransform::getIdentity()),
			  m_messenger(messenger),
			  m_update_orientation(true),
			  m_update_position(true){}

        virtual void getWorldTransform(btTransform &ret) const{
            ret = mTransform;
        }

		void UpdatePosition(bool value) { m_update_position = value; }
		void UpdateOrientation(bool value) { m_update_orientation = value; }

		void setWorldTransformMod(const btTransform &in, Ogre::Vector3 posMod){
			if (m_messenger == NULL)
			{ return; }

			m_hack_pivot = true;
			setWorldTransform(in);
           
		}

        virtual void setWorldTransform(const btTransform &in){
            if (m_messenger == NULL)
                return;

            mTransform = in;
            btTransform transform = in * mCenterOfMassOffset;

            btQuaternion rot = transform.getRotation();
            btVector3 pos = transform.getOrigin();
			Ogre::Vector3 vec3 = BtOgre::Convert::toOgre(pos);
			if (m_hack_pivot)
			{ 
				//vec3.y -= 0.8; 
			}
			m_hack_pivot = false;
			Ogre::Quaternion quat = BtOgre::Convert::toOgre(rot);
			Ogre::SceneNode* node = NULL;
			m_messenger->Notify(MSG_NODE_GET_NODE, &node);
			if (node){
				if (m_update_position) node->setPosition(vec3);
				if (m_update_orientation) node->setOrientation(quat);
			}
			
        }
};

//Softbody-Ogre connection goes here!

}

#endif
