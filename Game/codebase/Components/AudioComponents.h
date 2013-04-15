#ifndef AUDIO_COMPONENTS_H
#define AUDIO_COMPONENTS_H

#include "ComponentsPrereq.h"
#include <string>
class SoundManager;

class Sound2DComponent : public Component, public IComponentObserver{
public:
	Sound2DComponent(void){ m_type = COMPONENT_AUDIO; };
	virtual ~Sound2DComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

private:
	SoundManager* m_sound_manager;
};


class Sound3DComponent : public Component, public IComponentObserver{
public:
	Sound3DComponent(void){ m_type = COMPONENT_AUDIO; };
	virtual ~Sound3DComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

private:
	SoundManager* m_sound_manager;
};


class Music2DComponent : public Component, public IComponentObserver{
public:
	Music2DComponent(void){ m_type = COMPONENT_AUDIO; };
	virtual ~Music2DComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

private:
	SoundManager* m_sound_manager;
};


class Music3DComponent : public Component, public IComponentObserver{
public:
	Music3DComponent(void){ m_type = COMPONENT_AUDIO; };
	virtual ~Music3DComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(SoundManager* sound_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

private:
	SoundManager* m_sound_manager;
};



#endif //AUDIO_COMPONENTS_H