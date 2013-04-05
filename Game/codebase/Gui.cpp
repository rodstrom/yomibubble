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
	Ogre::SceneNode* myNode = static_cast<Ogre::SceneNode*>(p_scene_manager->getRootSceneNode()->createChild());
	Ogre::BillboardSet* mySet = p_scene_manager->createBillboardSet("mySet");
	Ogre::Billboard* myBillboard = mySet->createBillboard(Ogre::Vector3(100, 0, 200));
	myNode->attachObject(mySet);
}