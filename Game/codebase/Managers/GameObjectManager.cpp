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
#include "..\Managers\VariableManager.h"
#include "..\Managers\AnimationManager.h"
#include <sstream>

GameObjectManager::GameObjectManager(void) : 
	m_physics_engine(NULL), m_scene_manager(NULL), m_input_manager(NULL), m_viewport(NULL){
		m_max_bubbles = (unsigned int)VariableManager::GetSingletonPtr()->GetAsInt("MaxBubbles");
}
GameObjectManager::~GameObjectManager(void){}

void GameObjectManager::Init(PhysicsEngine* physics_engine, Ogre::SceneManager* scene_manager, InputManager* input_manager, Ogre::Viewport* viewport, SoundManager* sound_manager, MessageSystem* message_system, VariableManager* variable_manager){	
	m_physics_engine = physics_engine;
	m_scene_manager = scene_manager;
	m_input_manager = input_manager;
	m_viewport = viewport;
	m_sound_manager = sound_manager;
	m_variable_manager = variable_manager;
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
	//m_create_fptr[GAME_OBJECT_CAMERA]	   =	&GameObjectManager::CreatePlayerCamera;
	m_create_fptr[GAME_OBJECT_PARTICLE]	   =    &GameObjectManager::CreateParticleEffect;
	m_create_fptr[GAME_OBJECT_QUEST_ITEM]  =    &GameObjectManager::CreateQuestItem;
	m_create_fptr[GAME_OBJECT_SPEECH_BUBBLE] =  &GameObjectManager::CreateSpeechBubble;
	m_create_fptr[GAME_OBJECT_ROCK_SLIDE] =		&GameObjectManager::CreateRockSlide;
	m_create_fptr[GAME_OBJECT_LEVEL_CHANGE] =   &GameObjectManager::CreateLevelChange;
	m_create_fptr[GAME_OBJECT_QUEST_TOTT] =		&GameObjectManager::CreateQuestTott;

	m_leaf_iterations = 0;
	m_particle_iterations = 0;
	m_tott_iterations = 0;
	m_speech_bubble_iterations = 0;
	m_quest_item_iterations = 0;
	m_temp_node = NULL;
}

void GameObjectManager::Update(float dt){
	if (!m_remove_list.empty()){
		this->ClearQueuedGameObjects();
	}
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
	std::list<GameObject*>::iterator it = std::find(m_remove_list.begin(), m_remove_list.end(), gameobject);
	if (it == m_remove_list.end()){
		m_remove_list.push_back(gameobject);
	}
}

void GameObjectManager::ClearQueuedGameObjects(){
	std::list<GameObject*>::iterator it;
	GameObject* go = NULL;
	for (it = m_remove_list.begin(); it != m_remove_list.end(); it++){
		go = *it;
		if (go->GetType() == GAME_OBJECT_BLUE_BUBBLE || go->GetType() == GAME_OBJECT_PINK_BUBBLE){
			RemoveBubble(go);
		}
		if (go->DoUpdate()){
			m_updateable_game_objects.remove(go);
		}
		go->Shut();
		m_game_objects.remove(go);
		delete go;
		go = NULL;
	}
	m_remove_list.clear();
}

void GameObjectManager::CheckBubbleSize(){
	if (m_bubbles.size() >= m_max_bubbles){
		RemoveGameObject(m_bubbles.front());
		m_bubbles.pop_front();
	}
}

void GameObjectManager::RemoveBubble(GameObject* bubble){
	std::deque<GameObject*>::iterator it = std::find(m_bubbles.begin(), m_bubbles.end(), bubble);
	if (it != m_bubbles.end()){
		m_bubbles.erase(it);
	}
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
	if (!m_bubbles.empty()){
		m_bubbles.clear();
	}
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
	if (!m_bubbles.empty()){
		m_bubbles.clear();
	}
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
	PlayerDef& def = *static_cast<PlayerDef*>(data);
	CharacterControllerDef& char_def = *def.character_contr;
	GameObject* go = new GameObject(GAME_OBJECT_PLAYER, "player");
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	AnimationComponent* acomp = new AnimationComponent;
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	MeshRenderComponent* staff_mesh_comp = new MeshRenderComponent;
	go->AddComponent(staff_mesh_comp);
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
	go->AddUpdateable(gui);
	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);
	PlayerRaycastCollisionComponent* prcc = new PlayerRaycastCollisionComponent;
	go->AddComponent(prcc);
	TutorialGraphicsComponent* tutorial = new TutorialGraphicsComponent;
	go->AddComponent(tutorial);
	go->AddUpdateable(tutorial);
	/*
	TriggerComponent* camera_tc = new TriggerComponent;
	go->AddComponent(camera_tc);
	RaycastComponent* camera_rc = new RaycastComponent;
	go->AddComponent(camera_rc);
	CameraRaycastCollisionComponent* camera_rcc = new CameraRaycastCollisionComponent;
	go->AddComponent(camera_rcc);
	//RigidbodyComponent* camera_rb = new RigidbodyComponent;
	*/
	if (def.level_id == "try"){
		tutorial->Init("Level1", def.level_id);
	}
	else{
		tutorial->Init("Level2", def.level_id);
	}

	go->SetId("Player");

	node_comp->Init(position, m_scene_manager);
	node_comp->SetId("player_node");
	
	if(char_def.mesh == "" || char_def.mesh == "Yomi_2Yomi.mesh") char_def.mesh = "Yomi.mesh";
	acomp->Init(char_def.mesh, m_scene_manager, node_comp->GetId(), true);
	acomp->AddAnimationState("Base_Idle");
	acomp->AddAnimationState("Top_Idle");
	//acomp->GetEntity()->setMaterialName("_YomiFBXASC039sFBXASC032staffMaterial__191");

	TriggerDef tdef;
	tdef.body_type = STATIC_BODY;
	tdef.collider_type = COLLIDER_SPHERE;
	tdef.radius = 0.1f;
	tdef.collision_filter.filter = COL_NOTHING;
	tdef.collision_filter.mask = COL_NOTHING;

	tc->Init(position, m_physics_engine, tdef);
	tc->SetId("btrig");
	contr->Init(position, m_physics_engine, char_def);
	contr->HasFollowCam(true);
	contr->SetId("body");
	contr->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	sound2D->Init(m_sound_manager);
	sound3D->Init(m_sound_manager);
	music2D->Init(m_sound_manager);
	music3D->Init(m_sound_manager);
	gui->Init(def.level_id);
	csnc->Init(Ogre::Vector3(0.0f, char_def.offset.y, 1.0f), "CreateBubble", node_comp->GetSceneNode());
	m_sound_manager->GetYomiNode(node_comp->GetSceneNode()->getName());
	prcc->Init(m_physics_engine);
	prcc->SetCustomVariables(VariableManager::GetSingletonPtr()->GetAsFloat("Bounce_Jump_Mod"));

	fcc->SetNode(node_comp->GetSceneNode());
	fcc->Init(position, m_scene_manager, m_viewport, true);
	fcc->SetMovementSpeed(2.5f);

	fcc->SetPhysEngine(m_physics_engine);
	fcc->SetCustomVariables(
		//VariableManager::GetSingletonPtr()->GetAsInt("Camera_Inverted_Controller_Sticks"), 
		VariableManager::GetSingletonPtr()->GetAsFloat("Camera_Zoom_Speed"), 
		VariableManager::GetSingletonPtr()->GetAsFloat("Camera_Stick_Rotation_Acceleration"), 
		VariableManager::GetSingletonPtr()->GetAsFloat("Camera_Change_Angle_After_Player"),
		VariableManager::GetSingletonPtr()->GetAsFloat("Camera_Start_Distance"),
		VariableManager::GetSingletonPtr()->GetAsFloat("Camera_Start_Pitch")
		);
	staff_mesh_comp->Init("Staff.mesh", m_scene_manager, Ogre::StringUtil::BLANK);
	acomp->GetEntity()->attachObjectToBone("CATRigLArmDigit21", staff_mesh_comp->GetEntity(), Ogre::Quaternion(1.0f, 1.0f, 0.7f, 0.0f), Ogre::Vector3(0.01f, -0.02f, 0.0f));
	pccomp->Init(m_input_manager, m_sound_manager, m_physics_engine, m_message_system);
	pccomp->SetCustomVariables(
		VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Min_Size"),
		VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Max_Size"), 
		VariableManager::GetSingletonPtr()->GetAsFloat("On_Bubble_Speed_Mod"), 
		VariableManager::GetSingletonPtr()->GetAsFloat("In_Bubble_Speed_Mod"),
		def.level_id
		);
	return go;
}

//GameObject* GameObjectManager::CreatePlayerCamera(const Ogre::Vector3& position, void* data, const Ogre::String& id){
//	GameObject* go = new GameObject(GAME_OBJECT_CAMERA, "CameraTrig");
//
//	CameraCollisionComponent* ccc = new CameraCollisionComponent;
//	go->AddComponent(ccc);
//	go->AddUpdateable(ccc);
//	ccc->Init(static_cast<GameObject*>(data));
//
//	return go;
//};

GameObject* GameObjectManager::CreateBlueBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_BLUE_BUBBLE);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);


	node_comp->Init(position, m_scene_manager);
	mrc->Init("BlueBubble.mesh", m_scene_manager);
	mrc->GetEntity()->setRenderQueueGroup(60);
	Ogre::Vector3 scale(def.start_scale);
	node_comp->GetSceneNode()->setScale(scale);
	
	//bc->SetCustomVariables(VariableManager::GetSingletonPtr()->GetAsFloat("Blue_Bubble_Life_Time"));

	RigidBodyDef body_def;
	body_def.body_type = DYNAMIC_BODY;
	body_def.collider_type = COLLIDER_SPHERE;
	body_def.friction = def.friction;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_BUBBLE;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_WORLD_STATIC | COL_QUESTITEM;
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, body_def);
	rc->SetId("body");
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	rc->GetRigidbody()->setDamping(VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Linear_Damping"), VariableManager::GetSingletonPtr()->GetAsFloat("Bubble_Angular_Damping"));
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));
	bc->Init(m_physics_engine, m_message_system, VariableManager::GetSingletonPtr()->GetAsFloat("OnBubbleImpulse"), VariableManager::GetSingletonPtr()->GetAsFloat("OnBubbleMaxVelocity"), def.start_scale);
	this->CheckBubbleSize();
	m_bubbles.push_back(go);
	return go;
}

GameObject* GameObjectManager::CreatePinkBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	BubbleDef& def = *static_cast<BubbleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_PINK_BUBBLE);
	BubbleController* bc = new BubbleController;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	Point2PointConstraintComponent* cons = new Point2PointConstraintComponent;
	go->AddComponent(cons);

	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);

	node_comp->Init(position, m_scene_manager);
	mrc->Init("PinkBubble.mesh", m_scene_manager);
	mrc->GetEntity()->setRenderQueueGroup(60);
	Ogre::Vector3 scale(def.start_scale);
	node_comp->GetSceneNode()->setScale(scale);
	
	//bc->SetCustomVariables(VariableManager::GetSingletonPtr()->GetAsFloat("Pink_Bubble_Life_Time"));

	//mrc->GetEntity()->setMaterialName("PinkBubble");
	RigidBodyDef body_def;
	body_def.body_type = DYNAMIC_BODY;
	body_def.collider_type = COLLIDER_SPHERE;
	body_def.friction = def.friction;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_BUBBLE;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_WORLD_STATIC | COL_QUESTITEM;
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, body_def);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	//rc->GetRigidbody()->setLinearFactor(btVector3(1,0,1));
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	//rc->GetRigidbody()->setDamping(0.5, 0.5);
	rc->SetId("body");
	cons->Init(m_physics_engine,rc->GetRigidbody(), def.connection_body, btVector3(0,0,0), btVector3(0,0,0));
	bc->Init(m_physics_engine, m_message_system, VariableManager::GetSingletonPtr()->GetAsFloat("OnBubbleImpulse"), VariableManager::GetSingletonPtr()->GetAsFloat("OnBubbleMaxVelocity"), def.start_scale);
	this->CheckBubbleSize();
	m_bubbles.push_back(go);
	return go;
}

GameObject* GameObjectManager::CreateTott(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	static int tott_counter = 0;
	Ogre::String tott_id = "Tott" + NumberToString(tott_counter);
	TottDef& def = *static_cast<TottDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_TOTT, tott_id);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	TottAIComponent* ai_comp = new TottAIComponent;
	go->AddComponent(ai_comp);
	go->AddUpdateable(ai_comp);
	AnimationComponent* acomp = new AnimationComponent;
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	
	node_comp->Init(position, m_scene_manager);
	node_comp->SetId(tott_id);
	def.node_name = node_comp->GetSceneNode()->getName();
	acomp->Init(def.mesh_name, m_scene_manager);
	

	
	if (def.type_name == "Hidehog"){
		acomp->AddAnimationState("Run", true);
	}
	else if (def.type_name == "Kittyshroom"){
		acomp->AddAnimationState("walk", true);
	}
	else if (def.type_name == "Nightcap"){
		acomp->AddAnimationState("walk", true);
	}
	else if (def.type_name == "Shroomfox"){
		acomp->AddAnimationState("idle", true);
	}
	else{
		acomp->AddAnimationState("Run", true);
	}
	ai_comp->Init(def.ai_states);
	m_sound_manager->GetTottNode(node_comp->GetSceneNode()->getName());
	contr->Init(position, m_physics_engine, def.character_controller);
	contr->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	contr->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	acomp->GetEntity()->setRenderingDistance(100.0f);
	tott_counter++;
	return go;
}

GameObject* GameObjectManager::CreateQuestTott(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	static int quest_tott_counter = 0;
	Ogre::String tott_id = "Tott" + NumberToString(quest_tott_counter);
	TottDef& def = *static_cast<TottDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_QUEST_TOTT, tott_id);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	TottAIComponent* ai_comp = new TottAIComponent;
	go->AddComponent(ai_comp);
	go->AddUpdateable(ai_comp);
	AnimationComponent* acomp = new AnimationComponent;
	go->AddComponent(acomp);
	go->AddUpdateable(acomp);
	CharacterController* contr = new CharacterController;
	go->AddComponent(contr);
	go->AddUpdateable(contr);
	ChildSceneNodeComponent* child_node = new ChildSceneNodeComponent;
	go->AddComponent(child_node);
	MeshRenderComponent* spbubble = new MeshRenderComponent;
	go->AddComponent(spbubble);
	SyncedTriggerComponent* trcomp = new SyncedTriggerComponent;
	go->AddComponent(trcomp);
	go->AddUpdateable(trcomp);
	TottController* tott_contr = new TottController;
	go->AddComponent(tott_contr);
	go->AddUpdateable(tott_contr);
	
	node_comp->Init(position, m_scene_manager);
	node_comp->SetId(tott_id);
	def.node_name = node_comp->GetSceneNode()->getName();
	acomp->Init(def.mesh_name, m_scene_manager);
	
	child_node->Init(Ogre::Vector3(0,def.speech_bubble_y,0), "speech_bubble", node_comp->GetSceneNode());
	child_node->SetId("speech_bubble");
	spbubble->Init(def.quest_item_mesh_name, m_scene_manager, "speech_bubble");
	child_node->GetNode()->attachObject(spbubble->GetEntity());
	spbubble->GetEntity()->setCastShadows(false);
	child_node->GetNode()->setScale(0,0,0);

	if (def.type_name == "Hidehog"){
		acomp->AddAnimationState("Run", true);
	}
	else if (def.type_name == "Kittyshroom"){
		acomp->AddAnimationState("walk", true);
	}
	else if (def.type_name == "Nightcap"){
		acomp->AddAnimationState("walk", true);
	}
	else if (def.type_name == "Shroomfox"){
		acomp->AddAnimationState("idle", true);
	}
	else{
		acomp->AddAnimationState("Run", true);
	}
	ai_comp->Init(def.ai_states);
	m_sound_manager->GetTottNode(node_comp->GetSceneNode()->getName());
	contr->Init(position, m_physics_engine, def.character_controller);
	contr->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	contr->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);

	tott_contr->Init(m_physics_engine, m_scene_manager, def.idle_anim, def.excited_anim, def.quest_item);

	TriggerDef trdef;
	trdef.body_type = STATIC_BODY;
	trdef.collider_type = COLLIDER_SPHERE;
	trdef.collision_filter.filter = COL_WORLD_TRIGGER;
	trdef.collision_filter.mask = COL_PLAYER;
	trdef.mass = 1.0f;
	trdef.radius = 4.0f;

	trcomp->Init(position, m_physics_engine, &trdef);
	acomp->GetEntity()->setRenderingDistance(100.0f);
	quest_tott_counter++;
	return go;
};

GameObject* GameObjectManager::CreateSpeechBubble(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	//Ogre::SceneNode* node = *static_cast<Ogre::SceneNode**>(data);

	std::ostringstream stream;
	stream << "Speech_Bubble_" << m_speech_bubble_iterations;
	m_speech_bubble_iterations++;
	Ogre::String spbubble_id = stream.str();

	GameObject* tott = *static_cast<GameObject**>(data);
	Ogre::SceneNode* node = static_cast<NodeComponent*>(tott->GetComponent(COMPONENT_NODE))->GetSceneNode();
	TottDef tott_def = static_cast<TottController*>(tott->GetComponent(COMPONENT_CHARACTER_CONTROLLER))->m_def;

	static_cast<TottController*>(tott->GetComponent(COMPONENT_CHARACTER_CONTROLLER))->SetSpeechBubble(spbubble_id);

	GameObject* go = new GameObject(GAME_OBJECT_SPEECH_BUBBLE, spbubble_id);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	SyncedTriggerComponent* trc = new SyncedTriggerComponent;
	go->AddComponent(trc);
	go->AddUpdateable(trc);
	SpeechBubbleComponent* sbcomp = new SpeechBubbleComponent;
	go->AddComponent(sbcomp);
	go->AddUpdateable(sbcomp);



	node_comp->Init(node, m_scene_manager);
	node_comp->SetId(spbubble_id);
	mrc->Init("PratBubblaCherry.mesh", m_scene_manager, node_comp->GetSceneNode()->getName());//, node->getName());
	sbcomp->Init(node, m_scene_manager, tott);
	
	/*
	node_comp->Init(position, m_scene_manager);
	mrc->Init("PratBubbla.mesh", m_scene_manager);//, m_temp_node->getName());//, node->getName());
	sbcomp->Init(m_temp_node, m_scene_manager);
	*/

	TriggerDef trdef;
	trdef.body_type = DYNAMIC_BODY;
	trdef.collider_type = COLLIDER_SPHERE;
	trdef.mass = 0.0f;
	trdef.radius = 2.5f;
	trdef.collision_filter.filter = COL_WORLD_TRIGGER;
	trdef.collision_filter.mask = COL_PLAYER;
	trc->Init(position, m_physics_engine, &trdef);

	return go;
};

GameObject* GameObjectManager::CreateQuestItem(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	
	GameObject* tott = *static_cast<GameObject**>(data);
	//Ogre::SceneNode* node = static_cast<NodeComponent*>(tott->GetComponent(COMPONENT_NODE))->GetSceneNode();
	//TottDef tott_def = static_cast<TottController*>(tott->GetComponent(COMPONENT_CHARACTER_CONTROLLER))->m_def;
	QuestItemDef& quest_item_def = *static_cast<QuestItemDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_QUEST_ITEM);
	DestroyCallbackComponent* dcc = new DestroyCallbackComponent;
	go->AddComponent(dcc);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	RotationComponent* rot_comp = new RotationComponent;
	go->AddComponent(rot_comp);
	go->AddUpdateable(rot_comp);
	StringComponent* string_comp = new StringComponent;
	go->AddComponent(string_comp);


	string_comp->Init(quest_item_def.id);
	std::function<void()> func = [&] { IEvent evt; evt.m_type = EVT_QUEST_ITEM_REMOVE; m_message_system->Notify(&evt); };
	dcc->Init(func);

	std::ostringstream stream;
	stream << "Quest_Item_" << m_quest_item_iterations;
	m_quest_item_iterations++;
	Ogre::String quest_item_id = stream.str();

	node_comp->Init(position, m_scene_manager);
	node_comp->SetId(quest_item_id);
	mrc->Init(quest_item_def.mesh_name, m_scene_manager);
	
	RigidBodyDef body_def;
	body_def.body_type = DYNAMIC_BODY;
	body_def.collider_type = COLLIDER_SPHERE;
	body_def.friction = 1.0;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_QUESTITEM;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_WORLD_STATIC | COL_BUBBLE;
	body_def.sync_orientation = false;
	rc->Init(position,  mrc->GetEntity(), m_physics_engine, body_def);
	rc->GetRigidbody()->setGravity(btVector3(0.0f, -0.3f, 0.0f));
	rc->GetRigidbody()->setContactProcessingThreshold(btScalar(0));
	rc->GetRigidbody()->setActivationState(DISABLE_DEACTIVATION);
	rc->GetRigidbody()->setDamping(0.5, 0.5);
	rc->SetId(quest_item_id);
	rot_comp->Init(node_comp->GetSceneNode(), VariableManager::GetSingletonPtr()->GetAsFloat("QuestItemRotationSpeed"));

	return go;
};

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
	def.collision_filter.mask = COL_PLAYER | COL_BUBBLE | COL_TOTT;
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
	
	go->SetGameObjectManager(this);
	
	return go;
}

GameObject* GameObjectManager::CreateGUI(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GuiDef& guiDef = *static_cast<GuiDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_GUI);

	CountableResourceGUI* gui = new CountableResourceGUI;
	go->AddComponent(gui);

	//gui->Init(guiDef.tex_inact, guiDef.tex_act, guiDef.num_elems);

	return go;
};

GameObject* GameObjectManager::CreateLeaf(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	ParticleDef& particleDef = *static_cast<ParticleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_LEAF);
	ParticleComponent* particle = new ParticleComponent;
	go->AddComponent(particle);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	MeshRenderComponent* mrc = new MeshRenderComponent;
	go->AddComponent(mrc);
	BobbingComponent* bc = new BobbingComponent;
	go->AddComponent(bc);
	go->AddUpdateable(bc);
	TriggerComponent* stc = new TriggerComponent;
	go->AddComponent(stc);


	Ogre::Vector3 scale = Ogre::Vector3(0.6f);
	node_comp->Init(position, m_scene_manager);
	node_comp->GetSceneNode()->setPosition(Ogre::Vector3(position));
	node_comp->GetSceneNode()->setScale(scale);
	mrc->Init("Collectable_Leaf.mesh", m_scene_manager);
	mrc->GetEntity()->setMaterialName("CollectibleLeaf");
	bc->Init(node_comp->GetSceneNode());

	std::ostringstream stream;
	stream << "Leaf_" << m_leaf_iterations;
	m_leaf_iterations++;
	Ogre::String leaf_id = stream.str();

	particle->Init(m_scene_manager, leaf_id, particleDef.particle_name, node_comp->GetSceneNode()->getPosition(), node_comp->GetSceneNode());
	//particle->CreateParticle(node_comp->GetSceneNode(), node_comp->GetSceneNode()->getPosition(), Ogre::Vector3(0,-1.8f,0));

	TriggerDef trdef;
	trdef.body_type = STATIC_BODY;
	trdef.collider_type = COLLIDER_SPHERE;
	trdef.mass = 0.0f;
	trdef.radius = 0.4f;
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
	ButtonDef& buttonDef = *static_cast<ButtonDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_COMPANION);
	TimerComponent* timer = new TimerComponent;
	go->AddComponent(timer);
	go->AddUpdateable(timer);

	TriggerComponent* tc = new TriggerComponent;
	go->AddComponent(tc);

	return go;
}

GameObject* GameObjectManager::CreateTerrain(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_TERRAIN, "Terrain");
	TerrainComponent* tc = new TerrainComponent;
	go->AddComponent(tc);
	go->AddUpdateable(tc);
	tc->Init(m_scene_manager, m_physics_engine, this, m_sound_manager, *static_cast<Ogre::String*>(data));
	return go;
}

GameObject* GameObjectManager::CreateGate(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GateDef& def = *static_cast<GateDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_GATE);
	NodeComponent* nc = new NodeComponent;
	go->AddComponent(nc);
	MeshRenderComponent* left_gate_mesh = new MeshRenderComponent;
	go->AddComponent(left_gate_mesh);
	MeshRenderComponent* right_gate_mesh = new MeshRenderComponent;
	go->AddComponent(right_gate_mesh);
	ChildSceneNodeComponent* left_gate_node = new ChildSceneNodeComponent;
	go->AddComponent(left_gate_node);
	ChildSceneNodeComponent* right_gate_node = new ChildSceneNodeComponent;
	go->AddComponent(right_gate_node);

	RigidbodyComponent* rc = new RigidbodyComponent;
	go->AddComponent(rc);
	GateControllerComponent* gate_controller = new GateControllerComponent;
	go->AddComponent(gate_controller);
	go->AddUpdateable(gate_controller);

	nc->Init(position, m_scene_manager);
	left_gate_mesh->Init("GateLeft.mesh", m_scene_manager, Ogre::StringUtil::BLANK);
	right_gate_mesh->Init("GateRight.mesh", m_scene_manager, Ogre::StringUtil::BLANK);
	left_gate_node->Init(Ogre::Vector3(2.7f,0,0), "left_gate_node", nc->GetSceneNode());
	left_gate_node->SetId("left_gate_node");
	right_gate_node->Init(Ogre::Vector3(-2.7f,0,0), "right_gate_node", nc->GetSceneNode());
	right_gate_node->SetId("right_gate_node");
	left_gate_node->GetNode()->attachObject(left_gate_mesh->GetEntity());
	right_gate_node->GetNode()->attachObject(right_gate_mesh->GetEntity());

	RigidBodyDef body_def;
	body_def.body_type = STATIC_BODY;
	body_def.collider_type = COLLIDER_BOX;
	body_def.mass = 1.0f;
	body_def.collision_filter.filter = COL_WORLD_STATIC;
	body_def.collision_filter.mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_QUESTITEM;
	body_def.collider_def.x = 10.0f;
	body_def.collider_def.y = 12.0f;
	body_def.collider_def.z = 0.5f;

	rc->Init(position, m_physics_engine, body_def);
	gate_controller->Init(m_message_system, def.leaves);
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

GameObject* GameObjectManager::CreateParticleEffect(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	ParticleDef& particleDef = *static_cast<ParticleDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_PARTICLE, id);

	ParticleComponent* particle = new ParticleComponent;
	go->AddComponent(particle);
	NodeComponent* node_comp = new NodeComponent;
	go->AddComponent(node_comp);
	node_comp->Init(position, m_scene_manager);
	node_comp->GetSceneNode()->setPosition(Ogre::Vector3(position));

	std::ostringstream stream;
	stream << "Particle_" << m_particle_iterations;
	m_particle_iterations++;
	Ogre::String particle_id = stream.str();

	particle->Init(m_scene_manager, particle_id, particleDef.particle_name, Ogre::Vector3(0,0,0), node_comp->GetSceneNode());
	return go;
};

GameObject* GameObjectManager::CreateRockSlide(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	GameObject* go = new GameObject(GAME_OBJECT_ROCK_SLIDE, id);
	NodeComponent* node_component = new NodeComponent;
	go->AddComponent(node_component);
	MeshRenderComponent* mesh = new MeshRenderComponent;
	go->AddComponent(mesh);
	RigidbodyComponent* body = new RigidbodyComponent;
	go->AddComponent(body);
	TriggerComponent* trigger = new TriggerComponent;
	go->AddComponent(trigger);

	RigidBodyDef body_def;
	body_def.body_type = COLLIDER_TRIANGLE_MESH_SHAPE;
	body_def.collision_filter.filter = COL_WORLD_STATIC;
	body_def.collision_filter.mask = COL_BUBBLE | COL_PLAYER | COL_QUESTITEM | COL_TOTT;
	
	return go;
}

GameObject* GameObjectManager::CreateLevelChange(const Ogre::Vector3& position, void* data, const Ogre::String& id){
	TriggerDef& def = *static_cast<TriggerDef*>(data);
	GameObject* go = new GameObject(GAME_OBJECT_LEVEL_CHANGE, id);
	TriggerComponent* trigger_comp = new TriggerComponent;
	go->AddComponent(trigger_comp);

	trigger_comp->Init(position, m_physics_engine, def);
	return go;
}
