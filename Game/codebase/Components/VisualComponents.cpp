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
#include "..\MessageSystem.h"

void NodeComponent::Init(const Ogre::Vector3& pos, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->setPosition(pos);
}

void NodeComponent::Init(Ogre::SceneNode* node, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_node = node->createChildSceneNode();
	m_node->setPosition(0, 1, 0); 
};

void NodeComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_NODE_GET_NODE:
		*static_cast<Ogre::SceneNode**>(msg) = m_node;
		//if (m_owner->GetId() == "TestTott") { std::cout << "Tott pos " << m_node->getPosition() << std::endl; }
		//if (m_owner->GetId() == "TestSpeechBubble") { std::cout << "Speech Bubble at " << m_node->getPosition() << std::endl; }
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
	case MSG_NODE_GET_POSITION:
		{
			*static_cast<Ogre::Vector3*>(msg) = m_node->getPosition();
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
		m_messenger->Unregister(MSG_NODE_GET_POSITION, this);
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
	m_messenger->Register(MSG_NODE_GET_POSITION, this);
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
	Ogre::String top = "Top";

	for (int i = 0; i < m_entity->getSkeleton()->getNumAnimations(); i++){
		Ogre::Animation* anim = m_entity->getSkeleton()->getAnimation(i);
		anim_id = anim->getName();
		size_t find = anim_id.find(base);
		if (find  != std::string::npos){
			RemoveWeights(top_anims, anim);
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

void AnimationComponent::AddAnimationState(const Ogre::String& anim_name, bool loop){
	m_animation_states.push_back(AnimationData(NULL, Ogre::StringUtil::BLANK, false, new AnimationBlender(m_entity)));
	m_animation_states.back().animation_blender->init(anim_name, loop);
	m_animation_states.back().id = anim_name;
}

void AnimationComponent::Update(float dt){
	for (unsigned int i = 0; i < m_animation_states.size(); i++){
		m_animation_states[i].animation_blender->addTime(dt);
	}
}

void AnimationComponent::PlayQueued(){

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
			//m_animation_states[index].wait = true;
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
		//m_messenger->Notify(MSG_OVERLAY_HIDE, NULL);
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

void ParticleComponent::Init(Ogre::SceneManager* p_scene_manager, const Ogre::String& p_particle_name, const Ogre::String& p_particle_file_name, const Ogre::Vector3& position, Ogre::SceneNode* node){
	m_scene_manager = p_scene_manager;
	m_particle_system = m_scene_manager->createParticleSystem(p_particle_name, p_particle_file_name);
	m_particle_system->setDefaultHeight(0.1f);
	m_particle_system->setCastShadows(false);
	node->attachObject(m_particle_system);
}

void ParticleComponent::Notify(int type, void* message){
}

void ParticleComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void ParticleComponent::Shut(){
	m_particle_system->removeAllEmitters();
	m_scene_manager->destroyParticleSystem(m_particle_system);
}

void CountableResourceGUI::Notify(int type, void* message){

	if (type == MSG_LEAF_PICKUP)
	{
		if (m_can_pick_up){
			m_timer_counter = 0.0f;
			m_can_pick_up = false;
			if (m_current_number < m_total_number) {
				Ogre::Overlay::Overlay2DElementsIterator it = m_overlay->get2DElementsIterator();
				int i = 0;
				while (i < m_current_number){
					it.moveNext();
					i++;
				}
			Ogre::OverlayContainer* container = it.peekNext();
			container->setMaterialName("HUD/Leaf/Filled");
			m_current_number++;
			}
		}
	}
};

void CountableResourceGUI::Shut(){
	m_messenger->Unregister(MSG_LEAF_PICKUP, this);
	m_overlay->hide();
};

void CountableResourceGUI::Update(float dt){
	if (m_timer_counter < m_pickup_timer){
		m_timer_counter += dt;
		m_can_pick_up = false;
	}
	else{
		m_can_pick_up = true;
	}

	if (m_level == "try"){
		if (m_current_number == 1){
			//camera zoom
			m_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("ChangeToCameraZoom"));
		}
		else if (m_current_number == 2){
			//get into bubble
			m_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("ChangeToIntoBubble"));
		}
		else if (m_current_number == 3){
			//och här kan man få lära sig gå in i förstapersonsvy :D
			m_messenger->Notify(MSG_TGRAPH_STOP, &Ogre::String("ChangeIntoCameraClick"));
		}
	}
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
		Ogre::OverlayContainer* container = it.getNext();
		container->setMaterialName("HUD/Leaf/Empty");
	}
	m_overlay->show();

	m_timer_counter = 0.0f;
	m_pickup_timer = 3.0f;
	m_level = level_id;
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

ArtifexLoader* TerrainComponent::GetArtifex(){
	return m_artifex_loader;
}

void PlayerStaffComponent::Notify(int type, void* msg){

}

void PlayerStaffComponent::Update(float dt){

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
		if (static_cast<AnimationComponent*>(m_owner->GetGameObjectManager()->GetGameObject(m_tott->GetId())->GetComponent(COMPONENT_ANIMATION))->m_current_animation != "walk"){
			m_owner->GetGameObjectManager()->GetGameObject(m_tott->GetId())->GetComponentMessenger()->Notify(MSG_ANIMATION_PLAY, &msg);
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

void SpeechBubbleComponent::ScaleUp(){
	if (m_current_scale < 1.0f){
		m_current_scale += 0.1f;
		Ogre::Vector3 test(0.1f);
		m_messenger->Notify(MSG_INCREASE_SCALE_BY_VALUE, &test);
	}
};
	
void SpeechBubbleComponent::ScaleDown(){
	if (m_current_scale > 0.1f){
		m_current_scale -= 0.1f;
		Ogre::Vector3 test(-0.1f);
		m_messenger->Notify(MSG_INCREASE_SCALE_BY_VALUE, &test);
	}
};

void SpeechBubbleComponent::Update(float dt){
	
	if (m_player_collide){
		//static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setMaterialName("SolidColor/Blue");
		//m_messenger->Notify(MSG_MESH_SET_MATERIAL_NAME, &Ogre::String("SpeechCherry"));
		//ScaleUp();
	}
	else {
		//static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->setMaterialName("SolidColor/Green");
		//m_messenger->Notify(MSG_MESH_SET_MATERIAL_NAME, &Ogre::String("SpeechCherryInvisible"));
		//ScaleDown();
	}

	m_player_collide = false;
	//m_node->setPosition(m_node->getPosition().x, m_node->getPosition().y, m_node->getPosition().z);

	//static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity()->set

	//m_messenger->Notify(MSG_SET_OBJECT_POSITION, &Ogre::Vector3(m_node->getPosition().x, m_node->getPosition().y, m_node->getPosition().z));

};

void SpeechBubbleComponent::Init(Ogre::SceneNode* node, SceneManager* scene_manager, GameObject* tott){
	m_node = node;
	m_player_collide = false;
	m_current_scale = 1.0f;
	m_messenger->Notify(MSG_MESH_SET_MATERIAL_NAME, &Ogre::String("SpeechCherry"));
	m_tott = tott;
	m_given_leaf = false;
	//m_messenger->Notify(MSG_NODE_ATTACH_ENTITY, static_cast<MeshRenderComponent*>(m_owner->GetComponent(COMPONENT_MESH_RENDER))->GetEntity());

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

void TutorialGraphicsComponent::Notify(int type, void* message){
	Ogre::String msg = *static_cast<Ogre::String*>(message);
	
	switch(type){
	case MSG_TGRAPH_STOP:
		if (m_level == "try"){
		if (msg == "Stick" && m_pic_one == "HUD/Tutorial/Stick_One"){
			m_pic_one = "HUD/Tutorial/BlueBubble_One";
			m_pic_two = "HUD/Tutorial/BlueBubble_Two";
		}
		else if (msg == "BlueBubble" && m_pic_one == "HUD/Tutorial/BlueBubble_One"){
			m_pic_one = "HUD/Tutorial/Jump_One";
			m_pic_two = "HUD/Tutorial/Jump_Two";
		}
		else if (msg == "Jump" && m_pic_one == "HUD/Tutorial/Jump_One"){
			m_overlay->hide();
		}
		else if (msg == "ChangeToCameraZoom" && m_pic_one == "HUD/Tutorial/Jump_One"){
			m_pic_one = "HUD/Tutorial/CameraZoom_One";
			m_pic_two = "HUD/Tutorial/CameraZoom_Two";
			m_overlay->show();
		}
		else if(msg == "CameraZoom" && m_pic_one == "HUD/Tutorial/CameraZoom_One"){
			m_overlay->hide();
		}
		else if (msg == "ChangeToIntoBubble" && m_pic_one == "HUD/Tutorial/CameraZoom_One"){
			m_pic_one = "HUD/Tutorial/IntoBubble_One";
			m_pic_two = "HUD/Tutorial/IntoBubble_Two";
			m_overlay->show();
		}
		else if(msg == "IntoBubble" && m_pic_one == "HUD/Tutorial/IntoBubble_One"){
			m_overlay->hide();
		}
		else if (msg == "ChangeIntoCameraClick" && m_pic_one == "HUD/Tutorial/IntoBubble_One"){
			m_pic_one = "CleverBugFix";
			m_pic_one_sec = "HUD/Tutorial/CameraClick_One";
			m_pic_two_sec = "HUD/Tutorial/CameraClick_Two";
			m_overlay_sec->show();
		}
		else if (msg == "CameraClick" && m_pic_one_sec == "HUD/Tutorial/CameraClick_One"){
			m_overlay_sec->hide();
		}
		else{}
		} //if level == try

		if(msg == "PinkBubble" && m_pic_one == "HUD/Tutorial/PinkBubble_One"){
			m_overlay->hide();
		}

		if (msg == "CameraMove" && m_overlay_sec->isVisible()){
			m_overlay_sec->hide();
		}
		break;
	default:
		break;
	};
};

void TutorialGraphicsComponent::Shut(){
	m_messenger->Unregister(MSG_TGRAPH_STOP, this);
};

void TutorialGraphicsComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_TGRAPH_STOP, this);
};

void TutorialGraphicsComponent::Init(const Ogre::String& id, const Ogre::String& level){
	m_timer = 1;
	m_timer_counter = 0;
	m_first_pic = true;

	m_level = level;

	m_pic_one_sec = "";
	m_pic_two_sec = "";

	m_overlay = Ogre::OverlayManager::getSingleton().getByName(id);
	m_overlay_sec = Ogre::OverlayManager::getSingleton().getByName(id);
	
	if (level == "try" || level == "Dayarea") { m_overlay->show(); }

	if (level == "try"){
		m_pic_one = "HUD/Tutorial/Stick_One";
		m_pic_two = "HUD/Tutorial/Stick_Two";

		m_pic_one_sec = "HUD/Tutorial/CameraMove_One";
		m_pic_two_sec = "HUD/Tutorial/CameraMove_Two";
		m_overlay_sec = Ogre::OverlayManager::getSingleton().getByName("Level1_Add");
		m_overlay_sec->show();
		m_overlay->show();
	}
	else if (level == "Dayarea"){
		m_overlay_sec->show();
		m_overlay->hide();
		m_pic_one = "HUD/Tutorial/PinkBubble_One";
		m_pic_two = "HUD/Tutorial/PinkBubble_Two";
	}
};

void TutorialGraphicsComponent::Update(float dt){
	m_timer_counter += dt;
	if (m_timer_counter >= m_timer){
		m_timer_counter = 0;
		if (m_first_pic) { m_first_pic = false; }
		else { m_first_pic = true; }
	}
	if (m_overlay->isVisible()){
		if (m_first_pic){
			Ogre::Overlay::Overlay2DElementsIterator it = m_overlay->get2DElementsIterator();
			Ogre::OverlayContainer* container = it.peekNext();
			container->setMaterialName(m_pic_one);
		}
		else{
			Ogre::Overlay::Overlay2DElementsIterator it = m_overlay->get2DElementsIterator();
			Ogre::OverlayContainer* container = it.peekNext();		
			container->setMaterialName(m_pic_two);
		}
	}
	if (m_level == "try"){
		if (m_overlay_sec->isVisible()){
			if (m_first_pic){
				Ogre::Overlay::Overlay2DElementsIterator it = m_overlay_sec->get2DElementsIterator();
				Ogre::OverlayContainer* container = it.peekNext();
				container->setMaterialName(m_pic_one_sec);
			}
			else{
				Ogre::Overlay::Overlay2DElementsIterator it = m_overlay_sec->get2DElementsIterator();
				Ogre::OverlayContainer* container = it.peekNext();
				container->setMaterialName(m_pic_two_sec);
			}
		}
	}
};

void GateControllerComponent::Notify(int type, void* msg){
	switch (type){
	case MSG_GATE_OPEN_GET:
		{
			if (m_counter <= 0){
				*static_cast<bool*>(msg) = true;
			}
			else {
				*static_cast<bool*>(msg) = false;
			}
		}
		break;
	default:
		break;
	}
}

void GateControllerComponent::Shut(){
	m_messenger->Unregister(MSG_GATE_OPEN_GET, this);
	m_message_system->Unregister(EVT_LEAF_PICKUP, this);
}

void GateControllerComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_GATE_OPEN_GET, this);
}

void GateControllerComponent::Update(float dt){
	if (!m_can_decrease) {   // ugly hack to prevent double pickup
		m_timer += dt;
		if (m_timer >= 1.0f){
			m_can_decrease = true;
		}
	}
	if (m_rotate){
		float y_axis = m_left_gate_node->getOrientation().y;
		if (y_axis >= 0.7f){
			m_rotate = false;
		}
		float rot_speed = 0.1f * dt;
		m_left_gate_node->rotate(Ogre::Quaternion(1.0f, 0.0f, rot_speed, 0.0f));
		m_right_gate_node->rotate(Ogre::Quaternion(1.0f, 0.0f, -rot_speed, 0.0f));
	}
}

void GateControllerComponent::Init(MessageSystem* message_system, int leaves){
	m_counter = leaves;
	m_message_system = message_system;
	m_message_system->Register(EVT_LEAF_PICKUP, this, &GateControllerComponent::LeafPickup);
	m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &m_left_gate_node, "left_gate_node");
	m_messenger->Notify(MSG_CHILD_NODE_GET_NODE, &m_right_gate_node, "right_gate_node");
}

void GateControllerComponent::LeafPickup(IEvent* evt){
	if (evt->m_type == EVT_LEAF_PICKUP){
		if (m_can_decrease){
			m_counter--;
			if (m_counter <= 0){
				this->OpenGate();
			}
			else {
				m_can_decrease = false;
				m_timer = 0.0f;
			}
		}
	}
}

void GateControllerComponent::OpenGate(){
	m_rotate = true;
	m_owner->RemoveComponent(COMPONENT_RIGIDBODY);	// remove the collider so we can pass through the gate
}

void RotationComponent::Notify(int type, void* message){

}

void RotationComponent::Shut(){

}

void RotationComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void RotationComponent::Update(float dt){
	float rot_speed = m_rotation_speed * dt;
	m_node->rotate(Ogre::Quaternion(1.0f, 0.0f, rot_speed, 0.0f));
}

void RotationComponent::Init(Ogre::SceneNode* node, float rotation_speed){
	m_node = node;
	m_rotation_speed = rotation_speed;
}
