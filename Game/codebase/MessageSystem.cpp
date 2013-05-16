#include "stdafx.h"
#include "MessageSystem.h"
MessageSystem::MessageSystem() {
};

MessageSystem::~MessageSystem() {
	Shut();
};

void MessageSystem::Notify(IEvent *event) {
	/**
	TODO: Translate all OS messages into engine specific messages
		  Instead of notifying all listeners direct, store the 
		  events in a local buffer, then pump them to all listeners
		  (Change notify-all into a chain of responsability pattern.)
	*/
	if(m_message_lists[(int)event->m_type].empty()) {return;};
	CallbackList::iterator it;
	it=m_message_lists[(int)event->m_type].begin();
	CallbackList::iterator it_end;
	it_end=m_message_lists[(int)event->m_type].end();
	for( ;it!=it_end;++it) {
		(*(*it))(event);
	};
};

void MessageSystem::Unregister(EEventType type,void *object) {
	if(m_message_lists[(int)type].empty()) {return;};
	CallbackList::iterator it_end;
	it_end=m_message_lists[(int)type].end();
	CallbackList::iterator it;
	for(it=m_message_lists[(int)type].begin();it!=it_end;++it) {
		if((*it)->Compare(object)) {
			delete (*it);
			m_message_lists[(int)type].erase(it);
			break;
		};
	};
};

void MessageSystem::Shut() {
	for(int i=0;i<EVT_MAX;i++) {
		if(m_message_lists[i].empty()) {continue;};
		CallbackList::iterator it=m_message_lists[i].begin();
		CallbackList::iterator ie=m_message_lists[i].end();
		while(it!=ie) {
			delete (*it);
			++it;
		};
		m_message_lists[i].clear();
	};
};


// Function that converts OS specific virtual keycodes 
// to internal keycodes
EKeyCode Convert(int vk) {
	switch(vk) {
		case VK_BACK: return  KC_Backspace;
		case VK_TAB: return  KC_Tab;
		case VK_RETURN: return  KC_Return;
		case VK_PAUSE: return  KC_Pause;
		case VK_ESCAPE: return  KC_Escape;
		case VK_SPACE: return  KC_Space;
		case VK_PRIOR: return  KC_Pageup;
		case VK_NEXT: return  KC_Pagedown;
		case VK_END: return  KC_End;
		case VK_HOME: return  KC_Home;
		case VK_LEFT: return  KC_Left;
		case VK_UP: return  KC_Up;
		case VK_RIGHT: return  KC_Right;
		case VK_DOWN: return  KC_Down;
		case VK_SNAPSHOT: return  KC_Printscreen;
		case VK_INSERT: return  KC_Insert;
		case VK_DELETE: return  KC_Delete;
		case VK_SHIFT: return  KC_Shift;
		case VK_CONTROL: return  KC_Control;
		case VK_MENU: return KC_Alt;
	
		case '0': return  KC_0;
		case '1': return  KC_1;
		case '2': return  KC_2;
		case '3': return  KC_3;
		case '4': return  KC_4;
		case '5': return  KC_5;
		case '6': return  KC_6;
		case '7': return  KC_7;
		case '8': return  KC_8;
		case '9': return  KC_9;

		case 'A': return  KC_a;
		case 'B': return  KC_b;
		case 'C': return  KC_c;
		case 'D': return  KC_d;
		case 'E': return  KC_e;
		case 'F': return  KC_f;
		case 'G': return  KC_g;
		case 'H': return  KC_h;
		case 'I': return  KC_i;
		case 'J': return  KC_j;
		case 'K': return  KC_k;
		case 'L': return  KC_l;
		case 'M': return  KC_m;
		case 'N': return  KC_n;
		case 'O': return  KC_o;
		case 'P': return  KC_p;
		case 'Q': return  KC_q;
		case 'R': return  KC_r;
		case 'S': return  KC_s;
		case 'T': return  KC_t;
		case 'U': return  KC_u;
		case 'V': return  KC_v;
		case 'W': return  KC_w;
		case 'X': return  KC_x;
		case 'Y': return  KC_y;
		case 'Z': return  KC_z;

		case VK_LWIN: return  KC_LeftWin;
		case VK_RWIN: return  KC_RightWin;
		case VK_APPS: return  KC_Apps;
		case VK_NUMPAD0: return  KC_Numpad0;
		case VK_NUMPAD1: return  KC_Numpad1;
		case VK_NUMPAD2: return  KC_Numpad2;
		case VK_NUMPAD3: return  KC_Numpad3;
		case VK_NUMPAD4: return  KC_Numpad4;
		case VK_NUMPAD5: return  KC_Numpad5;
		case VK_NUMPAD6: return  KC_Numpad6;
		case VK_NUMPAD7: return  KC_Numpad7;
		case VK_NUMPAD8: return  KC_Numpad8;
		case VK_NUMPAD9: return  KC_Numpad9;
		case VK_MULTIPLY: return  KC_NumpadMultiply;
		case VK_ADD: return  KC_NumpadAdd;
		case VK_SEPARATOR: return  KC_NumpadSeparator;
		case VK_SUBTRACT: return  KC_NumpadSubtract;
		case VK_DECIMAL: return  KC_NumpadDecimal;
		case VK_DIVIDE: return  KC_NumpadDivide;

		case VK_F1: return  KC_F1;
		case VK_F2: return  KC_F2;
		case VK_F3: return  KC_F3;
		case VK_F4: return  KC_F4;
		case VK_F5: return  KC_F5;
		case VK_F6: return  KC_F6;
		case VK_F7: return  KC_F7;
		case VK_F8: return  KC_F8;
		case VK_F9: return  KC_F9;
		case VK_F10: return  KC_F10;
		case VK_F11: return  KC_F11;
		case VK_F12: return  KC_F12;
		case VK_F13: return  KC_F13;
		case VK_F14: return  KC_F14;
		case VK_F15: return  KC_F15;

		case VK_NUMLOCK: return  KC_Numlock;
		case VK_SCROLL: return  KC_ScrollLock;

		case VK_LSHIFT: return  KC_LeftShift;
		case VK_RSHIFT: return  KC_RightShift;
		case VK_LCONTROL: return  KC_LeftControl;
		case VK_RCONTROL: return  KC_RightControl;
		case VK_LMENU: return  KC_LeftAlt;
		case VK_RMENU: return  KC_RightAlt;
	};
	return KC_Invalid;
};