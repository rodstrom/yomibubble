#include "Gui.h"
#include <vector>

Gui::Gui(void)
{
}


Gui::~Gui(void)
{
}

void Gui::createGui(Ogre::SceneManager* p_scene_manager)
{
	// Load layout
   Ogre::ResourceGroupManager::getSingletonPtr();
   Ogre::OverlayManager*   mg = Ogre::OverlayManager::getSingletonPtr();
   Ogre::Overlay*         ov = mg->getByName("testOverlay");
   ov->show();

	/*Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
         // Create an overlay
         Ogre::Overlay* overlay = overlayManager.create( "OverlayName" );
 
         // Create a panel
         Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "PanelName" ) );
         panel->setPosition( 0.0, 0.0 );
         panel->setDimensions( 0.1, 0.1 );
         panel->setMaterialName( "BaseWhite" );
         // Add the panel to the overlay
         overlay->add2D( panel );
 
         // Show the overlay
         overlay->show();*/


	/*Ogre::SceneNode* myNode = static_cast<Ogre::SceneNode*>(p_scene_manager->getRootSceneNode()->createChild());
	Ogre::BillboardSet* mySet = p_scene_manager->createBillboardSet("mySet");
	Ogre::Billboard* myBillboard = mySet->createBillboard(Ogre::Vector3(100, 0, 200));
	myNode->attachObject(mySet);*/
}