#include "AudioComponents.h"
#include "..\Audio\SoundManager.h"
#include "ComponentMessenger.h"

///////////////////// SOUND 2D COMPONENT //////////////////////

void Sound2DComponent::Notify(int type, void* message){

	Ogre::String& sound_name = *static_cast<Ogre::String*>(message);

	if (type == MSG_SFX2D_PLAY)
	{ 
		m_sound_manager->Play2DSound(sound_name); 
	}
	else if (type == MSG_SFX2D_STOP)
	{ 
		m_sound_manager->Stop2DSound(sound_name); 
	}
};

void Sound2DComponent::Shut(){
	m_sound_manager = NULL;
};
	
void Sound2DComponent::Init(SoundManager* sound_manager){
	m_sound_manager = sound_manager;
};
	
void Sound2DComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_SFX2D_PLAY, this);
	m_messenger->Register(MSG_SFX2D_STOP, this);
};


///////////////////// SOUND 3D COMPONENT //////////////////////

void Sound3DComponent::Notify(int type, void* message){
	
	SoundData3D& sound_data = *static_cast<SoundData3D*>(message);

	if (type == MSG_SFX3D_PLAY)
	{ 
		m_sound_manager->Play3DSound(sound_data.m_name, sound_data.m_position);
	}
	else if (type == MSG_SFX3D_STOP)
	{ 
		m_sound_manager->Stop3DSound(sound_data.m_name);
	}
};
	
void Sound3DComponent::Shut(){
	m_sound_manager = NULL;
};
	
void Sound3DComponent::Init(SoundManager* sound_manager){
	m_sound_manager = sound_manager;
};
	
void Sound3DComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_SFX3D_PLAY, this);
	m_messenger->Register(MSG_SFX3D_STOP, this);
};

///////////////////// MUSIC 2D COMPONENT //////////////////////

void Music2DComponent::Notify(int type, void* message){
	//magic goes here
	//I put on my robe and wizard hat

	Ogre::String& sound_name = *static_cast<Ogre::String*>(message);

	if (type == MSG_MUSIC2D_PLAY)
	{
		m_sound_manager->Play2DMusic(sound_name);
	}
	else if (type == MSG_MUSIC2D_STOP)
	{
		m_sound_manager->Stop2DMusic(sound_name);
	}
};

void Music2DComponent::Shut(){
	m_sound_manager = NULL;
};

void Music2DComponent::Init(SoundManager* sound_manager){
	m_sound_manager = sound_manager;
};
	
void Music2DComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_MUSIC2D_PLAY, this);
	m_messenger->Register(MSG_MUSIC2D_STOP, this);
};


///////////////////// MUSIC 3D COMPONENT //////////////////////

void Music3DComponent::Notify(int type, void* message){
	//magic goes here
};
	
void Music3DComponent::Shut(){
	m_sound_manager = NULL;
};
	
void Music3DComponent::Init(SoundManager* sound_manager){
	m_sound_manager = sound_manager;
};
	
void Music3DComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_MUSIC3D_PLAY, this);
	m_messenger->Register(MSG_MUSIC3D_STOP, this);
};