#ifndef _N_FUNCTOR_H_
#define _N_FUNCTOR_H_


class IFunctor{
public:
	virtual ~IFunctor(void){}
	virtual void Call(void* data) = 0;
};


template <typename T>
class Functor : public IFunctor{
public:
	Functor(void){}
	Functor(T* ob, void (T::*callback)(void*)) : m_ob(ob), m_callback(callback){}
	virtual ~Functor(void){}

	virtual void Init(T* ob, void (T::*callback)(void*)){ m_ob = ob; m_callback = callback; }
	virtual void Call(void* data) { (*m_ob.*m_callback)(data); }

private:
	void (T::*m_callback)(void*);
	T* m_ob;
};



#endif // _N_FUNCTOR_H_