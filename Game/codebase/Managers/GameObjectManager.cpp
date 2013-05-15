#include "stdafx.h"
#include "GameObjectManager.h"
#include "..\Components\GameObject.h"
#include "..\PhysicsEngine.h"
#include "..\Components\VisualComponents.h"
#include "..\Components\PhysicsComponents.h"
#include "..\Components\CameraComponents.h"
#include "..\Components\AIComponents.h"
#include "..\Components\TimerComponent.h"
#include "InputManager.h"
#include "..\Components\AudioComponents.h"
#include "..\Components\PlayerInputComponent.h"
#include "..\Managers\SoundManager.h"
#include "..\PhysicsPrereq.h"
#include "..\MessageSystem.h"

GameObjectManager::GameObjectManager(void) : 
	m_physics_engine(NULL), m_scene_manager(NULL), m_input_manager(NULL), m_viewport(NULL){}
GameObjectManager::~GameObjectManager(void){}

void GameObjectManager::Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager, MessageSystem* message_system){	
	m_physics_engine = physics_engine;
	m_scene_manager = scene_manager;
	m_input_manager = input_manager;
	m_viewport = viewport;
	m_sound_manager = sound_manager;
	m_message_system = message_system;
	m_create_fptr[GAME_OBJECT_PLAYER]      =	&GameObjectManager::CreatePlayer;
	m_create_fptr[GAME_OBJECT_BLUE_BUBBLE] =	&GameObjectManager::CreateBlueBubble;
	m_create_fptr[GAME_OBJECT_PINK_BUBBLE] =	&GameObjectManager::CreatePinkBubble;
	m_create_fptr[GAME_OBJECT_TOTT]        =	&GameObjectManager::CreateTott;
	m_create_fptr[GAME_OBJECT_PLANE]       =	&GameObjectManager::CreatePlane;
	m_create_fptr[GAME_OBJECT_BUTTON]	   =	&GameObjectManager::CreateButton;
	m_create_fptr[GAME_OBJECT_OVERLAY]	   =	&GameObjectManager::Create2DOverlay;
	m_create_fptr[GAME_OBJECT_GUI]		   =	&GameObjectManager::CreateGUI;
	m_create_fptr[GAME_OBJECT_LEAF]		   =	&GameObjectManager::CreateLeaf;
	m_create_fptr[GAME_OBJECT_TRIGGER_TEST]=	&GameObjectManager::CreateTestTrigger;
	m_create_fptr[GAME_OBJECT_COMPANION]   =	&GameObjectManager::CreateCompanion;
	m_create_fptr[GAME_OBJECT_TERRAIN]	   =	&GameObjectManager::CreateTerrain;
	m_create_fptr[GAME_OBJECT_GATE]		   =	&GameObjectManager::CreateGate;
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
	m_game_objects.push_back(gameobject);
}

GameObject* GameObjectManager::CreateGameObject(int type, const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = (this->*m_create_fptr[type])(position, data, id);
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

void GameObjectManager::ClearAllGameObjects(){
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
}

GameObject* GameObjectManager::GetGameObject(const Ogre::String& id){
	GameObject* go = NULL;
	std::list<GameObject*>::iterator it;
	for (it = m_game_objects.begin(); it != m_game_objects.end(); it++){
		go = *it;
		if (go->GetId() == id){
			return go;
		}
	}
	return NULL;
}

GameObject* GameObjectManager::CreatePlayer(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	CharacterControllerDef& def = *static_cast<CharacterControllerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_PLAYER);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	AnimationComponent* acomp = new AnimationComponent;
	acomp->AddAnimationStates(2);
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
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
	CountableResourceGUI* gui = new CountableResourceGUI;
	go->AddComponent(gui);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);
	PlayerRaycastCollisionComponent* prcc = new PlayerRaycastCollisionComponent;
	go->AddComponent(prcc);
	TriggerComponent* camera_tc = new TriggerComponent;
	go->AddComponent(camera_tc);

	node_comp->Init(position, m_scene_manager);
	node_comp->SetId("player_node");

	acomp->Init("Yomi.mesh", m_scene_manager, node_comp->GetId());
	acomp->GetEntity()->setMaterialName("_YomiFBXASC039sFBXASC032staffMaterial__191");

	TriggerDef tdef;
	tdef.body_type = STATIC_BODY;
	tdef.collider_type = COLLIDER_SPHERE;
	tdef.radius = 0.1f;

	tc->Init(position, m_physics_engine, tdef);
	tc->SetId("btrig");
	contr->Init(position, m_physics_engine, def);
	contr->HasFollowCam(true);
	contr->SetId("body");
	contr->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	pccomp->Init(m_input_manager, m_sound_manager);
	sound2D->Init(m_sound_manager);
	sound3D->Init(m_sound_manager);
	music2D->Init(m_sound_manager);
	music3D->Init(m_sound_manager);
	gui->Init("Examples/Empty", "Examples/Filled", 6);
	csnc->Init(Ogre::Vector3(0.0f, def.offset.y, 1.0f), "CreateBubble", node_comp->GetSceneNode());
	m_sound_manager->GetYomiNode(node_comp->GetSceneNode()->getName());
	prcc->Init(m_physics_engine);
	
	TriggerDef trdef;
	trdef.body_type = STATIC_BODY;
	trdef.collider_type = COLLIDER_SPHERE;
	trdef.mass = 0.0f;
	trdef.radius = 1.5f;
	trdef.collision_filter.filter = COL_WORLD_TRIGGER;
	trdef.collision_filter.mask = COL_WORLD_STATIC;
	camera_tc->Init(position, m_physics_engine, trdef);

	fcc->Init(m_scene_manager, m_viewport, true);
	fcc->SetTrigger(camera_tc);
	fcc->SetMovementSpeed(2.5f);
	//DEBUGGING GRAVITY
	//contr->GetRigidbody()->setGravity(btVector3(0,0,0));

	return go;
}

GameObject* GameObjectManager::CreateBlueBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);


	bc->Init(m_physics_engine, 0.00001f, 0.00002f);
	node_comp->Init(position, m_scene_manager);
	mrc->Init("BlueBubble.mesh", m_scene_manager);
	//Ogre::Vector3 scale(def.start_scale);
	Ogre::Vector3 scale(0.1f);
	node_comp->GetSceneNode()->setScale(scale);
	//mrc->GetEntity()->setMaterialName("BlueBubble");
	RigidBodyDef body_def;
	body_def.body_type = DYNAMIC_BODY;
	body_def.collider_type = COLLIDER_SPHERE;
	body_def.friction = def.friction;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_BUBBLE;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_WORLD_STATIC;
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, body_def);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	rc->GetRigidbody()->setDamping(0.5f, 0.2f);
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));

	return go;
}

GameObject* GameObjectManager::CreatePinkBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);

	bc->Init(m_physics_engine, 10.0f, 50.0f);
	node_comp->Init(position, m_scene_manager);
	mrc->Init("PinkBubble.mesh", m_scene_manager);
	Ogre::Vector3 scale(def.start_scale);
	node_comp->GetSceneNode()->setScale(scale);
	//mrc->GetEntity()->setMaterialName("PinkBubble");
	RigidBodyDef body_def;
	body_def.body_type = DYNAMIC_BODY;
	body_def.collider_type = COLLIDER_SPHERE;
	body_def.friction = def.friction;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_BUBBLE;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_WORLD_STATIC;
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, body_def);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	rc->GetRigidbody()->setDamping(0.5f, 0.2f);
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));
	return go;
}

GameObject* GameObjectManager::CreateTott(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	CharacterControllerDef& def = *static_cast<CharacterControllerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_TOTT);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	AnimationComponent* acomp = new AnimationComponent;
	acomp->AddAnimationStates(1);
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	WayPointComponent* way_point = new WayPointComponent;
	go->AddComponent(way_point);
	go->AddUpdateable(way_point);

	node_comp->Init(position, m_scene_manager);
	acomp->Init("Yomi_2Yomi.mesh", m_scene_manager);
	acomp->GetEntity()->setMaterialName("SolidColor/Green");
	m_sound_manager->GetTottNode(node_comp->GetSceneNode()->getName());
	way_point->Init(node_comp->GetSceneNode(), "false");
	way_point->AddWayPoint(Ogre::Vector3(15.0f, -10.0f, 21.0f));
	
	contr->Init(position, m_physics_engine, def);
	contr->SetTurnSpeed(def.turn_speed);
	contr->SetVelocity(def.velocity);
	contr->GetRigidbody()->setRestitution(def.restitution);
	contr->GetRigidbody()->setFriction(def.friction);
	
	//DEBUGGING GRAVITY
	contr->GetRigidbody()->setGravity(btVector3(0,0,0));

	return go;
}

GameObject* GameObjectManager::CreatePlane(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_PLANE);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	
	node_comp->Init(position, m_scene_manager);
	PlaneDef& plane_def = *static_cast<PlaneDef*>(data);
	mrc->Init(plane_def.plane_name, m_scene_manager);
	mrc->GetEntity()->setMaterialName(plane_def.material_name);
	mrc->GetEntity()->setCastShadows(false);
	
	RigidBodyDef def;
	def.body_type = STATIC_BODY;
	def.collider_type = COLLIDER_TRIANGLE_MESH_SHAPE;
	def.friction = plane_def.friction;
	def.restitution = plane_def.restitution;
	def.mass = 0.0f;
	def.collision_filter.filter = COL_WORLD_STATIC;
	def.collision_filter.mask = COL_PLAYER | COL_BUBBLE | COL_TOTT | COL_COMPANION;
	rc->Init(position, mrc->GetEntity(), m_physics_engine, def);
	rc->GetCollisionDef().flag = COLLISION_FLAG_STATIC;
	return go;
}
GameObject* GameObjectManager::CreateButton(const Ogre::Vector3& position, void* data, const Ogre::String& id) {
	ButtonDef& buttonDef = *static_cast<ButtonDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BUTTON);
	OverlayCollisionCallbackComponent* collisionCallBack = new OverlayCollisionCallbackComponent;
	go->AddComponent(collisionCallBack);
	go->AddUpdateable(collisionCallBack);
	Overlay2DAnimatedComponent* overlay2DAnim = new Overlay2DAnimatedComponent;
	go->AddComponent(overlay2DAnim);
	OverlayCallbackComponent* overlaycallback = new OverlayCallbackComponent;
	go->AddComponent(overlaycallback);
	overlaycallback->Init(buttonDef.func);
	collisionCallBack->Init(m_input_manager, m_viewport);
	overlay2DAnim->Init(buttonDef.overlay_name, buttonDef.mat_start_hover, buttonDef.mat_start, buttonDef.cont_name);

	return go;
}

GameObject* GameObjectManager::Create2DOverlay(const Ogre::Vector3& position, void* data, const Ogre::String& id) {
 OverlayDef& overlayDef = *static_cast<OverlayDef*>(data);
 GameObject* go = new GameObject(GAME_OBJECT_OVERLAY);
 Overlay2DComponent* overlayComp = new Overlay2DComponent;
 go->AddComponent(overlayComp);
 OverlayCollisionCallbackComponent* coll_comp = new OverlayCollisionCallbackComponent;
 go->AddComponent(coll_comp);
 go->AddUpdateable(coll_comp);

 coll_comp->Init(m_input_manager, m_viewport);
 overlayComp->Init(overlayDef.overlay_name, overlayDef.cont_name);

 return go;
}

GameObject* GameObjectManager::CreateGUI(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GuiDef& guiDef = *static_cast<GuiDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_GUI);

	CountableResourceGUI* gui = new CountableResourceGUI;
	go->AddComponent(gui);

	gui->Init(guiDef.tex_inact, guiDef.tex_act, guiDef.num_elems);
	
	return go;
};

GameObject* GameObjectManager::CreateLeaf(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	ParticleDef& particleDef = *static_cast<ParticleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_LEAF);
	//ParticleComponent* particle = new ParticleComponent;
	//go->AddComponent(particle);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	BobbingComponent* bc = new BobbingComponent;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	TriggerComponent* stc = new TriggerComponent;
	go->AddComponent(stc);

	node_comp->Init(position, m_scene_manager);
	mrc->Init("Collectable_Leaf.mesh", m_scene_manager);
	bc->Init(node_comp->GetSceneNode());
	//Ogre::Vector3 scale(0.002f);
	//node_comp->GetSceneNode()->setScale(scale);

	mrc->GetEntity()->setMaterialName("Examples/Leaf");
	node_comp->GetSceneNode()->setPosition(Ogre::Vector3(position));
	//particle->Init(m_scene_manager, "bajs", particleDef.particle_name);
	//particle->CreateParticle(node_comp->GetSceneNode(), node_comp->GetSceneNode()->getPosition(), Ogre::Vector3(0,-3,0));

	TriggerDef trdef;
	trdef.body_type = STATIC_BODY;
	trdef.collider_type = COLLIDER_SPHERE;
	trdef.mass = 0.0f;
	trdef.radius = 1.5f;
	trdef.collision_filter.filter = COL_WORLD_TRIGGER;
	trdef.collision_filter.mask = COL_PLAYER;
	stc->Init(position, m_physics_engine, trdef);

	return go;
}

GameObject* GameObjectManager::CreateTestTrigger(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	TriggerDef& def = *static_cast<TriggerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_TRIGGER_TEST);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);

	tc->Init(position, m_physics_engine, def);
	return go;
}

GameObject* GameObjectManager::CreateCompanion(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	CharacterControllerDef& def = *static_cast<CharacterControllerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_COMPANION);
	TimerComponent* timer = new TimerComponent;
	go->AddComponent(timer);
	go->AddUpdateable(timer);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	AnimationComponent* acomp = new AnimationComponent;
	acomp->AddAnimationStates(1);
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	SyncedTriggerComponent* stc = new SyncedTriggerComponent;
	go->AddComponent(stc);
	WayPointComponent* way_point = new WayPointComponent;
	go->AddComponent(way_point);
	go->AddUpdateable(way_point);

	TriggerDef tdef;
	tdef.body_type = STATIC_BODY;
	tdef.collider_type = COLLIDER_SPHERE;
	tdef.radius = 0.1f;

	node_comp->Init(position, m_scene_manager);
	node_comp->SetId("companion");
	
	acomp->Init("Yomi.mesh", m_scene_manager, node_comp->GetId());
	acomp->GetEntity()->setMaterialName("_YomiFBXASC039sFBXASC032staffMaterial__191");
	stc->Init(position, m_physics_engine, &tdef);
	contr->Init(position, m_physics_engine, def);
	//rb->Init(position, acomp->GetEntity(), m_physics_engine, rdef);
	way_point->Init(node_comp->GetSceneNode(), "true");
	//way_point->AddWayPoint(static_cast<NodeComponent*>(GetGameObject("GameObject1")->GetComponent(EComponentType::COMPONENT_NODE))->GetSceneNode());
	node_comp->GetSceneNode()->setPosition(Ogre::Vector3(position));
	return go;
}

GameObject* GameObjectManager::CreateTerrain(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_TERRAIN);
	TerrainComponent* tc = new TerrainComponent;
	go->AddComponent(tc);

	tc->Init(m_scene_manager, m_physics_engine, this, m_sound_manager, *static_cast<Ogre::String*>(data));
	return go;
}

GameObject* GameObjectManager::CreateGate(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_GATE);
	NodeComponent* nc = new NodeComponent;
	go->AddComponent(nc);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	//RigidbodyComponent* rc = new RigidbodyComponent;
	//go->AddComponent(rc);

	nc->Init(position, m_scene_manager);
	mrc->Init("Gate.mesh", m_scene_manager);
	Ogre::Vector3 scale(0.02f);
	nc->GetSceneNode()->setScale(scale);
	mrc->GetEntity()->setMaterialName("Examples/Athene/NormalMapped");
	return go;
}

GameObject* GameObjectManager::CreateLoadingScreen(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_LOADING_SCREEN);
	//TODO Create 2D image of loading
	return go;
}

GameObject* GameObjectManager::CreateNextLevelTrigger(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	TriggerDef& def = *static_cast<TriggerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_NEXT_LEVEL);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);
	tc->Init(position, m_physics_engine, def);
	return go;
}