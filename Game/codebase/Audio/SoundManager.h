#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

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

#include <map>

using namespace OgreOggSound;

struct SoundData3D{
	Ogre::String m_name;
	Ogre::Vector3 m_position;
	Ogre::SceneNode* m_scene_node;
	//Ogre::SceneNode m_node;
};

class SoundManager
{
public:
	//SoundManager();
	SoundManager(Ogre::SceneManager* scene_manager, Ogre::Camera* camera);
	~SoundManager();

	    void            playSoundOrRestart(OgreOggISound *sound);
 //   OgreOggISound*  getSoundInstance(SoundType h,   int uniqueID, Ogre::SceneNode *attachTo, bool loop = false);
 //   OgreOggISound*  getSoundInstance(PowerupType p, int uniqueID, Ogre::SceneNode *attachTo);
    void            deleteSoundInstance(OgreOggISound* sound);
    void            frameEvent(Ogre::Real timeSinceLastFrame);
   
	void LoadAudio();

	void Play2DSound(Ogre::String name);
	void Stop2DSound(Ogre::String name);
	void Play3DSound(Ogre::String name, Ogre::Vector3 position);
	void Stop3DSound(Ogre::String name);

	void Play2DMusic(Ogre::String name);
	void Stop2DMusic(Ogre::String name);

	OgreOggSoundManager* GetSoundManager() { return m_sound_manager; };

	void Update(Ogre::Camera* camera, float dt); //typ setta listenerpos och sånt mög?

	bool testbajs;

	std::map<int, Ogre::SceneNode*> m_scene_nodes;

private:
    Ogre::SceneNode* m_ear_node;
	Ogre::SceneManager* m_scene_manager;
	
	void processSoundDeletesPending();
   // OgreOggISound* getNamedSoundInstance(std::string name, std::string file, Ogre::SceneNode *attachTo, bool loop = false);

    OgreOggSoundManager* m_sound_manager;
    std::list<OgreOggISound*> *mSoundDeletesPending;
    bool mInitOK;

    OgreOggISound *mBackingTrack;

	std::map<Ogre::String, OgreOggISound*> m_sounds;


private:

};

#endif //SOUND_MANAGER_H