#include "stdafx.h"
#include "GameObjectManager.h"
#include "..\Components\GameObject.h"
#include "..\PhysicsEngine.h"
#include "..\Components\VisualComponents.h"
#include "..\Components\PhysicsComponents.h"
#include "..\Components\CameraComponents.h"
#include "..\Components\AIComponents.h"
#include "InputManager.h"
#include "..\Components\AudioComponents.h"
#include "..\Components\PlayerInputComponent.h"
#include "..\Managers\SoundManager.h"

GameObjectManager::GameObjectManager(void) : 
	m_physics_engine(NULL), m_scene_manager(NULL), m_input_manager(NULL), m_viewport(NULL){}
GameObjectManager::~GameObjectManager(void){}

void GameObjectManager::Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager){
	m_physics_engine = physics_engine;
	m_scene_manager = scene_manager;
	m_input_manager = input_manager;
	m_viewport = viewport;
	m_sound_manager = sound_manager;
	m_create_fptr[GAME_OBJECT_PLAYER] =			&GameObjectManager::CreatePlayer;
	m_create_fptr[GAME_OBJECT_BLUE_BUBBLE] =	&GameObjectManager::CreateBlueBubble;
	m_create_fptr[GAME_OBJECT_PINK_BUBBLE] =	&GameObjectManager::CreatePinkBubble;
	m_create_fptr[GAME_OBJECT_TOTT] =			&GameObjectManager::CreateTott;
	m_create_fptr[GAME_OBJECT_PLANE] =			&GameObjectManager::CreatePlane;
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
	go->SetGameObjectManager(this);
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
	CharControllerDef& def = *static_cast<CharControllerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_PLAYER);
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
	ChildSceneNodeComponent* csnc = new ChildSceneNodeComponent;
	go->AddComponent(csnc);
	Sound2DComponent* sound2D = new Sound2DComponent;
	go->AddComponent(sound2D);
	Sound3DComponent* sound3D = new Sound3DComponent;
	go->AddComponent(sound3D);
	Music2DComponent* music2D = new Music2DComponent;
	go->AddComponent(music2D);
	Music3DComponent* music3D = new Music3DComponent;
	go->AddComponent(music3D);

	acomp->Init("Yomi_2Yomi.mesh", m_scene_manager);
	//acomp->Init("yomi.mesh", m_scene_manager);
	contr->Init(position, acomp->GetEntity(), def.step_height, def.collider_type, m_physics_engine);
	contr->SetTurnSpeed(def.turn_speed);
	contr->SetVelocity(def.velocity);
	//GravityDebug!
	contr->GetController()->setGravity(0.0f);
	//contr->SetJumpPwr(1.0f);
	//contr->SetJumpPwr(10.0f);
	contr->HasFollowCam(true);
	pccomp->Init(m_input_manager, m_sound_manager);
	sound2D->Init(m_sound_manager);
	sound3D->Init(m_sound_manager);
	music2D->Init(m_sound_manager);
	music3D->Init(m_sound_manager);
	fcc->Init(m_scene_manager, m_viewport, true);
	fcc->GetCamera()->setNearClipDistance(0.1f);
	//fcc->GetCamera()->setFarClipDistance(1000);
	csnc->Init(Ogre::Vector3(0.0f, 0.0f, 1.0f), "CreateBubble", acomp->GetSceneNode());
	m_sound_manager->GetYomiNode(acomp->GetSceneNode()->getName());

	return go;
}

GameObject* GameObjectManager::CreateBlueBubble(const Ogre::Vector3& position, void* data){
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);

	mrc->Init("sphere.mesh", m_scene_manager);
	Ogre::Vector3 scale(0.002,0.002,0.002);
	mrc->GetSceneNode()->setScale(scale);
	//mrc->GetEntity()->setMaterialName();
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, 1.0f, COLLIDER_SPHERE, DYNAMIC_BODY);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setRestitution(1.0f);
	rc->GetRigidbody()->setFriction(0.5);
	return go;
}

GameObject* GameObjectManager::CreatePinkBubble(const Ogre::Vector3& position, void* data){
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);

	mrc->Init("sphere.mesh", m_scene_manager);
	Ogre::Vector3 scale(0.002,0.002,0.002);
	mrc->GetSceneNode()->setScale(scale);
	//mrc->GetEntity()->setMaterialName();
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, 1.0f, COLLIDER_SPHERE, DYNAMIC_BODY);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setLinearFactor(btVector3(1,0,1));
	return go;
}

GameObject* GameObjectManager::CreateTott(const Ogre::Vector3& position, void* data){
	CharControllerDef& def = *static_cast<CharControllerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_TOTT);
	AnimationComponent* acomp = new AnimationComponent;
	acomp->AddAnimationStates(1);
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	go->AddLateUpdate(contr);
	WayPointComponent* way_point = new WayPointComponent;
	go->AddComponent(way_point);
	go->AddUpdateable(way_point);
	acomp->Init("Yomi_2Yomi.mesh", m_scene_manager);

	m_sound_manager->GetTottNode(acomp->GetSceneNode()->getName());

	way_point->Init(acomp->GetSceneNode(), 0.001);
	
	contr->Init(position, acomp->GetEntity(), def.step_height, def.collider_type, m_physics_engine);
	contr->SetTurnSpeed(def.turn_speed);
	contr->SetVelocity(def.velocity);
	//GravityDebug!
	contr->GetController()->setGravity(0.0f);
	return go;
}

GameObject* GameObjectManager::CreatePlane(const Ogre::Vector3& position, void* data){
	GameObject* go = new GameObject(GAME_OBJECT_PLANE);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	
	PlaneDef& plane_def = *static_cast<PlaneDef*>(data);
	mrc->Init(plane_def.plane_name, m_scene_manager);
	mrc->GetEntity()->setMaterialName(plane_def.material_name);
	rc->Init(position, mrc->GetEntity(), m_physics_engine, 0.0f, COLLIDER_TRIANGLE_MESH_SHAPE, STATIC_BODY);
	rc->GetRigidbody()->setRestitution(0.5);
	rc->GetRigidbody()->setFriction(0.5f);
	return go;
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