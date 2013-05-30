#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "OgreOggSound.h"
#include "OgreOggSoundManager.h"

#include <map>
#include <vector>

using namespace OgreOggSound;

struct SoundData3D{
	Ogre::String m_name;
	Ogre::String m_node_name;

	bool m_attached;
	bool m_change_pitch;
	bool m_change_volume;

	float m_pitch;
	float m_volume;
};

struct SoundData2D{
	Ogre::String m_name;

	bool m_fade_in;
	bool m_fade_out;
	bool m_change_pitch;
	bool m_change_volume;

	float m_fade_time;
	float m_fade_amount;

	float m_pitch;
	float m_volume;
};

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	void Init(Ogre::SceneManager* scene_manager, bool play_state = false);
	void Exit();

    void deleteSoundInstance(OgreOggISound* sound);
   
	void LoadAudio();

	SoundData2D Create2DData(Ogre::String name, bool fade_in, bool fade_out, bool change_pitch, bool change_volume, float pitch, float volume);
	SoundData3D Create3DData(Ogre::String name, Ogre::String node_name, bool attached, bool change_pitch, bool change_volume, float pitch, float volume);

	void Play2DSound(Ogre::String name);
	void Stop2DSound(Ogre::String name);
	void Play3DSound(Ogre::String name, Ogre::String node_name, bool attached);
	void Stop3DSound(Ogre::String name);

	void FadeOut(Ogre::String name, float duration);
	void FadeIn(Ogre::String name, float duration);
	void ChangeVolume(Ogre::String name, float volume);
	void ChangePitch(Ogre::String name, float pitch);
	//void AddEffect(Ogre::String name, Ogre::String effect); //I might want an enumarator for the effect //eeeeh, check out how they work

	void CreatePlayList();
	void Init3D(Ogre::String name, float reference_distance, float roll_off_factor, float max_distance);

	void Play2DMusic(Ogre::String name);
	void Stop2DMusic(Ogre::String name);
	void Play3DMusic(Ogre::String name, Ogre::String node_name, bool attached);
	void Stop3DMusic(Ogre::String name);

	OgreOggSoundManager* GetSoundManager() { return m_sound_manager; };
	void GetYomiNode(Ogre::String node_name);
	void GetTottNode(Ogre::String node_name);
	void Update(Ogre::SceneManager* scene_manager, float dt); 

	std::vector<Ogre::String> m_scene_nodes;
	std::vector<Ogre::String> m_music_playlist;

	Ogre::String m_yomi_node_name;

private:
    Ogre::SceneNode* m_ear_node;
	Ogre::SceneManager* m_scene_manager;

	void processSoundDeletesPending();

    OgreOggSoundManager* m_sound_manager;
    std::list<OgreOggISound*> *mSoundDeletesPending;
};

#endif //SOUND_MANAGER_H