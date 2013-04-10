#ifndef _N_GAME_OBJECT_FACTORY_H_
#define _N_GAME_OBJECT_FACTORY_H_

class GameObject;
class IGameObjectFactory
{
public:
	virtual GameObject* CreatePlayer(const Ogre::Vector3& position, void* data)		= 0;
	virtual GameObject* CreateBlueBubble(const Ogre::Vector3& position, void* data) = 0;
	virtual GameObject* CreatePinkBubble(const Ogre::Vector3& position, void* data) = 0;
};

#endif // _N_GAME_OBJECT_FACTORY_H_