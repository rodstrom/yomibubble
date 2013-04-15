#ifndef _CORE_H_
#define _CORE_H_

class MessageSystem;
class InputSystem;
class Game;
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
	Game*				m_game;
};

#endif // _CORE_H_