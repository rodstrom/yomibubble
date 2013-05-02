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
	m_create_fptr[GAME_OBJECT_PLAYER]      =	&GameObjectManager::CreatePlayer;
	m_create_fptr[GAME_OBJECT_BLUE_BUBBLE] =	&GameObjectManager::CreateBlueBubble;
	m_create_fptr[GAME_OBJECT_PINK_BUBBLE] =	&GameObjectManager::CreatePinkBubble;
	m_create_fptr[GAME_OBJECT_TOTT]        =	&GameObjectManager::CreateTott;
	m_create_fptr[GAME_OBJECT_PLANE]       =	&GameObjectManager::CreatePlane;
	m_create_fptr[GAME_OBJECT_OVERLAY]	   =	&GameObjectManager::Create2DOverlay;
	m_create_fptr[GAME_OBJECT_GUI]		   =	&GameObjectManager::CreateGUI;
	m_create_fptr[GAME_OBJECT_LEAF]		   =	&GameObjectManager::CreateLeaf;
	m_create_fptr[GAME_OBJECT_TRIGGER_TEST]=	&GameObjectManager::CreateTestTrigger;
	m_create_fptr[GAME_OBJECT_TERRAIN]	   =	&GameObjectManager::CreateTerrain;
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
	CharacterControllerDef& def = *static_cast<CharacterControllerDef*>(data);
	//DynamicCharacterControllerDef& def = *static_cast<DynamicCharacterControllerDef*>(data);
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
	/*DynamicCharacterController* dcc = new DynamicCharacterController;
	go->AddComponent(dcc);
	go->AddUpdateable(dcc);*/
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
	RaycastComponent* raycast = new RaycastComponent;
	go->AddComponent(raycast);
	CountableResourceGUI* gui = new CountableResourceGUI;
	go->AddComponent(gui);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);

	node_comp->Init(position, m_scene_manager);
	node_comp->SetId("player_node");
	acomp->Init("Yomi_2Yomi.mesh", m_scene_manager);
	
	TriggerDef tdef;
	tdef.body_type = STATIC_BODY;
	tdef.collider_type = COLLIDER_SPHERE;
	tdef.radius = 0.1f;

	tc->Init(position, m_physics_engine, &tdef);
	tc->SetId("btrig");
	contr->Init(position, m_physics_engine, def);
	//contr->SetTurnSpeed(def.turn_speed);
	//contr->SetVelocity(def.velocity);
	//contr->SetJumpPower(def.jump_power);
	contr->HasFollowCam(true);
	//contr->SetMaxSpeed(def.max_speed);
	//contr->SetDeacceleration(def.deceleration);
	//contr->SetMaxJumpHeight(def.max_jump_height);
	contr->SetId("body");
	//contr->GetRigidbody()->setFriction(def.friction);
	//contr->GetRigidbody()->setRestitution(def.restitution);
	//contr->SetRaycastLength(5.0f);
	contr->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	//dcc->Init(position, m_physics_engine, def);
	//dcc->GetRigidbody()->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	//dcc->GetGhostObject()->getWorldTransform().setOrigin(BtOgre::Convert::toBullet(position));
	pccomp->Init(m_input_manager, m_sound_manager);
	pccomp->SetMaxVelocity(def.max_speed);
	pccomp->SetVelocity(1.0f);
	pccomp->SetDeacceleration(def.deceleration);
	sound2D->Init(m_sound_manager);
	sound3D->Init(m_sound_manager);
	music2D->Init(m_sound_manager);
	music3D->Init(m_sound_manager);
	gui->Init("Examples/Empty", "Examples/Filled", 10);
	fcc->Init(m_scene_manager, m_viewport, true);
	fcc->GetCamera()->setNearClipDistance(0.1f);
	csnc->Init(Ogre::Vector3(0.0f, 0.0f, 1.0f), "CreateBubble", node_comp->GetSceneNode());
	m_sound_manager->GetYomiNode(node_comp->GetSceneNode()->getName());
	raycast->Init(m_physics_engine, contr->GetRigidbody(), "body");
	raycast->SetLength(Ogre::Vector3(0.0f,-1.0f,0.0f));
	raycast->SetAttached(true);

	return go;
}

GameObject* GameObjectManager::CreateBlueBubble(const Ogre::Vector3& position, void* data){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);

	//Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	//go->AddComponent(cons);
	bc->Init(m_physics_engine, 50.0f, 100.0f);
	node_comp->Init(position, m_scene_manager);
	mrc->Init("sphere.mesh", m_scene_manager);
	Ogre::Vector3 scale(def.start_scale);
	node_comp->GetSceneNode()->setScale(scale);
	mrc->GetEntity()->setMaterialName("Examples/BlueBubble");
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, 1.0f, COLLIDER_SPHERE, DYNAMIC_BODY);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setRestitution(def.restitution);
	rc->GetRigidbody()->setFriction(def.friction);
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));
	return go;
}

GameObject* GameObjectManager::CreatePinkBubble(const Ogre::Vector3& position, void* data){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);

	bc->Init(m_physics_engine, 10.0f, 50.0f);
	node_comp->Init(position, m_scene_manager);
	mrc->Init("sphere.mesh", m_scene_manager);
	Ogre::Vector3 scale(def.start_scale);
	node_comp->GetSceneNode()->setScale(scale);
	mrc->GetEntity()->setMaterialName("Examples/PinkBubble");
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, 1.0f, COLLIDER_SPHERE, DYNAMIC_BODY);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setLinearFactor(btVector3(1,0,1));
	rc->GetRigidbody()->setRestitution(def.restitution);
	rc->GetRigidbody()->setFriction(def.friction);
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));
	return go;
}



GameObject* GameObjectManager::CreateTott(const Ogre::Vector3& position, void* data){
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
	m_sound_manager->GetTottNode(node_comp->GetSceneNode()->getName());
	way_point->Init(node_comp->GetSceneNode(), 0.001f);
	way_point->AddWayPoint(Ogre::Vector3(15.0f, -10.0f, 21.0f));
	
	contr->Init(position, m_physics_engine, def);
	contr->SetTurnSpeed(def.turn_speed);
	contr->SetVelocity(def.velocity);
	contr->GetRigidbody()->setRestitution(def.restitution);
	contr->GetRigidbody()->setFriction(def.friction);
	return go;
}

GameObject* GameObjectManager::CreatePlane(const Ogre::Vector3& position, void* data){
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
	
	rc->Init(position, mrc->GetEntity(), m_physics_engine, 0.0f, COLLIDER_TRIANGLE_MESH_SHAPE, STATIC_BODY);
	rc->GetRigidbody()->setRestitution(plane_def.restitution);
	rc->GetRigidbody()->setFriction(plane_def.friction);
	//mrc->GetSceneNode()->setPosition(BtOgre::Convert::toOgre(rc->GetRigidbody()->getWorldTransform().getOrigin()));

	return go;
}
GameObject* GameObjectManager::Create2DOverlay(const Ogre::Vector3& position, void* data) {
	ButtonDef& buttonDef = *static_cast<ButtonDef*>(data);
	GameObject* go = new GameObject;
	//Overlay2DComponent* overlayComp = new Overlay2DComponent;
	//go->AddComponent(overlayComp);
	OverlayCollisionCallbackComponent* overlayCallBack = new OverlayCollisionCallbackComponent;
	go->AddComponent(overlayCallBack);
	go->AddUpdateable(overlayCallBack);
	Overlay2DAnimatedComponent* overlay2DAnim = new Overlay2DAnimatedComponent;
	go->AddComponent(overlay2DAnim);
	OverlayCallbackComponent* overlaycallback = new OverlayCallbackComponent;
	go->AddComponent(overlaycallback);
	//overlayComp->Init(buttonDef.overlay_name, buttonDef.cont_name);
	overlaycallback->Init(buttonDef.func);
	overlayCallBack->Init(m_input_manager, m_viewport);
	overlay2DAnim->Init(buttonDef.overlay_name, buttonDef.mat_hover, buttonDef.mat_exit, buttonDef.cont_name);

	return go;
}

GameObject* GameObjectManager::CreateGUI(const Ogre::Vector3& position, void* data){
	GuiDef& guiDef = *static_cast<GuiDef*>(data);
	GameObject* go = new GameObject;

	CountableResourceGUI* gui = new CountableResourceGUI;
	go->AddComponent(gui);

	gui->Init(guiDef.tex_inact, guiDef.tex_act, guiDef.num_elems);
	
	return go;
};

GameObject* GameObjectManager::CreateLeaf(const Ogre::Vector3& position, void* data){
	ParticleDef& particleDef = *static_cast<ParticleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_LEAF);
	ParticleComponent* particle = new ParticleComponent;
	go->AddComponent(particle);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rb = new RigidbodyComponent;
	go->AddComponent(rb);

	node_comp->Init(position, m_scene_manager);
	mrc->Init("cube.mesh", m_scene_manager);
	Ogre::Vector3 scale(0.002f);
	node_comp->GetSceneNode()->setScale(scale);
	rb->Init(position, mrc->GetEntity(), m_physics_engine, 1, COLLIDER_BOX, STATIC_BODY);
	particle->Init(m_scene_manager, "Smoke", particleDef.particle_name);
	node_comp->GetSceneNode()->setPosition(Ogre::Vector3(position));
	particle->CreateParticle(node_comp->GetSceneNode(), node_comp->GetSceneNode()->getPosition(), Ogre::Vector3(0,-3,0));
	return go;
}

GameObject* GameObjectManager::CreateTestTrigger(const Ogre::Vector3& position, void* data){
	TriggerDef& def = *static_cast<TriggerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_TRIGGER_TEST);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);

	tc->Init(position, m_physics_engine, &def);
	return go;
}

GameObject* GameObjectManager::CreateTerrain(const Ogre::Vector3& position, void* data){
	GameObject* go = new GameObject(GAME_OBJECT_TERRAIN);
	TerrainComponent* tc = new TerrainComponent;
	go->AddComponent(tc);

	tc->Init(m_scene_manager, m_physics_engine, *static_cast<Ogre::String*>(data));
	return go;
}