#ifndef _N_GAME_OBJECT_H_
#define _N_GAME_OBJECT_H_

#include "ComponentsPrereq.h"

class GameObjectManager;
class GameObject
{
public:
	GameObject(int type, const Ogre::String& id = Ogre::StringUtil::BLANK);
	~GameObject(void);

	void Update(float dt);
	Component* GetComponent(int type);
	void GetComponents(int type, std::vector<Component*>& list);	// get a list of all components of selected type, should there be more than one of the same type
	void AddComponent(Component* component);			// Adds the component to the back of the component list.
	void AddComponentToFront(Component* component);		// Adds the component to the front instead of the back. This is useful when having constraints and rigid body components because you need to remove the constraint before removing the rigid body.
	void AddUpdateable(IComponentUpdateable* updateable);
	void Shut();
	bool DoUpdate();		// check if the game object has any components that requires updating with delta time
	GameObjectManager* GetGameObjectManager() const { return m_game_object_manager; }
	ComponentMessenger* GetComponentMessenger() const { return m_messenger; }
	void RemoveComponent(Component* component);
	void RemoveComponent(int type, bool all = false);
	void RemoveGameObject(GameObject* game_object);		// removes a game object using the GameObjectManager (so you don't have to get the game object manager directly)
	int GetType() { return m_type; }	// Returns the type of game object (eg: Player, Tott, Leaf etc)
	const Ogre::String& GetId() const { return m_id; }		// Returns the unique ID of the game object.
	void SetGameObjectManager(GameObjectManager* game_object_manager) { m_game_object_manager = game_object_manager; }
	void SetId(const Ogre::String& id) { m_id = id; }
	void SetType(int type) { m_type = type; }
	Component* CreateComponent(int type, const Ogre::Vector3& pos, void* data);		// Creates and adds a component of specified type
	GameObject* GetGameObject(const Ogre::String& id) const;

private:
	void Init();
	void RemoveUpdateable(Component* component);
	std::deque<Component*> m_components;
	std::deque<IComponentUpdateable*> m_updateables;
	ComponentMessenger* m_messenger;
	GameObjectManager* m_game_object_manager;
	int m_type;
	Ogre::String m_id;
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
	Component* CreateGeneric6DofComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateChildNode(const Ogre::Vector3& pos, void* data);
	Component* CreateNodeComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateTriggerComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateSyncedTriggerComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateRaycastComponent(const Ogre::Vector3& pos, void* data);
	Component* CreateShapeComponent(const Ogre::Vector3& pos, void* data);
};

#endif // _N_GAME_OBJECT_H_