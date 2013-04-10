#include "GuiManager.h"


GuiManager::GuiManager()
{
}


GuiManager::~GuiManager()
{
}

void GuiManager::CreateButton(OgreBites::SdkTrayManager* p_tray_manager){
	m_tray_manager = p_tray_manager;
	m_button = m_tray_manager->createButton(OgreBites::TL_TOPLEFT, "MyButton", "Click me!");
}

void GuiManager::CreateOverlayHUD(){
   Ogre::ResourceGroupManager::getSingletonPtr();
   m_overlay_manager = Ogre::OverlayManager::getSingletonPtr();
   m_overlay = m_overlay_manager->getByName("testOverlay");
   m_overlay->show();
}

void GuiManager::ButtonHit(OgreBites::Button* p_button){

}