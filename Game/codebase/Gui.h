#ifndef BILLBOARD_GUI_H
#define BILLBOARD_GUI_H

#include "BillboardGUI.h"
#include <OGRE\OgreBillboard.h>

class Gui
{
public:
	Gui(void);
	virtual ~Gui(void);

	void createGui(Ogre::SceneManager* p_scene_manager);
};

#endif //BILLBOARD_GUI_H