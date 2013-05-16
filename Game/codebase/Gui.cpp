#include "Gui.h"
#include <vector>
#include <OGRE\SdkTrays.h>	
#include "InputSystem.h"

Gui::Gui(void)
{
}


Gui::~Gui(void)
{
}

void Gui::createGui(Ogre::SceneManager* p_scene_manager, OgreBites::SdkTrayManager* p_tray_manager){
	m_tray_manager = p_tray_manager;
	m_button = m_tray_manager->createButton(OgreBites::TL_TOPLEFT, "MyButton", "Click me!");

	if(m_button->getName() == "MyButton"){
		int p = 0;
	}

	// Load layout
   Ogre::ResourceGroupManager::getSingletonPtr();
   Ogre::OverlayManager* mg = Ogre::OverlayManager::getSingletonPtr();
   Ogre::Overlay* ov = mg->getByName("testOverlay");
   ov->show();


}

void Gui::ButtonHit(){
	
	
	m_tray_manager->buttonHit(m_button);
	
}