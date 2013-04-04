#ifndef _N_GAME_OBJECT_H_
#define _N_GAME_OBJECT_H_

#include "ComponentsPrereq.h"

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

private:
	std::vector<Component*> m_components;
	std::vector<IComponentUpdateable*> m_updateables;
	ComponentMessenger* m_messenger;
};

#endif // _N_GAME_OBJECT_H_