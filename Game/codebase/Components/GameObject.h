#ifndef _N_GAME_OBJECT_H_
#define _N_GAME_OBJECT_H_

#include "ComponentsPrereq.h"

class GameObjectManager;
class GameObject
{
public:
	GameObject(void);
	GameObject(const Ogre::String& id);
	~GameObject(void);

	void Update(float dt);
	void LateUpdate(float dt);		// Late Update is used to sync Kinematic character controllers to the renderer
	Component* GetComponent(int type);
	void GetComponents(int type, std::vector<Component*>& list);	// get a list of all components, should there be more than one of the same type
	void AddComponent(Component* component);
	void AddUpdateable(IComponentUpdateable* updateable);
	void AddLateUpdate(IComponentLateUpdate* late_update);
	void Shut();
	bool DoUpdate();		// check if the game object has any components that requires updating with delta time
	bool DoLateUpdate();
	GameObjectManager* GetGameObjectManager() const { return m_game_object_manager; }
	ComponentMessenger* GetComponentMessenger() const { return m_messenger; }
	const Ogre::String& GetId() { return m_id; }

private:
	std::vector<Component*> m_components;
	std::vector<IComponentUpdateable*> m_updateables;
	std::vector<IComponentLateUpdate*> m_late_updates;
	ComponentMessenger* m_messenger;
	GameObjectManager* m_game_object_manager;
	Ogre::String m_id;
	static int m_object_counter;
};

#endif // _N_GAME_OBJECT_H_