#ifndef _N_PLAYER_CONTROLLER_H_
#define _N_PLAYER_CONTROLLER_H_

#include "ComponentsPrereq.h"
#include "..\Audio\SoundManager.h" //this is only for testing

class InputManager;
class PlayerInputComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	PlayerInputComponent(void){ m_type = COMPONENT_PLAYER_INPUT; }
	virtual ~PlayerInputComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void Init(InputManager* input_manager);
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	InputManager* m_input_manager;

	Ogre::String m_walk_sound;
	Ogre::String m_def_music;
	SoundData3D sound_data;
};

#endif // _N_PLAYER_CONTROLLER_H_