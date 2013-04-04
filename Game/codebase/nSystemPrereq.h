#ifndef _NOSTROMO_SYSTEM_PREREQ_H_
#define _NOSTROMO_SYSTEM_PREREQ_H_

class MessageSystem;
struct Systems {
	MessageSystem* m_message_system;
	Ogre::RenderWindow* m_render_window;
	Ogre::SceneManager* m_scene_manager;
};

#endif // _NOSTROMO_SYSTEM_PREREQ_H_