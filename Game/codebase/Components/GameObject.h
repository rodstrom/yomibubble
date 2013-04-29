#ifndef _N_GAME_OBJECT_H_
#define _N_GAME_OBJECT_H_

#include "ComponentsPrereq.h"

class GameObjectManager;
class GameObject
{
public:
	GameObject(void);
	GameObject(int id);
	~GameObject(void);

	void Update(float dt);
	void LateUpdate(float dt);		// Late Update is used to sync Kinematic character controllers to the renderer
	Component* GetComponent(int type);
	void GetComponents(int type, std::vector<Component*>& list);	// get a list of all components of selected type, should there be more than one of the same type
	void AddComponent(Component* component);
	void AddUpdateable(IComponentUpdateable* updateable);
	void AddLateUpdate(IComponentLateUpdate* late_update);
	void Shut();
	bool DoUpdate();		// check if the game object has any components that requires updating with delta time
	bool DoLateUpdate();
	GameObjectManager* GetGameObjectManager() const { return m_game_object_manager; }
	ComponentMessenger* GetComponentMessenger() const { return m_messenger; }
	void RemoveComponent(Component* component);
	void RemoveComponent(int type, bool all = false);
	int GetType() { return m_type; }
	void SetGameObjectManager(GameObjectManager* game_object_manager) { m_game_object_manager = game_object_manager; }
	Component* CreateComponent(int type, const Ogre::Vector3& pos, void* data);

private:
	void Init();
	void RemoveUpdateable(Component* component);
	std::vector<Component*> m_components;
	std::vector<IComponentUpdateable*> m_updateables;
	std::vector<IComponentLateUpdate*> m_late_updates;
	ComponentMessenger* m_messenger;
	GameObjectManager* m_game_object_manager;
	int m_type;
	static int m_object_counter;

	typedef Component* (GameObject::*ComponentCreate)(const Ogre::Vector3&,void*);
	ComponentCreate m_component_creator[COMPONENT_SIZE];

	//Component Create Methods
	Component* CreateMeshRenderComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateAnimationComponent(const Ogre::Vector3& pos, void* data);
	Component* CreatePlayerInputComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateBubbleControlComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateRigidbodyComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateAIComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateCharacterControllerComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateFollowCameraComponent(const Ogre::Vector3& pos, void* data);
	Component* CreatePoint2PointConstraintComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateHingeConstraintComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateChildNode(const Ogre::Vector3& pos, void* data);
	Component* CreateNodeComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateTriggerComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateRaycastComponent(const Ogre::Vector3& pos, void* data);
};

#endif // _N_GAME_OBJECT_H_