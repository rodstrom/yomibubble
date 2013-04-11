#include "stdafx.h"
#include "Core.h"

#include "OgreOggSound.h"
#include "OgreOggSoundManager.h"
#include "OgreOggSoundPrereqs.h"
#include "OgreOggSoundFactory.h"
#include "OgreOggISound.h"
#include "OgreOggListener.h"
#include "OgreOggSoundCallback.h"
#include "OgreOggSoundPlugin.h"
#include "OgreOggSoundRecord.h"
#include "OgreOggStaticSound.h"
#include "OgreOggStaticWavSound.h"
#include "OgreOggStreamSound.h"
#include "OgreOggStreamWavSound.h"


int main(int argc, char** argv){
	/*
	OgreOggSound::OgreOggSoundManager* m_sound_manager;
	m_sound_manager = nullptr;

	//m_sound_manager = new OgreOggSound::OgreOggSoundManager();
	m_sound_manager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	m_sound_manager->init();
	*/
	Core core;
	if (!core.Init())
		return -1;
	core.Run();
	core.Shut();
	return 0;
}