#ifndef _VN_MESSAGE_SYSTEM_H_
#define _VN_MESSAGE_SYSTEM_H_

#include <string>
#include <vector>
#include <map>

#include "MessageSystemPrereq.h"

class MessageSystem {
	class ICallback {
	public:
		virtual ~ICallback() {};
		virtual void operator()(IEvent*) = 0;
		virtual bool Compare(void *) = 0;
	};

	template <class T>
	class Callback : public ICallback {
		T *m_obj; 
		void(T::*m_mtd)(IEvent*);
	public:
		Callback(T *obj,void(T::*mtd)(IEvent*)):m_obj(obj),m_mtd(mtd) {};
		void operator()(IEvent *evt) {(*m_obj.*m_mtd)(evt);};
		bool Compare(void *obj) {return m_obj==obj;}; 
	};
public:
	MessageSystem();
	~MessageSystem();

	void Notify(IEvent *event);
	void Unregister(EEventType type,void *object);

	template <class T>
	void Register(EEventType type,T *object,void(T::*method)(IEvent*)) {
		m_message_lists[(int)type].push_back(new Callback<T>(object,method));
	};

private:
	void Shut();

private:
	typedef std::vector<ICallback*> CallbackList;
	CallbackList m_message_lists[EVT_MAX];
};

#endif // _VN_MESSAGE_SYSTEM_H_