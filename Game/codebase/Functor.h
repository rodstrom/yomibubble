#ifndef _N_FUNCTOR_H_
#define _N_FUNCTOR_H_

template <typename T>
class Functor{
public:
	Functor(void){}
	Functor(T* ob, void (T::*callback)(void*)) : m_ob(ob), m_callback(callback){}
	virtual ~Functor(void){}

	void Init(T* ob, void (T::*callback)(void*)){ m_ob = ob; m_callback = callback; }
	void Call(void* data) { (*m_ob.*m_callback)(data); }

private:
	void (T::*m_callback)(void*);
	T* m_ob;
};


#endif // _N_FUNCTOR_H_