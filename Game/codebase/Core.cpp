#include "stdafx.h"
#include "Core.h"
#include "Game.h"
#include "InputSystem.h"
#include "MessageSystem.h"
#include "Managers\SoundManager.h"
#include "Managers\CollisionManager.h"
#include "Managers\VariableManager.h"

Core::Core(void) : m_root(NULL), m_game(NULL), m_message_system(NULL), m_input_system(NULL), m_run(true) {}
Core::~Core(void){}

bool Core::Init(){
	m_message_system = new MessageSystem;
	m_message_system->Register(EVT_QUIT, this, &Core::QuitMsg);
	CollisionManager::GetSingletonPtr()->SetMessageSystem(m_message_system);
	m_root = OGRE_NEW Ogre::Root;
#ifdef _DEBUG
	m_root->loadPlugin("RenderSystem_Direct3D9_d");
	m_root->loadPlugin("Plugin_CgProgramManager_d");
	m_root->loadPlugin("Plugin_OctreeSceneManager_d");
	m_root->loadPlugin("Plugin_ParticleFX_d");
	m_root->loadPlugin("OgreOggSound_d");
#else
	m_root->loadPlugin("RenderSystem_Direct3D9");
	m_root->loadPlugin("Plugin_CgProgramManager");
	m_root->loadPlugin("Plugin_OctreeSceneManager");
	m_root->loadPlugin("Plugin_ParticleFX");
	m_root->loadPlugin("OgreOggSound");
#endif
	// We hardcode our resource locations so users can't change them.
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/fonts", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/programs", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/overlays", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/bloom", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/particles", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/cg_shaders", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/depth_shadowmap", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/scripts/programs", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures/glow_maps", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures/normal_maps", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures/specular_maps", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures/GUI", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/materials/textures/HUD", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/models", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/models/grass", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/models/characters", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/terrain", "FileSystem", "Popular");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/audio", "FileSystem", "Popular");
	//Ogre::RenderSystem* rs = m_root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	//m_root->setRenderSystem(rs);
	if (!m_root->showConfigDialog()){
		return false;
	}

	//
	

	/*Ogre::ConfigFile videoCfg;
	videoCfg.load(Ogre::String("../../resources/config/ogre.cfg"));
	Ogre::String rendersystem = videoCfg.getSetting("Render System", Ogre::StringUtil::BLANK, "Direct3D9 Rendering Subsystem");
	Ogre::String fullscreen = videoCfg.getSetting("Full Screen", rendersystem, "No");
	Ogre::String videoMode = videoCfg.getSetting("Video Mode", rendersystem, "1024 x 768 @ 32-bit colour");


	
	rs->setConfigOption("Full Screen", fullscreen);
	rs->setConfigOption("Video Mode", videoMode);
	rs->setConfigOption("VSync", "Yes");*/

	m_render_window = m_root->initialise(true, "Yomi's Bubble Adventure");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	if (!VariableManager::GetSingletonPtr()->Init())
		return false;
	//m_sound_manager = new SoundManager();
	//m_sound_manager->LoadAudio();
	m_game = new Game;
	//m_game->Init(m_render_window, m_message_system, m_sound_manager, m_scene_manager);
	m_game->Init(m_render_window, m_message_system);
	m_input_system = new InputSystem(m_game, m_render_window, m_message_system);
	m_input_system->Init();
	gContactAddedCallback = Collision::ContactCallback;
	return true;
}

void Core::Run(){
	Ogre::Timer timer;
	double last_time = 0.0;
	double fps_cap = 60.0;
	double dt = 0.0f;
	m_root->clearEventTimes();
	while (m_run){
		Ogre::WindowEventUtilities::messagePump();
		bool render = true;
		double curr_sec = (double)timer.getMilliseconds() * 0.001;
		dt = curr_sec - last_time;

		double limit = 1.0 / fps_cap;
		if (dt < limit){
			render = false;
		}
		else {
			if (dt > 0.5){
				dt = 0.0;
			}
			last_time = curr_sec;
		}

		if (render){
			m_game->UpdateInput();
			m_input_system->Capture();
			if (!m_game->Update(dt)){
				return;
			}
			if (!m_root->renderOneFrame((float)dt)){
				return;
			}
		}
	}
}

void Core::Shut(){
	if (m_message_system){
		delete m_message_system;
		m_message_system = NULL;
	}
	if (m_game){
		m_game->Shut();
		delete m_game;
		m_game = NULL;
	}
	if (m_input_system){
		m_input_system->Shut();
		delete m_input_system;
		m_input_system = NULL;
	}
	if (m_root){
		OGRE_DELETE m_root;
		m_root = NULL;
		m_render_window = NULL;
	}
}

void Core::QuitMsg(IEvent* evt){
	m_run = false;
}