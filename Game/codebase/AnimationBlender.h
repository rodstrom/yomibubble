#ifndef ANIMATION_BLENDER_H
#define ANIMATION_BLENDER_H

#include "OgreEntity.h"
using namespace Ogre;

class AnimationBlender
 {
 public:
   enum BlendingTransition
   {
      BlendSwitch,         // stop source and start dest
      BlendWhileAnimating,   // cross fade, blend source animation out while blending destination animation in
      BlendThenAnimate      // blend source to first frame of dest, when done, start dest anim
   };
 
 private:
   Entity *mEntity;
   AnimationState *mSource;
   AnimationState *mTarget;
 
   BlendingTransition mTransition;
 
   bool loop;
 
   ~AnimationBlender() {}
 
 public: 
   Real mTimeleft, mDuration;
 
   bool complete;
 
   void blend( const String &animation, BlendingTransition transition, Real duration, bool l=true );
   void addTime( Real );
   Real getProgress() { return mTimeleft/ mDuration; }
   AnimationState *getSource() { return mSource; }
   AnimationState *getTarget() { return mTarget; }
   AnimationBlender( Entity *);
   void init( const String &animation, bool l=true );
 };

#endif //ANIMATION_BLENDER_H