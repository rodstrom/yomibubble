#include "stdafx.h"
#include "VisualComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "..\InputPrereq.h"
#include "..\PhysicsEngine.h"
#include "GameObject.h"
#include "..\Managers\GameObjectManager.h"
#include "..\Managers\SoundManager.h"
#include "..\PhysicsPrereq.h"

void NodeComponent::Init(const Ogre::Vector3& pos, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->setPosition(pos);
}

void NodeComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_NODE_GET_NODE:
		*static_cast<Ogre::SceneNode**>(msg) = m_node;
		break;
	case MSG_INCREASE_SCALE_BY_VALUE:
		{
			Ogre::Vector3 scale = m_node->getScale();
			scale += *static_cast<Ogre::Vector3*>(msg);
			m_node->setScale(scale);
		}
		break;
	case MSG_SET_OBJECT_POSITION:
		m_node->setPosition(*static_cast<Ogre::Vector3*>(msg));
		break;
	case MSG_NODE_ATTACH_ENTITY:
		{
			if (!m_has_attached_entity){
				m_node->attachObject(*static_cast<Ogre::Entity**>(msg));
				m_has_attached_entity = true;
			}
		}
		break;
	default:
		break;
	}
}

void NodeComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_NODE_GET_NODE, this);
		m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
		m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
		m_messenger->Unregister(MSG_NODE_ATTACH_ENTITY, this);
	}
	if (m_node){
		m_scene_manager->destroySceneNode(m_node);
		m_node = NULL;
	}
}

void NodeComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_NODE_GET_NODE, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
	m_messenger->Register(MSG_NODE_ATTACH_ENTITY, this);
}

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_messenger->Notify(MSG_NODE_ATTACH_ENTITY, &m_entity);
}

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_messenger->Notify(MSG_NODE_ATTACH_ENTITY, &m_entity, node_id);
}

void MeshRenderComponent::Notify(int type, void* msg){

}

void MeshRenderComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void MeshRenderComponent::Shut(){
	if (m_entity != NULL){
		m_scene_manager->destroyEntity(m_entity);
		m_entity = NULL;
	}
}

void AnimationComponent::SetMessenger(ComponentMessenger* messenger){
	MeshRenderComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_ANIMATION_PLAY, this);
	m_messenger->Register(MSG_ANIMATION_PAUSE, this);
	m_messenger->Register(MSG_ANIMATION_BLEND, this);
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	MeshRenderComponent::Init(filename, scene_manager);
	
	m_animation_blender = new AnimationBlender(GetEntity());
	/*
	m_animation_blender->init("Idle");
	m_animation_blender->init("Run");
	m_animation_blender->init("Walk");
	m_animation_blender->init("Jump");
	*/
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id){
	MeshRenderComponent::Init(filename, scene_manager, node_id);
	
	m_animation_blender = new AnimationBlender(GetEntity());
	/*
	m_animation_blender->init("Idle");
	m_animation_blender->init("Run");
	m_animation_blender->init("Walk");
	m_animation_blender->init("Jump");
	*/
	
}

void AnimationComponent::AddAnimationStates(unsigned int value){
	for (unsigned int i = 0; i < value; i++){
		Ogre::AnimationState* a = NULL;
		m_animation_states.push_back(a);
	}
}

void AnimationComponent::Update(float dt){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		if (m_animation_states[i] != NULL){
			if (m_animation_states[i]->getEnabled()){
				m_animation_states[i]->addTime(dt);
			}
		}
	}
	//m_animation_blender->addTime(dt);
}

void AnimationComponent::Notify(int type, void* msg){
	MeshRenderComponent::Notify(type, msg);
	AnimationMsg* anim_msg = static_cast<AnimationMsg*>(msg);

	switch (type){
	case MSG_ANIMATION_PLAY:
		{
			if (anim_msg->blend){
				m_animation_states[0] = m_entity->getAnimationState(anim_msg->bottom_anim);
				if (m_animation_states[0] != NULL){
					m_animation_states[0]->setEnabled(true);
					m_animation_states[0]->setLoop(true);
				}

				m_animation_states[1] = m_entity->getAnimationState(anim_msg->top_anim);
				if (m_animation_states[1] != NULL){
					m_animation_states[1]->setEnabled(true);
					m_animation_states[1]->setLoop(true);
				}
			}
			else{
				m_animation_states[0] = m_entity->getAnimationState(anim_msg->id);
				if (m_animation_states[0] != NULL){
					m_animation_states[0]->setEnabled(true);
					m_animation_states[0]->setLoop(true);
					//m_animation_blender->init("Walk");
					//m_animation_blender->blend("Idle", AnimationBlender::BlendWhileAnimating, 0.2, true);
				}
			}
		}
		break;
	case MSG_ANIMATION_PAUSE:
		{
			if (m_animation_states[0] != NULL){
					m_animation_states[0]->setEnabled(false);
					m_animation_states[0]->setLoop(false);
				}

			if (anim_msg->blend){
				if (m_animation_states[1] != NULL){
					m_animation_states[1]->setEnabled(false);
					m_animation_states[1]->setLoop(false);
				}
			}
		}
		break;
	default:
		break;
	}
}

void AnimationComponent::Shut(){
	if (!m_animation_states.empty()){
		for (unsigned int i = 0; i < m_animation_states.size(); i++){
			if (m_animation_states[i] != NULL){
				m_animation_states[i]->setEnabled(false);
			}
		}
	}
	m_animation_states.clear();
	m_messenger->Unregister(MSG_ANIMATION_PLAY, this);
	m_messenger->Unregister(MSG_ANIMATION_PAUSE, this);
	m_messenger->Unregister(MSG_ANIMATION_BLEND, this);
	MeshRenderComponent::Shut();
}

void ChildSceneNodeComponent::Notify(int type, void* msg){
	switch (type)
	{
	case MSG_CHILD_NODE_GET_NODE:
		*static_cast<Ogre::SceneNode**>(msg) = m_node;
		break;
	default:
		break;
	}
}

void ChildSceneNodeComponent::Init(const Ogre::Vector3& position, const Ogre::String& id, Ogre::SceneNode* parent){
	m_id = id;
	m_node = parent->createChildSceneNode();
	m_node->setPosition(position);
}

void ChildSceneNodeComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_CHILD_NODE_GET_NODE, this);
	}
}

void ChildSceneNodeComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CHILD_NODE_GET_NODE, this);
}

void Overlay2DComponent::Init(const Ogre::String& p_overlay_name, const Ogre::String& p_cont_name){
 	m_overlay_manager = Ogre::OverlayManager::getSingletonPtr();
	m_id = p_overlay_name;
	m_cont_name = p_cont_name;

	m_overlay = m_overlay_manager->getByName(m_id);
	m_messenger->Notify(MSG_OVERLAY_SHOW, NULL);
	//m_overlay->show();
}

void Overlay2DComponent::Shut()
{
	m_overlay->hide();
	if (m_messenger){
		m_messenger->Unregister(MSG_GET_2D_OVERLAY_CONTAINER, this);
		m_messenger->Unregister(MSG_OVERLAY_SHOW, this);
		m_messenger->Unregister(MSG_OVERLAY_HIDE, this);
	}
	//m_overlay_manager->destroyAllOverlayElements();
	//m_overlay_manager->destroyAll();
}

void Overlay2DComponent::Notify(int type, void* msg){
	switch(type){
	case MSG_GET_2D_OVERLAY_CONTAINER:
		*static_cast<Ogre::OverlayContainer**>(msg) = m_overlay->getChild(m_cont_name);	
		break;
	case MSG_OVERLAY_SHOW:
		m_overlay->show();
		break;

	case MSG_OVERLAY_HIDE:
		m_overlay->hide();
		break;
	default:
		break;
	}
}

void Overlay2DComponent::SetMessenger(ComponentMessenger* messenger) {
	m_messenger = messenger;
	m_messenger->Register(MSG_GET_2D_OVERLAY_CONTAINER, this);
	m_messenger->Register(MSG_OVERLAY_SHOW, this);
	m_messenger->Register(MSG_OVERLAY_HIDE, this);
	
}

void OverlayCollisionCallbackComponent::Init(InputManager* p_input_manager, Ogre::Viewport* p_view_port){
	m_input_manager = p_input_manager;
	m_view_port = p_view_port;
	m_ocs = OCS_DEFAULT;
	m_show_overlay = true;
}

void OverlayCollisionCallbackComponent::Update(float dt){
	Ogre::OverlayContainer* overlay_container = NULL;

	m_messenger->Notify(MSG_GET_2D_OVERLAY_CONTAINER, &overlay_container);
	if(overlay_container){

		float mouseX = m_input_manager->GetMouseState().X.abs / (float)m_view_port->getActualWidth();
		float mouseY = m_input_manager->GetMouseState().Y.abs / (float)m_view_port->getActualHeight(); 
		float x = overlay_container->getLeft();
		float y = overlay_container->getTop();
		float w = overlay_container->getWidth();
		float h = overlay_container->getHeight();

		if (m_ocs == OCS_DEFAULT){
			if (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h){
				m_messenger->Notify(MSG_OVERLAY_HOVER_ENTER, NULL);
			
				m_ocs = OCS_COLLISION;
			}
		}
		else if (m_ocs == OCS_COLLISION){
			if(mouseX <= x || mouseX >= x + w || mouseY <= y || mouseY >= y + h){
				m_messenger->Notify(MSG_OVERLAY_HOVER_EXIT, NULL);
				m_ocs = OCS_DEFAULT;
			}
		}
	}
	if(m_ocs == OCS_COLLISION && m_input_manager->IsButtonPressed(BTN_LEFT_MOUSE))
	{
		m_messenger->Notify(MSG_OVERLAY_CALLBACK, NULL);
		m_messenger->Notify(MSG_OVERLAY_HIDE, NULL);
	}

	/*if(m_input_manager->IsButtonPressed(BTN_START)){
		m_show_overlay = !m_show_overlay;
		m_messenger->Notify(MSG_OVERLAY_SHOW, NULL);
	}
	if(!m_show_overlay)
		m_messenger->Notify(MSG_OVERLAY_HIDE, NULL);*/
}


void OverlayCollisionCallbackComponent::Shut(){
}

void OverlayCollisionCallbackComponent::Notify(int type, void* msg){
}

void OverlayCollisionCallbackComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;

}

void Overlay2DAnimatedComponent::Init(const Ogre::String& p_overlay_name, const Ogre::String& p_material_name_hover, const Ogre::String& p_material_name_exit, const Ogre::String& p_cont_name){
	m_overlay_name = p_overlay_name;
	m_material_name_hover = p_material_name_hover;
	m_material_name_exit = p_material_name_exit;
	m_cont_name = p_cont_name;
	Overlay2DComponent::Init(p_overlay_name, p_cont_name);	
}

void Overlay2DAnimatedComponent::Update(float dt){
}

void Overlay2DAnimatedComponent::Notify(int type, void*msg){
	Overlay2DComponent::Notify(type, msg);
	switch(type){
	case MSG_OVERLAY_HOVER_ENTER:
		m_overlay->getChild(m_cont_name)->setMaterialName(m_material_name_hover);
		break;
	case MSG_OVERLAY_HOVER_EXIT:
		m_overlay->getChild(m_cont_name)->setMaterialName(m_material_name_exit);
		break;
	default:
		break;
	}
}

void Overlay2DAnimatedComponent::SetMessenger(ComponentMessenger* messenger){
	Overlay2DComponent::SetMessenger(messenger);

	m_messenger->Register(MSG_OVERLAY_HOVER_ENTER, this);
	m_messenger->Register(MSG_OVERLAY_HOVER_EXIT, this);
}

void Overlay2DAnimatedComponent::Shut(){
	m_messenger->Unregister(MSG_OVERLAY_HOVER_ENTER, this);
	m_messenger->Unregister(MSG_OVERLAY_HOVER_EXIT, this);
	Overlay2DComponent::Shut();
}

void OverlayCallbackComponent::Init(std::function<void()> func){
	m_func = func;
}

void OverlayCallbackComponent::Notify(int type, void* message){
	switch(type)
	{
	case MSG_OVERLAY_CALLBACK:
		m_func();
		break;
	default:
		break;
	}
}

void OverlayCallbackComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_OVERLAY_CALLBACK, this);
}

void OverlayCallbackComponent::Shut(){
	m_messenger->Unregister(MSG_OVERLAY_CALLBACK, this);
}

void ParticleComponent::Init(Ogre::SceneManager* p_scene_manager, const Ogre::String& p_particle_name, const Ogre::String& p_particle_file_name){
	m_scene_manager = p_scene_manager;
	m_particle_system = m_scene_manager->createParticleSystem(p_particle_name, p_particle_file_name);
	m_particle_system->setDefaultHeight(0.1f);
}

void ParticleComponent::CreateParticle(Ogre::SceneNode* p_scene_node, const Ogre::Vector3& p_position, const Ogre::Vector3& p_offset_position){
	m_nodes = p_scene_node;
	m_node = m_nodes->createChildSceneNode(p_offset_position);
	m_node->attachObject(m_particle_system);
}

void ParticleComponent::Notify(int type, void* message){
	/*switch (type)
	{
	default:
		break;
	}*/
}

void ParticleComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_CREATE_PARTICLE, this);
}

void ParticleComponent::Shut(){
	m_messenger->Unregister(MSG_CREATE_PARTICLE, this);
	m_particle_system->removeAllEmitters();
}

void CountableResourceGUI::Notify(int type, void* message){
	
	if (type == MSG_LEAF_PICKUP)
	{
		if (m_current_number < m_total_number)
		{
			m_current_number++;
			
			for(int i = 0; i < m_current_number; i++)
			{
				m_elements[i]->setMaterialName(m_material_name_active);
			}
		}
	}
};

void CountableResourceGUI::Shut(){
	m_messenger->Unregister(MSG_LEAF_PICKUP, this);
	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		//delete m_elements.end();
		m_elements[i]->hide();
		m_elements[i]=NULL;
		//delete m_elements[i];
	}
};

void CountableResourceGUI::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_LEAF_PICKUP, this);
};

void CountableResourceGUI::Init(const Ogre::String& material_name_inactive, const Ogre::String& material_name_active, int total_number){
	m_total_number = total_number;
	m_current_number = 0;
	m_material_name_active = material_name_active;
	m_material_name_inactive = material_name_inactive;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay* overlay = overlayManager.create( "OverlayName" );

	Ogre::Vector2 temppos(0.0f, 0.0f);

	if (total_number < 6)
	{ temppos.x = 0.0 + (((6-total_number)/2) * 0.15); }

	float rows = total_number / 6;

	int counter = 0;

	for (int i = 0; i < total_number; i++)
	{
		if (counter == 6)
		{
			counter = 0;
			temppos.x = 15.0f;
			temppos.y += 0.2f;

			if (total_number - i < 6)
			{ temppos.x = 0.0 + (((6-(total_number-i))/2) * 0.15); }
		}
		std::ostringstream stream;
		stream << "Panel" << i;
		Ogre::String panel_name = stream.str();

		m_elements.push_back(static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", panel_name ) ));
		m_elements[i]->setPosition( temppos.x, temppos.y );
		m_elements[i]->setDimensions( 0.15f, 0.15f );
		m_elements[i]->setMaterialName(m_material_name_inactive);
		overlay->add2D(m_elements[i]);
		counter++;
		temppos.x += 0.15f;
	};

    overlay->show();
};

void TerrainComponent::Notify(int type, void* message){

}

void TerrainComponent::Shut(){
	if (m_artifex_loader){
		m_artifex_loader->unloadZone();
		delete m_artifex_loader;
		m_artifex_loader = NULL;
	}
	if (m_terrain_body){
		m_physics_engine->GetDynamicWorld()->removeRigidBody(m_terrain_body);
		delete m_terrain_body;
		m_terrain_body;
	}
	if (m_terrain_shape){
		delete m_terrain_shape;
		m_terrain_shape = NULL;
	}
	if (m_terrain_motion_state){
		delete m_terrain_motion_state;
		m_terrain_motion_state = NULL;
	}
	if (m_data_converter){
		delete[] m_data_converter;
		m_data_converter = NULL;
	}
}

void TerrainComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void TerrainComponent::Init(Ogre::SceneManager* scene_manager, PhysicsEngine* physics_engine, GameObjectManager* game_object_manager, SoundManager* sound_manager, const Ogre::String& filename){
	m_scene_manager = scene_manager;
	m_physics_engine = physics_engine;
	m_artifex_loader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, NULL, m_scene_manager->getCamera("MainCamera"), physics_engine, game_object_manager, sound_manager, "../../resources/terrain/");

//mArtifexLoader = new ArtifexLoader(Ogre::Root::getSingletonPtr(), m_scene_manager, NULL, m_camera, m_game_object_manager, m_sound_manager, "../../resources/terrain/");


	m_artifex_loader->loadZone(filename, true, true, true, true, true, false);
	Ogre::Terrain* terrain = m_artifex_loader->mTerrain;
	size_t w = terrain->getSize();
	float* terrain_height_data = terrain->getHeightData();
	float world_size = terrain->getWorldSize();
	Ogre::Vector3 pos = terrain->getPosition();
	float max_height = terrain->getMaxHeight();
	float min_height = terrain->getMinHeight();

	float* data_converter = new float[terrain->getSize() * terrain->getSize()];
	for (int i = 0; i < terrain->getSize(); i++){
		memcpy(
			data_converter + terrain->getSize() * i,
			terrain_height_data + terrain->getSize() * (terrain->getSize()-i-1),
			sizeof(float)*(terrain->getSize())
			);
	}

	m_terrain_shape = new btHeightfieldTerrainShape(terrain->getSize(), terrain->getSize(), data_converter, 1, terrain->getMinHeight(), terrain->getMaxHeight(), 1, PHY_FLOAT, true);
	
	float units_between_vertices = terrain->getWorldSize() / (w - 1);
	btVector3 local_scaling(units_between_vertices, 1, units_between_vertices);
	m_terrain_shape->setLocalScaling(local_scaling);
	m_terrain_shape->setUseDiamondSubdivision(true);

	m_terrain_motion_state = new btDefaultMotionState;
	m_terrain_body = new btRigidBody(0, m_terrain_motion_state, m_terrain_shape);
	m_terrain_body->getWorldTransform().setOrigin(
		btVector3(
		pos.x,
		pos.y + (terrain->getMaxHeight() - terrain->getMinHeight()) / 2,
		pos.z
		)
		);

	m_terrain_body->getWorldTransform().setRotation(
		btQuaternion(Ogre::Quaternion::IDENTITY.x, Ogre::Quaternion::IDENTITY.y, Ogre::Quaternion::IDENTITY.z, Ogre::Quaternion::IDENTITY.w)
		);

	m_terrain_body->setCollisionFlags(m_terrain_body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_terrain_body->setRestitution(0.5f);
	m_terrain_body->setRollingFriction(1.0f);
	m_terrain_body->setFriction(1.0f);
	int filter = COL_WORLD_STATIC;
	int mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_CAMERA;
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_terrain_body, filter, mask);
	m_collision_def.flag = COLLISION_FLAG_GAME_OBJECT;
	m_collision_def.data = m_owner;
	m_terrain_body->setUserPointer(&m_collision_def);
}
