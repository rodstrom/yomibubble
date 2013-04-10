#ifndef BILLBOARD_GUI_H
#define BILLBOARD_GUI_H

#include "BillboardGUI.h"
#include <OGRE\OgreBillboard.h>
#include "InputSystem.h"


class Gui
{
public:
	Gui(void);
	virtual ~Gui(void);

	void createGui(Ogre::SceneManager* p_scene_manager, OgreBites::SdkTrayManager*);
	void ButtonHit();

private:
	OgreBites::SdkTrayManager* m_tray_manager;
	OgreBites::Button* m_button;
};

#endif //BILLBOARD_GUI_H