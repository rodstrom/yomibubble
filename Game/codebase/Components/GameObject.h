#ifndef _N_GAME_OBJECT_H_
#define _N_GAME_OBJECT_H_

#include "ComponentsPrereq.h"

class GameObjectManager;
class GameObject
{
public:
	GameObject(void);
	~GameObject(void);

	void Update(float deltatime);
	Component* GetComponent(int type);
	void AddComponent(Component* component);
	void AddUpdateable(IComponentUpdateable* updateable);
	void Shut();
	bool DoUpdate();		// check if the game object has any components that requires updating with delta time
	GameObjectManager* GetGameObjectManager() { return m_game_object_manager; }
	const Ogre::String& GetId() { return m_id; }

private:
	std::vector<Component*> m_components;
	std::vector<IComponentUpdateable*> m_updateables;
	ComponentMessenger* m_messenger;
	GameObjectManager* m_game_object_manager;
	Ogre::String m_id;
};

#endif // _N_GAME_OBJECT_H_