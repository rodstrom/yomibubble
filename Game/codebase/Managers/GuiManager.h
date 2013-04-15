#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include <OGRE\SdkTrays.h>
#include "..\stdafx.h"


class GuiManager
{
public:
	GuiManager();
	virtual ~GuiManager();

	void CreateButton(OgreBites::SdkTrayManager*);
	void CreateOverlayHUD();
	void ButtonHit(OgreBites::Button*);

private:
	OgreBites::Button* m_button;
	Ogre::OverlayManager* m_overlay_manager;
	OgreBites::SdkTrayManager* m_tray_manager;
	Ogre::Overlay* m_overlay;
};

#endif //GUI_MANAGER_H