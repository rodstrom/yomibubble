#include "..\stdafx.h"
#include "PlayState.h"
#include "..\Managers\InputManager.h"
#include "..\InputSystem.h"
#include "..\PhysicsEngine.h"
#include "..\Components\GameObject.h"
#include "..\Components\Components.h"
#include "..\Components\ComponentMessenger.h"
#include "..\Components\PlayerController.h"

PlayState::PlayState(void) : m_physics_engine(NULL){}
PlayState::~PlayState(void){}

void PlayState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_physics_engine->SetDebugDraw(m_scene_manager);
	m_camera = m_scene_manager->createCamera("Camera");
	m_camera->setPosition(Ogre::Vector3(0,0,50));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(5);
	Ogre::Viewport* viewport = m_render_window->addViewport(m_camera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,1.0));
	m_camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
	Ogre::Light* light = m_scene_manager->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 200, 200, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	m_plane = m_scene_manager->createEntity("LightPlaneEntity", "plane");
	m_scene_manager->getRootSceneNode()->createChildSceneNode()->attachObject(m_plane);
	m_plane->setMaterialName("Examples/BeachStones");
	BtOgre::StaticMeshToShapeConverter converter1(m_plane);
	m_plane_shape = converter1.createTrimesh();
	m_ground_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
	m_plane_body = new btRigidBody(0, m_ground_motion_state, m_plane_shape, btVector3(0,0,0));
	m_physics_engine->AddRigidBody(m_plane_body);

	m_messenger = new ComponentMessenger;
	m_sinbad = new GameObject;

	Animation* renderer = new Animation;
	renderer->SetType(COMPONENT_RENDERER);
	renderer->SetOwner(m_sinbad);
	renderer->SetMessenger(m_messenger);
	renderer->Init("sinbad.mesh", m_scene_manager);
	renderer->AddAnimationStates(2);
	m_sinbad->AddComponent(renderer);
	m_sinbad->AddUpdateable(renderer);
	Rigidbody* body = new Rigidbody;
	body->SetType(COMPONENT_RIGIDBODY);
	body->SetOwner(m_sinbad);
	body->SetMessenger(m_messenger);
	body->Init(renderer->GetEntity(), renderer->GetSceneNode(), m_physics_engine);
	m_sinbad->AddComponent(body);
	PlayerController* controller = new PlayerController;
	controller->SetType(COMPONENT_CONTROLLER);
	controller->SetOwner(m_sinbad);
	controller->SetMessenger(m_messenger);
	controller->Init(m_input_manager);
	m_sinbad->AddComponent(controller);
	m_sinbad->AddUpdateable(controller);
	Transform* transform = new Transform;
	transform->SetType(COMPONENT_NONE);
	transform->SetOwner(m_sinbad);
	transform->SetMessenger(m_messenger);
	m_sinbad->AddComponent(transform);

	/*Ogre::Vector3 pos = Ogre::Vector3(0,200,0);
	Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY;

	m_penguin = m_scene_manager->createEntity("penguin", "penguin.mesh");
	m_penguin_node = m_scene_manager->getRootSceneNode()->createChildSceneNode("penguinNode", pos, rot);
	m_penguin_node->attachObject(m_penguin);
	BtOgre::StaticMeshToShapeConverter converter2(m_penguin);
	m_penguin_shape = converter2.createSphere();
	btScalar mass = 5;
	btVector3 inertia;
	m_penguin_shape->calculateLocalInertia(mass, inertia);
	m_penguin_state = new BtOgre::RigidBodyState(m_penguin_node);
	m_penguin_body = new btRigidBody(mass, m_penguin_state, m_penguin_shape, inertia);
	m_physics_engine->AddRigidBody(m_penguin_body);
	m_physics_engine->ShowDebugDraw(false);

	Ogre::AnimationStateIterator it = m_penguin->getAllAnimationStates()->getAnimationStateIterator();
	while(it.hasMoreElements()){
		Ogre::AnimationState* astate = it.getNext();
		std::string line = astate->getAnimationName();
		std::cout << line << "\n";
	}*/
	m_physics_engine->ShowDebugDraw(false);
	m_scene_manager->setSkyDome(true, "Examples/CloudySky");
}

void PlayState::Exit(){
	m_physics_engine->RemoveRigidBody(m_plane_body);
	//m_physics_engine->RemoveRigidBody(m_penguin_body);

	/*m_penguin_body->getMotionState();
	delete m_penguin_body;
	delete m_penguin_shape;
	delete m_penguin_state;*/

	m_plane_body->getMotionState();
	delete m_plane_body;
	delete m_ground_motion_state;
	m_plane_shape->getMeshInterface();
	delete m_plane_shape;

	m_sinbad->Shut();
	delete m_sinbad;
	m_sinbad = NULL;
	delete m_messenger;
	m_messenger = NULL;

	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	m_scene_manager = NULL;
}

bool PlayState::frameStarted(const Ogre::FrameEvent& evt){
	return true;
}

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt){
	m_sinbad->Update(evt.timeSinceLastFrame);
	float cameraMovementSpeed = 50.0f;
	Ogre::Vector3 translateCamera(0,0,0);
	if (m_input_manager->IsButtonDown(BTN_W))
		translateCamera += Ogre::Vector3(0,0,-1);
	if (m_input_manager->IsButtonDown(BTN_S))
		translateCamera += Ogre::Vector3(0,0,1);
	if (m_input_manager->IsButtonDown(BTN_A))
		translateCamera += Ogre::Vector3(-1,0,0);
	if (m_input_manager->IsButtonDown(BTN_D))
		translateCamera += Ogre::Vector3(1,0,0);
	m_camera->moveRelative(translateCamera*evt.timeSinceLastFrame*cameraMovementSpeed);
	float rotX = m_input_manager->GetMousePosition().rel_x * evt.timeSinceLastFrame * -1;
	float rotY = m_input_manager->GetMousePosition().rel_y * evt.timeSinceLastFrame * -1;
	m_camera->yaw(Ogre::Radian(rotX));
	m_camera->pitch(Ogre::Radian(rotY));

	//CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	if (m_input_manager->IsButtonPressed(BTN_BACK)){
		return false;
	}

	m_physics_engine->Step(evt.timeSinceLastFrame, 10);
	return true;
}
