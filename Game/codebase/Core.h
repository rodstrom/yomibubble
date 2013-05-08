#ifndef _CORE_H_
#define _CORE_H_

class MessageSystem;
class InputSystem;
class Game;
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
	SoundManager* 		m_sound_manager;
	Game*				m_game;
};

#endif // _CORE_H_