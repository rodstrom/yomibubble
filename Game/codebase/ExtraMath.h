#ifndef _OGRE_EXTRA_MATH_H_
#define _OGRE_EXTRA_MATH_H_

Ogre::Vector3 Lerp (Ogre::Vector3& src_location, Ogre::Vector3& dest_location, Ogre::Real time){
	Ogre::Vector3 dest;
	dest.x = src_location.x + (dest_location.x - src_location.x) * time;
	dest.y = src_location.y + (dest_location.y - src_location.y) * time;
	dest.z = src_location.z + (dest_location.z - src_location.z) * time;
	return dest;
}

#endif // _OGRE_EXTRA_MATH_H_