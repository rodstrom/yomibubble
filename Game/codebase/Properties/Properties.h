#ifndef _NOSTROMO_PROPERTIES_H_
#define _NOSTROMO_PROPERTIES_H_

class Entity;
class EntityController{
public:
	EntityController(void) : m_owner(nullptr){}
	virtual ~EntityController(void){}
	virtual void Init(Entity* p_entity) { m_owner = p_entity; }
	virtual void Update(float deltatime) = 0;
protected:
	Entity* m_owner;
};

class EntityPhysics {
public:
	EntityPhysics(void) : m_owner(nullptr){}
	virtual ~EntityPhysics(void){}
	virtual void Init(Entity* p_entity) { m_owner = p_entity; }
	virtual void Update(float deltatime) = 0;
protected:
	Entity* m_owner;
};

class EntityVisual {
public:
	EntityVisual(void) : m_owner(nullptr){}
	virtual ~EntityVisual(void){}
	virtual void Init(Entity* p_entity) { m_owner = p_entity; }
	virtual void Update(float deltatime) = 0;
protected:
	Entity* m_owner;
};

#endif // _NOSTROMO_PROPERTIES_H_