#include "..\stdafx.h"
#include "GameObjectManager.h"
#include "..\Components\GameObject.h"
#include "..\PhysicsEngine.h"
#include "..\Components\Components.h"
#include "..\Components\PlayerInputComponent.h"
#include "InputManager.h"

GameObjectManager::GameObjectManager(void) : 
	m_physics_engine(NULL), m_scene_manager(NULL), m_input_manager(NULL), m_viewport(NULL){}
GameObjectManager::~GameObjectManager(void){}

void GameObjectManager::Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport){
	m_physics_engine = physics_engine;
	m_scene_manager = scene_manager;
	m_input_manager = input_manager;
	m_viewport = viewport;
	m_create_fptr[GAME_OBJECT_PLAYER] =			&GameObjectManager::CreatePlayer;
	m_create_fptr[GAME_OBJECT_BLUE_BUBBLE] =	&GameObjectManager::CreateBlueBubble;
	m_create_fptr[GAME_OBJECT_PINK_BUBBLE] =	&GameObjectManager::CreatePinkBubble;
	m_create_fptr[GAME_OBJECT_OVERLAY]	   =	&GameObjectManager::Create2DOverlay;
}

void GameObjectManager::Update(float dt){
	if (!m_updateable_game_objects.empty()){
		std::list<GameObject*>::iterator it;
		GameObject* go = NULL;
		for (it = m_updateable_game_objects.begin(); it != m_updateable_game_objects.end(); it++){
			go = *it;
			go->Update(dt);
		}
	}
}

void GameObjectManager::LateUpdate(float dt){
	if (!m_late_update_objects.empty()){
		std::list<GameObject*>::iterator it;
		GameObject* go = NULL;
		for (it = m_late_update_objects.begin(); it != m_late_update_objects.end(); it++){
			go = *it;
			go->LateUpdate(dt);
		}
	}
}

void GameObjectManager::RemoveGameObject(GameObject* gameobject){
	if (gameobject->DoUpdate()){
		m_updateable_game_objects.remove(gameobject);
	}
	gameobject->Shut();
	m_game_objects.remove(gameobject);
	delete gameobject;
	gameobject = NULL;
}

void GameObjectManager::AddGameObject(GameObject* gameobject){
	if (gameobject->DoUpdate()){
		m_updateable_game_objects.push_back(gameobject);
	}
	if (gameobject->DoLateUpdate()){
		m_late_update_objects.push_back(gameobject);
	}
	m_game_objects.push_back(gameobject);
}

GameObject* GameObjectManager::CreateGameObject(int type, const Ogre::Vector3& position, void* data){
	GameObject* go = (this->*m_create_fptr[type])(position, data);
	AddGameObject(go);
	return go;
}

void GameObjectManager::Shut(){
	if (!m_updateable_game_objects.empty()){
		m_updateable_game_objects.clear();
	}
	if (!m_game_objects.empty()){
		std::list<GameObject*>::iterator it;
		GameObject* go = NULL;
		for (it = m_game_objects.begin(); it != m_game_objects.end(); it++){
			go = *it;
			go->Shut();
			delete go;
			go = NULL;
		}
		m_game_objects.clear();
	}
	for (int i = 0; i < GAME_OBJECT_SIZE; i++){
		m_create_fptr[i] = NULL;
	}
	m_scene_manager = NULL;
	m_physics_engine = NULL;
}

GameObject* GameObjectManager::CreatePlayer(const Ogre::Vector3& position, void* data){
	GameObject* go = new GameObject;
	AnimationComponent* acomp = new AnimationComponent;
	acomp->AddAnimationStates(2);
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	go->AddLateUpdate(contr);
	FollowCameraComponent* fcc = new FollowCameraComponent;
	go->AddComponent(fcc);
	go->AddUpdateable(fcc);
	PlayerInputComponent* pccomp = new PlayerInputComponent;
	go->AddComponent(pccomp);
	go->AddUpdateable(pccomp);


	acomp->Init("sinbad.mesh", m_scene_manager);
	contr->Init(m_physics_engine);
	contr->SetTurnSpeed(500.0f);
	contr->SetVelocity(10.0f);
	pccomp->Init(m_input_manager);
	fcc->Init(m_scene_manager, m_viewport, true);
	fcc->GetCamera()->setNearClipDistance(0.1);
	fcc->GetCamera()->setFarClipDistance(100);
	return go;
}

GameObject* GameObjectManager::CreateBlueBubble(const Ogre::Vector3& position, void* data){
	return NULL;
}

GameObject* GameObjectManager::CreatePinkBubble(const Ogre::Vector3& position, void* data){
	return NULL;
}

GameObject* GameObjectManager::Create2DOverlay(const Ogre::Vector3& position, void* data) {
	GameObject* go = new GameObject;
	Overlay2DComponent* overlayComp = new Overlay2DComponent;
	go->AddComponent(overlayComp);
	OverlayCollisionCallback* overlayCallBack = new OverlayCollisionCallback;
	go->AddComponent(overlayCallBack);
	go->AddUpdateable(overlayCallBack);

	overlayComp->Init(*static_cast<Ogre::String*>(data));
	overlayCallBack->Init(m_input_manager, m_viewport);

	return go;
}
//mouseposition, storleken på overlayen