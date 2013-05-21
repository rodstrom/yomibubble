#include "stdafx.h"
#include "GameObject.h"
#include "ComponentMessenger.h"
#include "PhysicsComponents.h"
#include "..\Managers\GameObjectManager.h"
#include "..\PhysicsEngine.h"

int GameObject::m_object_counter = 0;

GameObject::GameObject(int type) : m_type(type){
	m_messenger = new ComponentMessenger;
	m_id = "GameObject" + NumberToString(m_object_counter);
	m_object_counter++;
	Init();
}

GameObject::GameObject(int type, const Ogre::String& id){
	m_messenger = new ComponentMessenger;
	m_type = type;
	Init();
	m_id = id;
	m_object_counter++;
}

GameObject::~GameObject(void){}

void GameObject::Init(){
	m_component_creator[COMPONENT_MESH_RENDER] = &GameObject::CreateMeshRenderComponent;
	m_component_creator[COMPONENT_ANIMATION] = &GameObject::CreateAnimationComponent;
	m_component_creator[COMPONENT_PLAYER_INPUT] = &GameObject::CreatePlayerInputComponent;
	m_component_creator[COMPONENT_BUBBLE_CONTROL] = &GameObject::CreateBubbleControlComponent;
	m_component_creator[COMPONENT_RIGIDBODY] = &GameObject::CreateRigidbodyComponent;
	m_component_creator[COMPONENT_POINT2POINT_CONSTRAINT] = &GameObject::CreatePoint2PointConstraintComponent;
	m_component_creator[COMPONENT_HINGE_CONSTRAINT] = &GameObject::CreateHingeConstraintComponent;
	m_component_creator[COMPONENT_FOLLOW_CAMERA] = &GameObject::CreateFollowCameraComponent;
	m_component_creator[COMPONENT_NODE] = &GameObject::CreateNodeComponent;
	m_component_creator[COMPONENT_CHILD_NODE] = &GameObject::CreateChildNode;
	m_component_creator[COMPONENT_TRIGGER] = &GameObject::CreateTriggerComponent;
	m_component_creator[COMPONENT_RAYCAST] = &GameObject::CreateRaycastComponent;
	m_component_creator[COMPONENT_SYNCED_TRIGGER] = &GameObject::CreateSyncedTriggerComponent;
	m_component_creator[COMPONENT_SHAPE] = &GameObject::CreateShapeComponent;
	m_component_creator[COMPONENT_GENERIC_6DOF_COMPONENT] = &GameObject::CreateGeneric6DofComponent;
}

Component* GameObject::CreateComponent(int type, const Ogre::Vector3& pos, void* data){
	Component* component = (this->*m_component_creator[type])(pos, data);
	return component;
}

void GameObject::Update(float dt){
	if (!m_updateables.empty()){
		for (unsigned int i = 0; i < m_updateables.size(); i++){
			m_updateables[i]->Update(dt);
		}
	}
}

GameObject* GameObject::GetGameObject(const Ogre::String& id) const{
	return m_game_object_manager->GetGameObject(id);
}

Component* GameObject::GetComponent(int type){
	if (!m_components.empty()){
		for (unsigned int i = 0; i < m_components.size(); i++){
			if (m_components[i]->GetComponentType() == type){
				return m_components[i];
			}
		}
	}
	return NULL;
}

void GameObject::AddComponent(Component* component){
	m_components.push_back(component);
	component->SetOwner(this);
	component->SetMessenger(m_messenger);
}

void GameObject::RemoveGameObject(GameObject* game_object){
	m_game_object_manager->RemoveGameObject(game_object);
}

void GameObject::AddComponentToFront(Component* component){
	m_components.push_front(component);
	component->SetOwner(this);
	component->SetMessenger(m_messenger);
}

void GameObject::AddUpdateable(IComponentUpdateable* updateable){
	m_updateables.push_back(updateable);
}


void GameObject::Shut(){
	if (m_updateables.empty()){
		m_updateables.clear();
	}
	if (!m_components.empty()){
		for (unsigned int i = 0; i < m_components.size(); i++){
			m_components[i]->Shut();
			delete m_components[i];
			m_components[i] = NULL;
		}
		m_components.clear();
	}
	if (m_messenger){
		delete m_messenger;
		m_messenger = NULL;
	}
}

bool GameObject::DoUpdate(){
	if (!m_updateables.empty()){
		return true;
	}
	return false;
}

void GameObject::GetComponents(int type, std::vector<Component*>& list){
	for (unsigned int i = 0; i < m_components.size(); i++){
		if (m_components[i]->GetComponentType() == type){
			list.push_back(m_components[i]);
		}
	}
}

void GameObject::RemoveComponent(Component* component){
	for (unsigned int i = 0; i < m_updateables.size(); i++){
		if ((void*)component == (void*)m_updateables[i]){
			m_updateables.erase(m_updateables.begin() + i);
			break;
		}
	}
	for (unsigned int i = 0; i < m_components.size(); i++){
		if (component == m_components[i]){
			component->Shut();
			delete component;
			m_components.erase(m_components.begin() + i);
			break;
		}
	}
}

void GameObject::RemoveComponent(int type, bool all){
	for (unsigned int i = 0; i < m_components.size(); i++) {
		if (m_components[i]->GetComponentType() == type){
			if (m_components[i]->DoUpdate()){
				RemoveUpdateable(m_components[i]);
			}
			m_components[i]->Shut();
			delete m_components[i];
			m_components[i] = NULL;
			m_components.erase(m_components.begin() + i);
			if (!all){
				break;
			}
			else{
				i--;
			}
		}
	}
}

void GameObject::RemoveUpdateable(Component* component){
	for (unsigned int i = 0; i < m_updateables.size(); i++){
		if ((void*)component == (void*)m_updateables[i]){
			m_updateables.erase(m_updateables.begin() + i);
			break;
		}
	}
}

Component* GameObject::CreateMeshRenderComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateAnimationComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreatePlayerInputComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateBubbleControlComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateRigidbodyComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateAIComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateCharacterControllerComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateFollowCameraComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreatePoint2PointConstraintComponent(const Ogre::Vector3& pos, void* data){
	Point2PointConstraintDef& def = *static_cast<Point2PointConstraintDef*>(data);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	AddComponentToFront(cons);
	cons->Init(GetGameObjectManager()->GetPhysicsEngine(), def.body_a, def.body_b, def.pivot_a, def.pivot_b);
	return cons;
}

Component* GameObject::CreateHingeConstraintComponent(const Ogre::Vector3& pos, void* data){
	HingeConstraintDef& def = *static_cast<HingeConstraintDef*>(data);
	HingeConstraintComponent* hcc = new HingeConstraintComponent;
	AddComponentToFront(hcc);
	hcc->Init(GetGameObjectManager()->GetPhysicsEngine(), def.body_a, def.body_b, def.pivot_a, def.pivot_b, def.axis_a, def.axis_b);
	return hcc;
}

Component* GameObject::CreateChildNode(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateNodeComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateTriggerComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateRaycastComponent(const Ogre::Vector3& pos, void* data){
	return NULL;
}

Component* GameObject::CreateSyncedTriggerComponent(const Ogre::Vector3& pos, void* data){
	TriggerDef& def = *static_cast<TriggerDef*>(data);
	SyncedTriggerComponent* stc = new SyncedTriggerComponent;
	AddComponent(stc);
	AddUpdateable(stc);
	stc->Init(pos, GetGameObjectManager()->GetPhysicsEngine(), &def);
	return stc;
}

Component* GameObject::CreateShapeComponent(const Ogre::Vector3& pos, void* data){
	ShapeDef& def = *static_cast<ShapeDef*>(data);
	ShapeComponent* shape_comp = new ShapeComponent;
	AddComponentToFront(shape_comp);
	shape_comp->Init(def);
	return shape_comp;
}

Component* GameObject::CreateGeneric6DofComponent(const Ogre::Vector3& pos, void* data){
	Generic6DofDef& def = *static_cast<Generic6DofDef*>(data);
	Generic6DofConstraintComponent* comp = new Generic6DofConstraintComponent;
	AddComponentToFront(comp);
	comp->Init(m_game_object_manager->GetPhysicsEngine(), def);
	return comp;
}