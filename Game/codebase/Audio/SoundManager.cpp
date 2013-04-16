#include "stdafx.h"
#include "SoundManager.h"

SoundManager::SoundManager(Ogre::SceneManager* scene_manager, Ogre::Camera* camera){

    m_sound_manager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    mSoundDeletesPending = new std::list<OgreOggISound*>;

	m_sound_manager->init();

	m_ear_node = scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0), Ogre::Quaternion::IDENTITY);
	m_ear_node->setPosition(camera->getPosition());
	m_ear_node->setOrientation(camera->getOrientation());
	m_ear_node->attachObject(m_sound_manager->getListener());
	
	m_sound_manager->setDistanceModel(AL_LINEAR_DISTANCE);

	m_scene_manager = scene_manager;

	testbajs = false;

	//camera->getHeadNode()->attachObject(m_sound_manager->getListener());
}

SoundManager::~SoundManager(){
    delete mSoundDeletesPending;
}

void SoundManager::LoadAudio(){
	m_sound_manager->createSound("Yomi_Walk", "SFX/Yomi/Yomi_Walk.wav", false, false, true);
	m_sound_manager->createSound("Dun_Dun", "SFX/Tott/Bristlestick_Zoom.wav", false, false, true);

	m_sound_manager->createSound("Main_Theme", "Music/Menu_theme.wav", false, true, true);
}

/// does what it says on the tin. can be given null sounds
void SoundManager::playSoundOrRestart(OgreOggISound *sound){
    // fix for some crash, if !mInitOK
    // (example of using optical output and some app is using it for encoded (i.e. dolby))
    if (!sound) return;

    if (sound->isPlaying()) sound->stop();
   
    sound->setPlayPosition(0);
    sound->play();
}

void SoundManager::Play2DMusic(Ogre::String name){
	m_sound_manager->getSound(name)->play();
};

void SoundManager::Stop2DMusic(Ogre::String name){

};

void SoundManager::Play3DSound(Ogre::String name, Ogre::Vector3 position){

	//testa med en bool att bara setta en gång

	if (!testbajs)
	{
	Ogre::SceneNode* node = m_scene_manager->getSceneNode("testnode");
	node->setPosition(position);
//	node->
	m_sound_manager->getSound(name)->setPosition(position);
	node->attachObject(m_sound_manager->getSound(name));
	m_sound_manager->getSound(name)->setReferenceDistance(10.0f);
	m_sound_manager->getSound(name)->setRolloffFactor(2.0f);
	m_sound_manager->getSound(name)->setMaxDistance(1000.0f);
	//m_sound_manager->getSound(name)->setListener(m_sound_manager->getListener());
	//m_sound_manager->getSound(name)->setMinDistance(1.0f);
	//m_sound_manager->getSound(name)->setListener(m_ear_node);
	m_sound_manager->getSound(name)->play();
	testbajs = true;
	}
	//m_sound_manager->getListener();

	/*
	if ( newSound = mSoundManager->createSound("Sound1", "sound.ogg", false, false, true) )
  {
    // Attach to a SceneNode
    Ogre::SceneNode* sNode=mSceneMgr->getSceneNode("NodeToAttachSoundTo");
    sNode->attachObject(newSound);
    newSound->setRolloffFactor(2.f);
    newSound->setReferenceDistance(10.f);
    newSound->play();
  }
	*/
}

void SoundManager::Stop3DSound(Ogre::String name){
	m_sound_manager->getSound(name)->stop();
};

void SoundManager::Update(Ogre::Camera* camera, float dt){

	m_sound_manager->update(dt);
	//Update earNode
    m_ear_node->setPosition(camera->getPosition());
    m_ear_node->setOrientation(camera->getOrientation());
}

/// it is quite possible mInitOK is false, so this may return null.
/// the ID needs to be unique to the sound type (can be same for different sounds)
/*
OgreOggISound* SoundManager::getSoundInstance(SoundType h, int uniqueID, Ogre::SceneNode *attachTo, bool loop)
{
    if (!mInitOK) return NULL;

    std::string file;
   
   

    return sound;
}
*/

/// it is quite possible mInitOK is false, so this may return null.
/// the ID needs to be unique to the sound type (can be same for different sounds)
/*
OgreOggISound* SoundManager::getSoundInstance(PowerupType p, int uniqueID, Ogre::SceneNode *attachTo)
{
    if (!mInitOK) return NULL;

    std::string file;
    switch (p)
    {
        case POWERUP_HEALTH: file = FILE_POWERUP_HEALTH; break;
        case POWERUP_MASS: file = FILE_POWERUP_HEAVY; break;
        case POWERUP_RANDOM: file = FILE_POWERUP_RANDOM; break;
        case POWERUP_SPEED: file = FILE_POWERUP_SPEED; break;
        default: return NULL;
    }
    std::string name = file + boost::lexical_cast<std::string>(uniqueID);

    return getNamedSoundInstance(name, file, attachTo);
}
*/
/*
OgreOggISound* SoundManager::getNamedSoundInstance(std::string name, std::string file, Ogre::SceneNode *attachTo, bool loop)
{
    if (!mInitOK) return NULL;

    OgreOggISound *soundObject = mSoundManager->createSound(name, file, false, loop, true, GameCore::mSceneMgr, false);
   
    if (attachTo != NULL)
    {
        attachTo->attachObject(soundObject);
        soundObject->setRolloffFactor(2.f);
        soundObject->setReferenceDistance(10.f);
    }

    // prebuffer + immediate
    return soundObject;
}
*/

void SoundManager::Play2DSound(Ogre::String name){
	
	m_sound_manager->getSound(name)->play();
};

void SoundManager::Stop2DSound(Ogre::String name){

	m_sound_manager->getSound(name)->stop();
};

void SoundManager::deleteSoundInstance(OgreOggISound* sound){
    if (!sound) return;

    mSoundDeletesPending->insert(mSoundDeletesPending->end(), sound);
}

void SoundManager::frameEvent(Ogre::Real timeSinceLastFrame){
    if (!mInitOK) return;

    processSoundDeletesPending();

    // In an ideal world the camera would be attached to a scene node so the listener would update
    // automatically. But that's not how we've done things, so manually set listener.
   
    // This ifdef is here to stop getLocalPlayer problems (server has no local player).
#ifdef COLLISION_DOMAIN_CLIENT
    // Attach ears to the car instead of the camera. Otherwise it just sounds weird!
    //GameCore::mGraphicsCore->mCamera->getPosition(),
    //GameCore::mGraphicsCore->mCamera->getOrientation()
    Ogre::Vector3 earsPosition = GameCore::mPlayerPool->getLocalPlayer()->getCar()->mBodyNode->getPosition();
    Ogre::Quaternion earsOrientation = GameCore::mPlayerPool->getLocalPlayer()->getCar()->mBodyNode->getOrientation();

    // if framerate is low, timeSinceLastFrame is larger.                   if framerate is high, timeSinceLastFrame is lower.
    // if framerate is low, unscaled Velocity will be higher than expected. if framerate is high, unscaled velocity will be lower than expected.

    mSoundManager->getListener()->setPosition(earsPosition);
    //mSoundManager->getListener()->setOrientation(earsOrientation);

    mSoundManager->getListener()->setVelocity(GameCore::mPlayerPool->getLocalPlayer()->getCar()->getLinearVelocity());

    // fire a frameevent for each car
    int numPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
    for (int i = 0; i < numPlayers; i++)
    {
        Player *player = GameCore::mPlayerPool->getPlayer(i);
        if (player == NULL) continue;
        Car *car = player->getCar();
        if (car == NULL) continue;
        car->updateAudioPitchFrameEvent();
    }
   
    // fire a frameevent for the local player
    Player *localPlayer = GameCore::mPlayerPool->getLocalPlayer();
    if (localPlayer != NULL && localPlayer->getCar() != NULL)
    {
        localPlayer->getCar()->updateAudioPitchFrameEvent();
    }
#endif
}

void SoundManager::processSoundDeletesPending(){
    // process deletes, BUT only for sounds which have finished playing
    std::list<OgreOggISound*>::iterator i = mSoundDeletesPending->begin();

    while (i != mSoundDeletesPending->end())
    {
        OgreOggISound* sound = NULL;
        sound = *i;
       
        if (sound && !sound->isPlaying())
        {
            m_sound_manager->destroySound(sound);
            mSoundDeletesPending->erase(i++); // alternatively, i = items.erase(i);
        }
        else
        {
            // leave it be for now, its still playing
            ++i;
        }
    }
}