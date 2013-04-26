#include "stdafx.h"
#include "VisualComponents.h"
#include "ComponentMessenger.h"
#include "..\Managers\InputManager.h"
#include "..\InputPrereq.h"

void MeshRenderComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	m_scene_manager = scene_manager;
	m_entity = m_scene_manager->createEntity(filename);
	m_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
	m_node->attachObject(m_entity);
}

void MeshRenderComponent::Notify(int type, void* msg){
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
	default:
		break;
	}
}

void MeshRenderComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
	m_messenger->Register(MSG_NODE_GET_NODE, this);
	m_messenger->Register(MSG_INCREASE_SCALE_BY_VALUE, this);
	m_messenger->Register(MSG_SET_OBJECT_POSITION, this);
}

void MeshRenderComponent::Shut(){
	if (m_messenger){
		m_messenger->Unregister(MSG_NODE_GET_NODE, this);
		m_messenger->Unregister(MSG_INCREASE_SCALE_BY_VALUE, this);
		m_messenger->Unregister(MSG_SET_OBJECT_POSITION, this);
	}
	if (m_node != NULL){
		m_scene_manager->destroySceneNode(m_node);
		m_node = NULL;
	}
	if (m_entity != NULL){
		m_scene_manager->destroyEntity(m_entity);
		m_entity = NULL;
	}
}

void AnimationComponent::SetMessenger(ComponentMessenger* messenger){
	MeshRenderComponent::SetMessenger(messenger);
	m_messenger->Register(MSG_ANIMATION_PLAY, this);
	m_messenger->Register(MSG_ANIMATION_PAUSE, this);
}

void AnimationComponent::Init(const Ogre::String& filename, Ogre::SceneManager* scene_manager){
	MeshRenderComponent::Init(filename, scene_manager);
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
	Ogre::OverlayManager* overlay_manager = Ogre::OverlayManager::getSingletonPtr();
	m_id = p_overlay_name;
	m_cont_name = p_cont_name;

	m_overlay = overlay_manager->getByName(m_id);
	m_overlay->show();
}

void Overlay2DComponent::Shut()
{
	if (m_messenger){
		m_messenger->Unregister(MSG_GET_2D_OVERLAY_CONTAINER, this);
		
	}
	//Ogre::OverlayManager& overlay_mrg = Ogre::OverlayManager::getSingleton();
	//overlay_mrg.destroy(m_overlay);
}

void Overlay2DComponent::Notify(int type, void* msg){
	switch(type){
	case MSG_GET_2D_OVERLAY_CONTAINER:
		*static_cast<Ogre::OverlayContainer**>(msg) = m_overlay->getChild(m_cont_name);	
		break;
	default:
		break;
	}
}

void Overlay2DComponent::SetMessenger(ComponentMessenger* messenger) {
	m_messenger = messenger;
	m_messenger->Register(MSG_GET_2D_OVERLAY_CONTAINER, this);
	
}

void OverlayCollisionCallbackComponent::Init(InputManager* p_input_manager, Ogre::Viewport* p_view_port){
	m_input_manager = p_input_manager;
	m_view_port = p_view_port;
	m_ocs = OCS_DEFAULT;
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
	}
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
	Ogre::OverlayManager& overlay_mrg = Ogre::OverlayManager::getSingleton();
	overlay_mrg.destroy(m_overlay);
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

void SkyXComponent::Init(Ogre::SceneManager* p_scene_manager, const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt, const bool& lc){
	
	m_scene_manager = p_scene_manager;
	m_basic_controller = new SkyX::BasicController;
	m_sky_x = new SkyX::SkyX(m_scene_manager, m_basic_controller);
 	Ogre::String name = m_sky_x->getGPUManager()->getSkydomeMaterialName();
	m_sky_x->create();
	m_sky_x->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));
	
	//m_camera = p_camera;
	m_time = t;
	m_time_multiplier = tm;
	m_moon_phase = mp;
	m_atmosphere_opt = atmOpt;
	m_layered_clouds = lc;
	//m_volumetric_clouds = vc;
	//m_vc_wind_speed = vcws;
	//m_vc_auto_update = vcauto;
	//m_vc_wind_dir = vcwd;
	//m_vc_ambient_color = vcac;
	//m_vc_light_response = vclr;
	//m_vc_ambient_factors = vcaf;
	//m_vc_wheater = vcw;
	//m_vc_lightnings = vcl;
	//m_vc_lightningsAT = vclat;
	//m_vc_lightnings_color;
	//m_vc_lightningsTM = vcltm;

	
}

void SkyXComponent::Update(float dt){
	m_sky_x->update(dt);
}

void SkyXComponent::Notify(int type, void* message){
}

void SkyXComponent::SetMessenger(ComponentMessenger* messenger){
	m_messenger = messenger;
}

void SkyXComponent::Shut(){
}

void SkyXComponent::CreateSkyBox(){
	
	
	m_sky_x->setTimeMultiplier(m_time_multiplier);
	m_basic_controller->setTime(m_time);
	m_basic_controller->setMoonPhase(m_moon_phase);
	m_sky_x->getAtmosphereManager()->setOptions(m_atmosphere_opt);

	// Layered clouds
	if (m_layered_clouds)
	{
		// Create layer cloud
		if (m_sky_x->getCloudsManager()->getCloudLayers().empty())
		{
			m_sky_x->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
		}
	}
	else
	{
		// Remove layer cloud
		if (!m_sky_x->getCloudsManager()->getCloudLayers().empty())
		{
			m_sky_x->getCloudsManager()->removeAll();
		}
	}

	m_sky_x->getVCloudsManager()->setWindSpeed(m_vc_wind_speed);
	m_sky_x->getVCloudsManager()->setAutoupdate(m_vc_auto_update);

	SkyX::VClouds::VClouds* vclouds = m_sky_x->getVCloudsManager()->getVClouds();

	vclouds->setWindDirection(m_vc_wind_dir);
	vclouds->setAmbientColor(m_vc_ambient_color);
	vclouds->setLightResponse(m_vc_light_response);
	vclouds->setAmbientFactors(m_vc_ambient_factors);
	vclouds->setWheater(m_vc_wheater.x, m_vc_wheater.y, false);

	if (m_volumetric_clouds)
	{
		
		// Create VClouds
		if (!m_sky_x->getVCloudsManager()->isCreated())
		{
			// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
			//m_sky_x->getVCloudsManager()->create(m_sky_x->getMeshManager()->getSkydomeRadius(m_camera));
		}
	}
	else
	{
		// Remove VClouds
		if (m_sky_x->getVCloudsManager()->isCreated())
		{
			m_sky_x->getVCloudsManager()->remove();
		}
	}

	vclouds->getLightningManager()->setEnabled(m_vc_lightnings);
	vclouds->getLightningManager()->setAverageLightningApparitionTime(m_vc_lightningsAT);
	vclouds->getLightningManager()->setLightningColor(m_vc_lightnings_color);
	vclouds->getLightningManager()->setLightningTimeMultiplier(m_vc_lightningsTM);

	//mTextArea->setCaption(buildInfoStr());

	// Reset camera position/orientation
	//mRenderingCamera->setPosition(0,0,0);
	//mRenderingCamera->setDirection(0,0,1);

	m_sky_x->update(0);
}