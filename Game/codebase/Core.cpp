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
	Ogre::String pluginsCfg = Ogre::StringUtil::BLANK;
	Ogre::String resourceCfg  = "../../resources/config/resources.cfg";
#ifdef _DEBUG
	pluginsCfg = "plugins_d.cfg";
#else
	pluginsCfg = "plugins.cfg";
#endif

	m_root = OGRE_NEW Ogre::Root(pluginsCfg);

	Ogre::ConfigFile cf;
	cf.load(resourceCfg);

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements()){
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
		for (auto it = settings->begin(); it != settings->end(); it++){
			typeName = it->first;
			archName = it->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	Ogre::ConfigFile videoCfg;
	videoCfg.load(Ogre::String("../../resources/config/ogre.cfg"));
	Ogre::String rendersystem = videoCfg.getSetting("Render System", Ogre::StringUtil::BLANK, "Direct3D9 Rendering Subsystem");
	Ogre::String fullscreen = videoCfg.getSetting("Full Screen", rendersystem, "No");
	Ogre::String videoMode = videoCfg.getSetting("Video Mode", rendersystem, "1024 x 768 @ 32-bit colour");

	Ogre::RenderSystem* rs = m_root->getRenderSystemByName(rendersystem);
	m_root->setRenderSystem(rs);

	rs->setConfigOption("Full Screen", fullscreen);
	rs->setConfigOption("Video Mode", videoMode);
	rs->setConfigOption("VSync", "No");
	
	m_render_window = m_root->initialise(true, "Yomi's Bubble Adventure");
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(3);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	/*Ogre::SceneManager* retardedBugfix = */m_root->createSceneManager(Ogre::ST_GENERIC); // Herman was here
	if (!VariableManager::GetSingletonPtr()->Init())
		return false;
	m_sound_manager = new SoundManager();
	m_sound_manager->LoadAudio();

	//delete retardedBugfix;
	//retardedBugfix = NULL;

	m_game = new Game;
	m_game->Init(m_render_window, m_message_system, m_sound_manager);
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
			if (dt > 1.0){
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
		m_root->renderOneFrame();
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
	if (m_sound_manager){
		delete m_sound_manager;
		m_sound_manager = NULL;
	}
	OGRE_DELETE m_root;
	m_root = NULL;
	m_render_window = NULL;
}

void Core::QuitMsg(IEvent* evt){
	m_run = false;
}