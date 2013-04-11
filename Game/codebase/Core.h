#ifndef _CORE_H_
#define _CORE_H_

class MessageSystem;
class InputSystem;
class BubbleAdventure;
class SoundManager;

class Core
{
public:
	Core(void);
	~Core(void);

	bool Init();
	void Run();
	void Shut();
private:
	Ogre::Root*			m_root;
	Ogre::RenderWindow* m_render_window;
	InputSystem*		m_input_system;
	MessageSystem*		m_message_system;
	BubbleAdventure*	m_bubble_adventure;
	SoundManager*		m_sound_manager;
	Ogre::SceneManager* m_single_scene_mgr;
};

#endif // _CORE_H_