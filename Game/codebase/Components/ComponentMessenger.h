#ifndef _N_COMPONENT_MESSENGER_H_
#define _N_COMPONENT_MESSENGER_H_

#include "ComponentsPrereq.h"

class ComponentMessenger{
public:
	ComponentMessenger(void);
	~ComponentMessenger(void);

	void Notify(int type, void* msg);	// Sends a message to all components listening to that type of message
	void Notify(int type, void* msg, const Ogre::String& id);	// Sends a message to components of a specific type and specific id
	typedef std::vector<std::vector<IComponentObserver*> > MessageList;
	void Register(int type, IComponentObserver* ob);
	void Unregister(IComponentObserver* ob);
	void Unregister(int type, IComponentObserver* ob);

private:
	inline void Shut();
	MessageList m_listeners;
};

#endif // _N_COMPONENT_MESSENGER_H_