#ifndef _N_PLAYER_CONTROLLER_H_
#define _N_PLAYER_CONTROLLER_H_

#include "ComponentsPrereq.h"

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
};

#endif // _N_PLAYER_CONTROLLER_H_