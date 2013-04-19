#include "stdafx.h"
#include "Core.h"
#include "Game.h"
#include "InputSystem.h"
#include "MessageSystem.h"
#include "Managers\SoundManager.h"

Core::Core(void) : m_root(NULL), m_game(NULL), m_message_system(NULL), m_input_system(NULL) {}
Core::~Core(void){}

bool Core::Init(){
	m_message_system = new MessageSystem;
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

	m_render_window = m_root->initialise(true, "Yomi's Bubble Adventure");
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	m_game = new Game;
	m_game->Init(m_render_window, m_message_system);
	m_input_system = new InputSystem(m_game, m_render_window);
	m_input_system->Init();

	return true;
}

void Core::Run(){
Ogre::Timer timer;
double dt = 0.0;
double last_time = 0.0;
double fps_cap = 60.0;
	while (1){
		bool render = true;
		double curr_sec = (double)timer.getMilliseconds() * 0.001;
		dt = curr_sec - last_time;
		double limit = 1.0 / fps_cap;
		if (dt < limit){
			render = false;
		}
		else {
			last_time = curr_sec;
			if (dt > 1.0){
				dt = 0.0;
			}
		}
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
				if (msg.message == WM_QUIT){
					return;
				}
				else{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
#endif
		if (render){
			m_game->UpdateInput();
			m_input_system->Capture();
			if (!m_game->Update(dt)){
				return;
			}
			m_root->renderOneFrame();
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
	OGRE_DELETE m_root;
	m_root = NULL;
	m_render_window = NULL;
}