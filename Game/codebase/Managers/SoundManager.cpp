#include "stdafx.h"
#include "SoundManager.h"

#include <iostream>

SoundManager::SoundManager(Ogre::SceneManager* scene_manager, Ogre::Camera* camera){

    m_sound_manager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    mSoundDeletesPending = new std::list<OgreOggISound*>;

	m_sound_manager->init();

	m_ear_node = scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0), Ogre::Quaternion::IDENTITY);
	m_ear_node->attachObject(m_sound_manager->getListener());
	m_scene_manager = scene_manager;
	m_sound_manager->setDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	testbajs = false; //temp, remove this later
}

SoundManager::~SoundManager(){
    delete mSoundDeletesPending;
	m_sound_manager->destroyAllSounds();
	m_scene_manager = NULL;
	m_sound_manager = NULL;
}

void SoundManager::LoadAudio(){
	m_sound_manager->createSound("Yomi_Walk", "SFX/Yomi/Yomi_Walk.wav", false, false, true);
	
	m_sound_manager->createSound("Dun_Dun", "SFX/Tott/Bristlestick_Zoom_mono.wav", false, false, true);

	m_sound_manager->createSound("Main_Theme", "Music/Day_area_theme_mono.wav", false, true, true);
	m_sound_manager->createSound("Menu_Theme", "Music/Menu_theme.wav", false, true, true);

	Init3D("Main_Theme", 10.0f, 10.0f, 150.0f);

	bool bajs = m_sound_manager->createEFXSlot();
	bajs;

//	m_sound_manager->createEFXEffect("Main_Theme", 
}

void SoundManager::Play2DMusic(Ogre::String name){
	m_sound_manager->getSound(name)->play();
	//if (m_sound_manager->getSound(name)->i
};

void SoundManager::Stop2DMusic(Ogre::String name){
	m_sound_manager->getSound(name)->play();
};

void SoundManager::CreatePlayList(){
	m_music_playlist.push_back(m_sound_manager->getSound("Main_Theme")->getName());
	m_music_playlist.push_back(m_sound_manager->getSound("Menu_Theme")->getName());
};

void SoundManager::Init3D(Ogre::String name, float reference_distance, float roll_off_factor, float max_distance){
	m_sound_manager->getSound(name)->setReferenceDistance(reference_distance);
	m_sound_manager->getSound(name)->setRolloffFactor(roll_off_factor);
	m_sound_manager->getSound(name)->setMaxDistance(max_distance);
};

void SoundManager::Play3DSound(Ogre::String name, Ogre::String node_name, bool attached){
	Ogre::SceneNode* node = NULL;
	if (node_name == "")
	{ node = m_scene_manager->getSceneNode(m_scene_nodes[0]); }
	else
	{ node = m_scene_manager->getSceneNode(node_name); }

	if (!attached)
	{ node->attachObject(m_sound_manager->getSound(name)); }
	m_sound_manager->getSound(name)->play();

	node = NULL;
}

void SoundManager::Stop3DSound(Ogre::String name){
	m_sound_manager->getSound(name)->stop();
};

void SoundManager::Play3DMusic(Ogre::String name, Ogre::String node_name, bool attached){
	Ogre::SceneNode* node = NULL;
	
	if (node_name == "")
	{ node = m_scene_manager->getSceneNode(m_scene_nodes[0]); }
	else
	{ node = m_scene_manager->getSceneNode(node_name); }

	if (!attached)
	{ node->attachObject(m_sound_manager->getSound(name)); }
	m_sound_manager->getSound(name)->play();

	node = NULL;
};
	
void SoundManager::Stop3DMusic(Ogre::String name){
	m_sound_manager->getSound(name)->stop();
};

void SoundManager::FadeOut(Ogre::String name){ //Does this even work? Try further.
	m_sound_manager->getSound(name)->startFade(true, 0.5f);
};
	
void SoundManager::FadeIn(Ogre::String name){
};
	
void SoundManager::ChangeVolume(Ogre::String name, float new_volume){ //This ranges from 0.0 to 1.0. Other values will normalize.
	m_sound_manager->getSound(name)->setVolume(new_volume);
};
	
void SoundManager::ChangePitch(Ogre::String name, float new_pitch){ //Around 0.0 - 5.0 is the useful range (the higher number, the more high-pitched, it stacks and multiplies)
	m_sound_manager->getSound(name)->setPitch(new_pitch);
};

void SoundManager::Update(Ogre::Camera* camera, Ogre::SceneManager* scene_manager, float dt){
	m_sound_manager->update(dt);

	m_ear_node->setPosition(scene_manager->getSceneNode(m_yomi_node_name)->getPosition());
	m_ear_node->setOrientation(scene_manager->getSceneNode(m_yomi_node_name)->getOrientation());

	//std::cout << "Ear node pos : " << m_ear_node->getPosition() << std::endl;
	processSoundDeletesPending();
}

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

void SoundManager::GetYomiNode(Ogre::String node_name){
	m_yomi_node_name = node_name;
};

void SoundManager::GetTottNode(Ogre::String node_name){
	m_scene_nodes.push_back(node_name);
};

SoundData2D SoundManager::Create2DData(Ogre::String name, bool fade_in, bool fade_out, bool change_pitch, bool change_volume, float pitch, float volume){
	SoundData2D data;
	data.m_name = name;
	data.m_fade_in = fade_in;
	data.m_fade_out = fade_out;
	data.m_change_pitch = change_pitch;
	data.m_change_volume = change_volume;
	data.m_pitch = pitch;
	data.m_volume = volume;

	return data;
};
	
SoundData3D SoundManager::Create3DData(Ogre::String name, Ogre::String node_name, bool attached, bool change_pitch, bool change_volume, float pitch, float volume){
	SoundData3D data;
	data.m_name = name;
	data.m_node_name = node_name;
	data.m_attached = attached;
	data.m_change_pitch = change_pitch;
	data.m_change_volume = change_volume;
	data.m_pitch = pitch;
	data.m_volume = volume;

	return data;
};

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