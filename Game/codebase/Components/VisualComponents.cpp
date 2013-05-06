#include "stdafx.h"
#include "VisualComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "..\InputPrereq.h"

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
	//m_animation_blender = new AnimationBlender(GetEntity());
	//m_animation_blender->init("RunBase");
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id){
	MeshRenderComponent::Init(filename, scene_manager, node_id);
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
}

void AnimationComponent::Notify(int type, void* msg){
	MeshRenderComponent::Notify(type, msg);
	switch (type){
	case MSG_ANIMATION_PLAY:
		{
			int index = static_cast<AnimationMsg*>(msg)->index;
			m_animation_states[index] = m_entity->getAnimationState(static_cast<AnimationMsg*>(msg)->id);
			if (m_animation_states[index] != NULL){
				m_animation_states[index]->setEnabled(true);
				m_animation_states[index]->setLoop(true);
			}
		}
		break;
	case MSG_ANIMATION_PAUSE:
		{
			int index = static_cast<AnimationMsg*>(msg)->index;
			if (m_animation_states[index] != NULL){
				m_animation_states[index]->setEnabled(false);
				m_animation_states[index]->setLoop(false);
			}
		}
		break;
	case MSG_ANIMATION_BLEND:
		//m_animation_blender->blend("RunBase", AnimationBlender::BlendWhileAnimating, 0.2, true);
		//m_animation_blender->blend("IdleTop", AnimationBlender::BlendWhileAnimating, 0.2, true);
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
		m_messenger->Notify(MSG_START_TIMER, NULL);
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
}

void ParticleComponent::CreateParticle(Ogre::SceneNode* p_scene_node, const Ogre::Vector3& p_position, const Ogre::Vector3& p_offset_position){
	m_nodes = p_scene_node;
	m_node = m_nodes->createChildSceneNode(p_offset_position);
	m_node->attachObject(m_particle_system);
}

void ParticleComponent::Notify(int type, void* message){
	switch (type)
	{
	default:
		break;
	}
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
	for (int i = 0; i < m_elements.size(); i++)
	{
		//delete m_elements.end();
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
			temppos.x = 15.0;
			temppos.y += 0.2;

			if (total_number - i < 6)
			{ temppos.x = 0.0 + (((6-(total_number-i))/2) * 0.15); }
		}
		std::ostringstream stream;
		stream << "Panel" << i;
		Ogre::String panel_name = stream.str();

		m_elements.push_back(static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", panel_name ) ));
		m_elements[i]->setPosition( temppos.x, temppos.y );
		m_elements[i]->setDimensions( 0.15, 0.15 );
		m_elements[i]->setMaterialName(m_material_name_inactive);
		overlay->add2D(m_elements[i]);
		counter++;
		temppos.x += 0.15;
	};

    overlay->show();
};
