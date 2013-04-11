#include "stdafx.h"
#include "Core.h"
#include "BubbleAdventure.h"
#include "InputSystem.h"
#include "MessageSystem.h"
#include "SoundManager.h"

Core::Core(void) : m_root(NULL), m_bubble_adventure(NULL), m_message_system(NULL), m_input_system(NULL) {}
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

	//Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

	m_bubble_adventure = new BubbleAdventure;
	m_bubble_adventure->Init(m_render_window, m_message_system);
	m_input_system = new InputSystem(m_bubble_adventure, m_render_window);
	m_input_system->Init();

	m_single_scene_mgr = m_root->createSceneManager ( Ogre::ST_GENERIC, "Default SceneManager" );
	m_sound_manager = new SoundManager(m_single_scene_mgr);
	m_sound_manager->LoadAudio();
	m_sound_manager->Play2DSound("Sound1");

	return true;
}

void Core::Run(){
	while (1){
		m_bubble_adventure->Update();
		m_input_system->Capture();
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
		if (!m_root->renderOneFrame()){
			return;
		}
	}
}

void Core::Shut(){
	if (m_message_system){
		delete m_message_system;
		m_message_system = NULL;
	}
	if (m_bubble_adventure){
		m_bubble_adventure->Shut();
		delete m_bubble_adventure;
		m_bubble_adventure = NULL;
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