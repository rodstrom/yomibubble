#include "..\stdafx.h"
#include "MenuState.h"

MenuState::MenuState(void){}
MenuState::~MenuState(void){}

void MenuState::Enter(){
	
}

void MenuState::Exit(){

}

bool MenuState::frameStarted(const Ogre::FrameEvent& evt){
	return true;
}

bool MenuState::frameRenderingQueued(const Ogre::FrameEvent& evt){
	return true;
}
