#ifndef _N_VISUAL_COMPONENTS_H_
#define _N_VISUAL_COMPONENTS_H_

#include "GameObjectPrereq.h"
#include "ComponentsPrereq.h"
#include "..\AnimationBlender.h"
#include "..\Artifex\Loader\ArtifexLoader.h"
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
#include <functional>
class GameObjectManager;
class SoundManager;

class NodeComponent : public Component, public IComponentObserver{
public:
	NodeComponent(void) : m_scene_manager(NULL), m_node(NULL), m_has_attached_entity(false){ m_type = COMPONENT_NODE; }
	virtual ~NodeComponent(void){}

	virtual void Init(const Ogre::Vector3& pos, Ogre::SceneManager* scene_manager);
	virtual void Init(Ogre::SceneNode* node, Ogre::SceneManager* scene_manager); 
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	Ogre::SceneNode* GetSceneNode() const { return m_node; }
	bool HasAttachedEntity() const { return m_has_attached_entity; }

protected:
	Ogre::SceneNode* m_node;
	Ogre::SceneManager* m_scene_manager;
	bool m_has_attached_entity;
};

class MeshRenderComponent : public Component, public IComponentObserver {
public:
	MeshRenderComponent(void) : m_entity(NULL), m_scene_manager(NULL){ m_type = COMPONENT_MESH_RENDER; }
	virtual ~MeshRenderComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id);
	//Ogre::SceneNode* GetSceneNode() const { return m_node; }
	Ogre::Entity* GetEntity() const { return m_entity; }
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

protected:
	//Ogre::SceneNode*	m_node;
	Ogre::Entity*		m_entity;
	Ogre::SceneManager* m_scene_manager;
};

class ChildSceneNodeComponent : public Component, public IComponentObserver{
public:
	ChildSceneNodeComponent(void){ m_type = COMPONENT_CHILD_NODE; }
	virtual ~ChildSceneNodeComponent(void){}
	virtual void Notify(int type, void* msg);
	virtual void Init(const Ogre::Vector3& position, const Ogre::String& id, Ogre::SceneNode* parent);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	Ogre::SceneNode* GetNode() const { return m_node; }
	const Ogre::String& GetId() const { return m_id; }

protected:
	Ogre::SceneNode* m_node;
	Ogre::String m_id;
};

class AnimationComponent : public MeshRenderComponent, public IComponentUpdateable{
public:
	AnimationComponent(void) : m_callback(NULL){ m_type = COMPONENT_ANIMATION; m_update = true; }
	virtual ~AnimationComponent(void){}
	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, bool remove_weights = false);
	virtual void Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id, bool remove_weights = false);
	virtual void AddAnimationState(const Ogre::String& anim_name, bool loop = true);	// add an animation state and initialize the animation with the animation name
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);

	
	Ogre::String m_current_animation;
protected:
	void PlayQueued();
	void FixPlayerWeights();	// Ugly hack for the player to fix animation weights because we didn't do enough research in the beginning
	void RemoveWeights(std::vector<std::string>& list, Ogre::Animation* anim);
	std::vector<AnimationData>	m_animation_states;
	std::deque<AnimationMsg> m_queue;
	std::function<void()> m_callback;
	AnimationBlender* m_animation_blender;
};

class Overlay2DComponent : public Component, public IComponentObserver {
public:
	Overlay2DComponent(void){}
	virtual ~Overlay2DComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(const Ogre::String& p_overlay_name, const Ogre::String& p_cont_name);

protected:
	Ogre::Overlay*					m_overlay;
	Ogre::String					m_id;
	Ogre::String					m_cont_name;
	Ogre::OverlayManager*			m_overlay_manager;
};

class InputManager;
class OverlayCollisionCallbackComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	enum OverlayCollisionState{
		OCS_DEFAULT = 0,
		OCS_COLLISION
	};

	OverlayCollisionCallbackComponent(void){ m_update = true; }
	virtual ~OverlayCollisionCallbackComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Update(float dt);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(InputManager* p_input_manager, Ogre::Viewport* p_view_port);

protected:
	InputManager*			m_input_manager;
	Ogre::Viewport*			m_view_port;
	OverlayCollisionState	m_ocs;
	bool					m_show_overlay;
};

class PlayerStaffComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	PlayerStaffComponent(void){ m_type = COMPONENT_PLAYER_STAFF; }
	virtual ~PlayerStaffComponent(void) {}
	virtual void Notify(int type, void* msg);
	virtual void Update(float dt);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(Ogre::SceneManager* scene_manager, Ogre::Entity* player_entity);
protected:
	Ogre::SceneManager* m_scene_manager;
	Ogre::Entity* m_entity;
	Ogre::SceneNode* m_node;
	Ogre::Entity* m_player_entity;
};

class Overlay2DAnimatedComponent : public IComponentUpdateable, public Overlay2DComponent{
public:
	Overlay2DAnimatedComponent(void){ m_update = true; }
	virtual ~Overlay2DAnimatedComponent(void){}
	virtual void Notify(int type, void*message);
	virtual void Update(float dt);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(const Ogre::String& p_overlay_name, const Ogre::String& p_material_name_hover, const Ogre::String& p_material_name_exit, const Ogre::String& p_cont_name);

protected:
	Ogre::String			m_material_name_hover;
	Ogre::String			m_material_name_exit;
	Ogre::String			m_overlay_name;
	Ogre::String			m_cont_name;
	Ogre::String			m_material_start_button;
};

class CountableResourceGUI : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	CountableResourceGUI(void) : m_total_number(0), m_current_number(0){}
	virtual ~CountableResourceGUI(void){}
	virtual void Notify(int type, void*message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(const Ogre::String& level_id);
	virtual void Update(float dt);

protected:
	int						m_total_number;
	int						m_current_number;
	
	float					m_pickup_timer;
	float					m_timer_counter;

	bool					m_can_pick_up;

	Ogre::String			m_material_name_active;
	Ogre::String			m_material_name_inactive;
	Ogre::String			m_level;

	Ogre::Overlay*			m_overlay;
	std::vector<Ogre::OverlayContainer*> m_elements;
};

class OverlayCallbackComponent : public Component, public IComponentObserver{
public:
	OverlayCallbackComponent(void){}
	virtual ~OverlayCallbackComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(std::function<void()> func);
protected:
	std::function<void()>		m_func;
};

class ParticleComponent : public Component, public IComponentObserver{
public:
	ParticleComponent(void){}
	virtual ~ParticleComponent(void){}
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(Ogre::SceneManager* p_scene_manager, const Ogre::String& p_particle_name, const Ogre::String& p_particle_file_name, const Ogre::Vector3& position, Ogre::SceneNode* node);
	Ogre::ParticleSystem* GetParticleSystem() const { return m_particle_system; }
protected:
	Ogre::SceneManager*			m_scene_manager;
	Ogre::ParticleSystem*		m_particle_system;
};

class PhysicsEngine;
class TerrainComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	TerrainComponent(void) : m_scene_manager(NULL), m_physics_engine(NULL), m_artifex_loader(NULL), m_terrain_shape(NULL), 
		m_terrain_body(NULL), m_terrain_motion_state(NULL), m_data_converter(NULL){ m_type = COMPONENT_TERRAIN; }
	virtual ~TerrainComponent(void){}

	virtual void Update(float dt);
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(Ogre::SceneManager* scene_manager, PhysicsEngine* physics_engine, GameObjectManager* game_object_manager, SoundManager* sound_manager, const Ogre::String& filename);

	btRigidBody* GetRigidBody() { return m_terrain_body; }

	ArtifexLoader* TerrainComponent::GetArtifex();

protected:
	float*							m_data_converter;
	Ogre::SceneManager*				m_scene_manager;
	PhysicsEngine*					m_physics_engine;
	ArtifexLoader*					m_artifex_loader;
	btHeightfieldTerrainShape*		m_terrain_shape;
	btRigidBody*					m_terrain_body;
	btDefaultMotionState*			m_terrain_motion_state;
	CollisionDef					m_collision_def;
};

class SpeechBubbleComponent : public Component, public IComponentObserver, public IComponentUpdateable {
public:
	SpeechBubbleComponent(void){ m_type = COMPONENT_SPEECH_BUBBLE; };
	virtual ~SpeechBubbleComponent(void){};

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(Ogre::SceneNode* node, SceneManager* scene_manager, GameObject* tott);
	virtual void Update(float dt);

	bool m_player_collide;

	void ScaleUp();
	void ScaleDown();

	GameObject* m_tott;

	bool m_given_leaf;

protected:
	MeshRenderComponent* m_mesh;
	Ogre::SceneNode* m_node;
	float m_current_scale;
};

class TutorialGraphicsComponent : public Component, public IComponentObserver, public IComponentUpdateable {
public:
	TutorialGraphicsComponent(void){}
	virtual ~TutorialGraphicsComponent(void){}
	
	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	void Init(const Ogre::String& id, const Ogre::String& level);
	virtual void Update(float dt);

protected:
	float m_timer;
	float m_timer_counter;
	bool m_first_pic;

	Ogre::String m_pic_one;
	Ogre::String m_pic_two;
	Ogre::String m_pic_one_sec;
	Ogre::String m_pic_two_sec;
	Ogre::String m_level;

	Ogre::Overlay* m_overlay;
	Ogre::Overlay* m_overlay_sec;

};

class DestroyCallbackComponent : public Component, public IComponentObserver{
public:
	DestroyCallbackComponent(void) : m_callback(NULL){}
	~DestroyCallbackComponent(void){}
	virtual void Notify(int type, void* message){}
	virtual void Shut(){
		if (m_callback){
			m_callback();
		}
	}
	virtual void SetMessenger(ComponentMessenger* messenger){ m_messenger = messenger; }
	virtual void Init(std::function<void()> callback) { m_callback = callback; }
protected:
	std::function<void()> m_callback;
};

class GateControllerComponent : public Component, public IComponentObserver, public IComponentUpdateable{
public:
	GateControllerComponent(void) : m_proceed(false), m_rotate(false), m_right_gate_node(NULL), m_left_gate_node(NULL){}
	~GateControllerComponent(void){}

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);
	virtual void Init(MessageSystem* message_system, int leaves);
	void LeafPickup(struct IEvent*);

protected:
	void OpenGate();
	class MessageSystem* m_message_system;
	bool m_can_decrease;
	bool m_proceed;	// if true the player can change level
	float m_timer;
	int m_counter;	// amount of leafs
	bool m_rotate;
	Ogre::SceneNode* m_left_gate_node;
	Ogre::SceneNode* m_right_gate_node;
};

class RotationComponent : public Component, public IComponentUpdateable, public IComponentObserver{
public:
	RotationComponent(void) : m_node(NULL){}
	~RotationComponent(void){}

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Update(float dt);
	virtual void Init(Ogre::SceneNode* node, float rotation_speed);

protected:
	Ogre::SceneNode* m_node;
	float m_rotation_speed;
};

class StringComponent : public Component, public IComponentObserver{
public:
	StringComponent(void){ m_type = COMPONENT_STRING; }
	virtual ~StringComponent(void){}

	virtual void Notify(int type, void* message);
	virtual void Shut();
	virtual void SetMessenger(ComponentMessenger* messenger);
	virtual void Init(const Ogre::String& line){ m_string = line; }
	const Ogre::String& GetString() const { return m_string; }
protected:
	Ogre::String m_string;
};

#endif // _N_VISUAL_COMPONENTS_H_