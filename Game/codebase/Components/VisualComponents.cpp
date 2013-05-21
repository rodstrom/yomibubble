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
		//if (m_owner->GetId() == "TestTott") { std::cout << "Tott pos " << m_node->getPosition() << std::endl; }
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
	case MSG_SET_OBJECT_ORIENTATION:
		m_node->setOrientation(*static_cast<Ogre::Quaternion*>(msg));
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
		m_messenger->Unregister(MSG_SET_OBJECT_ORIENTATION, this);
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
	m_messenger->Register(MSG_SET_OBJECT_ORIENTATION, this);
}

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_messenger->Notify(MSG_NODE_ATTACH_ENTITY, &m_entity);
}

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	if (node_id != Ogre::StringUtil::BLANK){
		m_messenger->Notify(MSG_NODE_ATTACH_ENTITY, &m_entity, node_id);
	}
}

void MeshRenderComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_MESH_SET_MATERIAL_NAME:
		m_entity->setMaterialName(*static_cast<Ogre::String*>(msg));
		break;
	default:
		break;
	}
}

void MeshRenderComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_MESH_SET_MATERIAL_NAME, this);
}

void MeshRenderComponent::Shut(){
	if (m_entity != NULL){
		m_scene_manager->destroyEntity(m_entity);
		m_entity = NULL;
	}
	m_messenger->Unregister(MSG_MESH_SET_MATERIAL_NAME, this);
}

void AnimationComponent::SetMessenger(ComponentMessenger* messenger){
	MeshRenderComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_ANIMATION_PLAY, this);
	m_messenger->Register(MSG_ANIMATION_PAUSE, this);
	m_messenger->Register(MSG_ANIMATION_BLEND, this);
	m_messenger->Register(MSG_ANIMATION_LOOP, this);
	m_messenger->Register(MSG_ANIMATION_QUEUE, this);
	m_messenger->Register(MSG_ANIMATION_CALLBACK, this);
	m_messenger->Register(MSG_ANIMATION_CLEAR_QUEUE, this);
	m_messenger->Register(MSG_ANIMATION_CLEAR_CALLBACK, this);
	m_messenger->Register(MSG_ANIMATION_SET_WAIT, this);
	m_messenger->Register(MSG_ANIMATION_GET_CURRENT_NAME, this);
	m_messenger->Register(MSG_ANIMATION_IS_DONE, this);
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, bool remove_weights){
	MeshRenderComponent::Init(filename, scene_manager);
	m_entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
	m_animation_blender = new AnimationBlender(GetEntity());
	if (remove_weights){
		FixPlayerWeights();
	}
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager, const Ogre::String& node_id, bool remove_weights){
	MeshRenderComponent::Init(filename, scene_manager, node_id);
	m_entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
	m_animation_blender = new AnimationBlender(GetEntity());
	if (remove_weights){
		FixPlayerWeights();
	}
	m_current_animation = "";
}

void AnimationComponent::FixPlayerWeights(){
	std::vector<std::string> base_anims;
	base_anims.push_back("CATRigLLegPlatform");
	base_anims.push_back("CATRigLLegAnkle");
	base_anims.push_back("CATRigLLeg2");
	base_anims.push_back("CATRigLLeg1");
	base_anims.push_back("Yomi_Pelvis");
	base_anims.push_back("CATRigRLeg1");
	base_anims.push_back("CATRigRLeg2");
	base_anims.push_back("CATRigRLegAnkle");
	base_anims.push_back("CATRigAnkleBone001");
	base_anims.push_back("CATRigRLegPlatform");
	base_anims.push_back("CATRigTail1");
	base_anims.push_back("CATRigTail2");
	base_anims.push_back("CATRigTail3");
	base_anims.push_back("CATRigTail4");
	base_anims.push_back("CATRigTail5");
	std::vector<std::string> top_anims;
	top_anims.push_back("CATRigSpine1");
	top_anims.push_back("CATRigSpine2");
	top_anims.push_back("CATRigSpine3");
	top_anims.push_back("Yomi_Torso");
	top_anims.push_back("CATRigLArmCollarbone");
	top_anims.push_back("CATRigLArm1");
	top_anims.push_back("CATRigLArm21");
	top_anims.push_back("CATRigLArm22");
	top_anims.push_back("CATRigLArmPalm");
	top_anims.push_back("CATRigLArmDigit21");
	top_anims.push_back("CATRigLArmDigit22");
	top_anims.push_back("CATRigLArmDigit11");
	top_anims.push_back("CATRigRArmCollarbone");
	top_anims.push_back("CATRigRArm1");
	top_anims.push_back("CATRigRArm21");
	top_anims.push_back("CATRigRArm22");
	top_anims.push_back("CATRigRArmPalm");
	top_anims.push_back("CATRigRArmDigit21");
	top_anims.push_back("CATRigRArmDigit22");
	top_anims.push_back("CATRigRArmDigit11");
	top_anims.push_back("CATRigRightHair1");
	top_anims.push_back("CATRigRightHair2");
	top_anims.push_back("CATRigRightHair3");
	top_anims.push_back("CATRigLeftHair1");
	top_anims.push_back("CATRigLeftHair2");
	top_anims.push_back("CATRigLeftHair3");
	top_anims.push_back("CATRigBackHair1");
	top_anims.push_back("CATRigBackHair2");
	top_anims.push_back("CATRigBackHair3");
	top_anims.push_back("Yomi_Head");

	Ogre::String line = Ogre::StringUtil::BLANK;
	Ogre::String anim_id = Ogre::StringUtil::BLANK;
	Ogre::String base = "Base";
	Ogre::String blow = "Blow";
	Ogre::String top = "Top";

	for (int i = 0; i < m_entity->getSkeleton()->getNumAnimations(); i++){
		Ogre::Animation* anim = m_entity->getSkeleton()->getAnimation(i);
		anim_id = anim->getName();
		size_t find = anim_id.find(base);
		if (find  != std::string::npos){
			RemoveWeights(top_anims, anim);
			continue;
		}
		find = anim_id.find(blow);
		if (find != std::string::npos){
			RemoveWeights(base_anims, anim);
			continue;
		}
		find = anim_id.find(top);
		if (find != std::string::npos){
			RemoveWeights(base_anims, anim);
			continue;
		}
	}
}

void AnimationComponent::RemoveWeights(std::vector<std::string>& list, Ogre::Animation* anim){
	Ogre::Skeleton::BoneIterator bone_it = m_entity->getSkeleton()->getBoneIterator();
	while (bone_it.hasMoreElements()){
		Ogre::Bone* bone = bone_it.getNext();
		Ogre::String bone_id = bone->getName();
		std::vector<std::string>::iterator it = std::find(list.begin(), list.end(), bone_id);
		if (it != list.end()){
			anim->destroyNodeTrack(bone->getHandle());
		}
	}
}

void AnimationComponent::AddAnimationStates(unsigned int value){
	for (unsigned int i = 0; i < value; i++){
		Ogre::AnimationState* a = NULL;
		m_animation_states.push_back(AnimationData(NULL, Ogre::StringUtil::BLANK, false, new AnimationBlender(m_entity)));
	}
	m_animation_states[0].animation_blender->init("Base_Idle", true);
	m_animation_states[0].id = "Base_Idle";
	m_animation_states[1].animation_blender->init("Top_Idle", true);
	m_animation_states[1].id = "Top_Idle";
}

void AnimationComponent::Update(float dt){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		//if (m_animation_states[i].anim_state != NULL){
			//if (m_animation_states[i].anim_state->getEnabled()){
				//m_animation_states[i].anim_state->addTime(dt);
			if (m_animation_states[i].active){
				m_animation_states[i].animation_blender->addTime(dt);
			}
				/*if (!m_animation_states[i].anim_state->getLoop() && m_animation_states[i].anim_state->hasEnded()){
					if (m_animation_states[i].wait){
						m_animation_states[i].wait = false;
					}
					if (m_callback){
						m_callback();
						m_callback = NULL;
					}
					PlayQueued();
				}*/
			//}
		//}
	}
}

void AnimationComponent::PlayQueued(){
	/*if (!m_queue.empty()){
		if (m_animation_states[m_queue.front().index].anim_state != NULL){
			if (m_animation_states[m_queue.front().index].anim_state->getEnabled()){
				m_animation_states[m_queue.front().index].anim_state->setTimePosition(0);
				m_animation_states[m_queue.front().index].anim_state->setEnabled(false);
				m_animation_states[m_queue.front().index].id = Ogre::StringUtil::BLANK;
			}
		}
		if (m_queue.front().full_body){
			if (m_animation_states[1].anim_state != NULL){
				if (m_animation_states[1].anim_state->getEnabled()){
					m_animation_states[1].anim_state->setTimePosition(0);
					m_animation_states[1].anim_state->setEnabled(false);
					m_animation_states[1].id = Ogre::StringUtil::BLANK;
				}
			}
		}
		m_animation_states[m_queue.front().index].anim_state = m_entity->getAnimationState(m_queue.front().id);
		m_animation_states[m_queue.front().index].anim_state->setEnabled(true);
		m_animation_states[m_queue.front().index].anim_state->setLoop(m_queue.front().loop);
		m_animation_states[m_queue.front().index].anim_state->setTimePosition(0);
		m_animation_states[m_queue.front().index].id = m_queue.front().id;
		m_queue.pop_front();
	}*/
}

void AnimationComponent::Notify(int type, void* msg){
	MeshRenderComponent::Notify(type, msg);
	switch (type){
	case MSG_ANIMATION_PLAY:
		{
			AnimationMsg& anim_msg = *static_cast<AnimationMsg*>(msg);
			if (m_animation_states[anim_msg.index].id != anim_msg.id){
				if (!m_animation_states[anim_msg.index].active){
					m_animation_states[anim_msg.index].active = true;
				}
				m_animation_states[anim_msg.index].animation_blender->blend(anim_msg.id, (AnimationBlender::BlendingTransition)anim_msg.blending_transition, anim_msg.duration, anim_msg.loop);
				m_animation_states[anim_msg.index].id = anim_msg.id;
			}
		}
		break;
	case MSG_ANIMATION_PAUSE:
		{
			int index = *static_cast<int*>(msg);
			//m_animation_states[index].animation_blender->getSource()->setWeight(0);
			//m_animation_states[index].active = false;
			//m_animation_states[index].animation_blender->getSource()->setTimePosition(0);
			//m_animation_states[index].animation_blender->getSource()->setEnabled(false);
		}
		break;
	case MSG_ANIMATION_LOOP:
		{
			AnimationMsg& anim_msg = *static_cast<AnimationMsg*>(msg);
			if (m_animation_states[anim_msg.index].anim_state != NULL){
				m_animation_states[anim_msg.index].anim_state->setLoop(anim_msg.loop);
			}
		}
		break;
	case MSG_ANIMATION_QUEUE:
		{
			AnimationMsg& anim_msg = *static_cast<AnimationMsg*>(msg);
			m_queue.push_back(anim_msg);
		}
		break;
	case MSG_ANIMATION_CALLBACK:
		m_callback = *static_cast<std::function<void()>*>(msg);
		break;
	case MSG_ANIMATION_CLEAR_QUEUE:
		m_queue.clear();
		break;
	case MSG_ANIMATION_CLEAR_CALLBACK:
		m_callback = NULL;
		break;
	case MSG_ANIMATION_SET_WAIT:
		{
			int index = *static_cast<int*>(msg);
			m_animation_states[index].wait = true;
		}
		break;
	case MSG_ANIMATION_GET_CURRENT_NAME:
		{
			AnimNameMsg& anim_msg = *static_cast<AnimNameMsg*>(msg);
			anim_msg.id = m_animation_states[anim_msg.index].animation_blender->getSource()->getAnimationName();
		}
		break;
	case MSG_ANIMATION_IS_DONE:
		{
			AnimIsDoneMsg& anim_msg = *static_cast<AnimIsDoneMsg*>(msg);
			anim_msg.is_done = m_animation_states[anim_msg.index].animation_blender->complete;
		}
		break;
	default:
		break;
	}
}

void AnimationComponent::Shut(){
	if (!m_animation_states.empty()){
		for (unsigned int i = 0; i < m_animation_states.size(); i++){
			if (m_animation_states[i].anim_state != NULL){
				m_animation_states[i].anim_state->setEnabled(false);
			}
		}
	}
	m_animation_states.clear();
	m_messenger->Unregister(MSG_ANIMATION_PLAY, this);
	m_messenger->Unregister(MSG_ANIMATION_PAUSE, this);
	m_messenger->Unregister(MSG_ANIMATION_BLEND, this);
	m_messenger->Unregister(MSG_ANIMATION_LOOP, this);
	m_messenger->Unregister(MSG_ANIMATION_QUEUE, this);
	m_messenger->Unregister(MSG_ANIMATION_CALLBACK, this);
	m_messenger->Unregister(MSG_ANIMATION_CLEAR_QUEUE, this);
	m_messenger->Unregister(MSG_ANIMATION_CLEAR_CALLBACK, this);
	m_messenger->Unregister(MSG_ANIMATION_SET_WAIT, this);
	m_messenger->Unregister(MSG_ANIMATION_GET_CURRENT_NAME, this);
	m_messenger->Unregister(MSG_ANIMATION_IS_DONE, this);
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
		if (!m_can_pick_up){
		if (m_current_number < m_total_number)
		{
			Ogre::Overlay::Overlay2DElementsIterator it = m_overlay->get2DElementsIterator();
			int i = 0;
			while (i < m_current_number){
				it.moveNext();
				i++;
			}
			Ogre::OverlayContainer* container = it.peekNext();
			if (i == 0){ 
				container->setMaterialName("HUD/Leaf/FilledFront"); }
			else if (i == m_total_number-1){
			container->setMaterialName("HUD/Leaf/FilledEnd"); }
			else{
			container->setMaterialName("HUD/Leaf/FilledMiddle"); }
			m_current_number++;
		}
		m_can_pick_up = true;
		}
		else{
			m_can_pick_up = false;
		}
	}
};

void CountableResourceGUI::Shut(){
	m_messenger->Unregister(MSG_LEAF_PICKUP, this);
	m_overlay->hide();
};

void CountableResourceGUI::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_LEAF_PICKUP, this);
};

void CountableResourceGUI::Init(const Ogre::String& level_id){
	m_can_pick_up = false;
	m_overlay = Ogre::OverlayManager::getSingleton().getByName(level_id);
	Ogre::Overlay::Overlay2DElementsIterator it = m_overlay->get2DElementsIterator();
	while (it.hasMoreElements()){
		m_total_number++;		// Search the overlay for X amount of containers so we know how many leaves the level has.
		it.moveNext();
	}
	m_overlay->show();
}

void TerrainComponent::Notify(int type, void* message){

}

void TerrainComponent::Shut(){

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
	if (m_artifex_loader){
		m_artifex_loader->unloadZone();
		delete m_artifex_loader;
		m_artifex_loader = NULL;
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
	int mask = COL_PLAYER | COL_TOTT | COL_BUBBLE | COL_CAMERA | COL_QUESTITEM;
	m_physics_engine->GetDynamicWorld()->addRigidBody(m_terrain_body, filter, mask);
	m_collision_def.flag = COLLISION_FLAG_STATIC;
	m_collision_def.data = m_owner;
	m_terrain_body->setUserPointer(&m_collision_def);
}

void TerrainComponent::Update(float dt){
	m_artifex_loader->mDBManager->Update();
}

void PlayerStaffComponent::Notify(int type, void* msg){

}

void PlayerStaffComponent::Update(float dt){
	Ogre::Bone* bone = m_player_entity->getSkeleton()->getBone("CATRigLArmDigit21");
	Ogre::Vector3 pos = bone->_getDerivedPosition() * m_node->_getDerivedPosition();
}

void PlayerStaffComponent::Shut(){
	m_scene_manager->destroySceneNode(m_node);
	m_scene_manager->destroyEntity(m_entity);
}

void PlayerStaffComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void PlayerStaffComponent::Init(Ogre::SceneManager* scene_manager, Ogre::Entity* player_entity){
	m_scene_manager = scene_manager;
	m_player_entity = player_entity;
	m_entity = m_scene_manager->createEntity("Staff.mesh");
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->attachObject(m_entity);
}

void SpeechBubbleComponent::Notify(int type, void* message){
	AnimationMsg msg;
		msg.blend = false;
		msg.full_body = true;
		msg.id = "walk";
		msg.index = 0;
		msg.loop = true;
		msg.wait = false;
	
	switch(type){
	case MSG_SP_BUBBLE_SHOW:
		if (static_cast<AnimationComponent*>(m_owner->GetGameObjectManager()->GetGameObject("TestTott")->GetComponent(COMPONENT_ANIMATION))->m_current_animation != "walk"){
			m_owner->GetGameObjectManager()->GetGameObject("TestTott")->GetComponentMessenger()->Notify(MSG_ANIMATION_PLAY, &msg);
		}
		m_player_collide = true;
		break;
	default:
		break;
	}
};

void SpeechBubbleComponent::Shut(){
	m_messenger->Unregister(MSG_SP_BUBBLE_SHOW, this);
};

void SpeechBubbleComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_SP_BUBBLE_SHOW, this);
};

void SpeechBubbleComponent::Update(float dt){
	
	if (m_player_collide){
		static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setMaterialName("SolidColor/Blue");
	}
	else {
		static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setMaterialName("SolidColor/Green");
	}

	m_player_collide = false;

};

void SpeechBubbleComponent::Init(Ogre::SceneNode* node, SceneManager* scene_manager){
	m_node = node;
	m_player_collide = false;
	//static_cast<MeshRenderComponent*>(speech_bubble->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setMaterialName("SolidColor/Blue");
	//m_mesh = new MeshRenderComponent;
	//m_owner->AddComponent(m_mesh);
	//m_mesh->Init("PratBubbla.mesh", scene_manager, "TottNode");
	

	/*
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay* overlay = overlayManager.create( "OverlayNameBajs" ); 

	Ogre::String panel_name = "BajsPanel";

	Ogre::OverlayContainer* element;
	*/
	/*
	element = (static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", panel_name ) ));
	element->SetPosition
	element->setDimensions( 0.15f, 0.15f );
	element->setMaterialName("Speech_Bubble.png");
	overlay->add3D(element);
	*/
};