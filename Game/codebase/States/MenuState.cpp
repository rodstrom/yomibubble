#include "stdafx.h"
#include "MenuState.h"
#include "..\Managers\InputManager.h"
#include "..\PhysicsEngine.h"
#include "..\Managers\SoundManager.h"
#include "..\Managers\GameObjectManager.h"
#include "..\FadeInFadeOut.h"
#include "..\Managers\VariableManager.h"
#include "..\PhysicsPrereq.h"
#include "TreeLoader3D.h"
#include "WindBatchPage.h"
#include "ImpostorPage.h"

MenuState::MenuState(void) : m_quit(false), m_current_selected_button(0) {} 
MenuState::~MenuState(void){}

void MenuState::Enter(){
	m_scene_manager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "MenuStateSceneManager");
	m_sound_manager = new SoundManager;
	m_sound_manager->Init(m_scene_manager);
	m_sound_manager->LoadAudio();
	m_fade->SetFadeInCallBack(NULL);
	m_fade->FadeIn(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_in_timer"));
	std::function<void()> func = [this] { ChangeStateToPlayState(); };
	m_fade->SetFadeOutCallBack(func);
	float dir_x, dir_y, dir_z;
	dir_x = 0.0f;
	dir_y = -1.0f;
	dir_z = 0.0f;
	m_scene_manager->setSkyBox(true, "_MySky", 2300.0f);
	
	m_target_time = (float)Ogre::Math::RangeRandom(Ogre::Real(1.0f), Ogre::Real(3.0f));
	m_anim_timer = 0.0f;

	m_camera = m_scene_manager->createCamera("MenuCamera");
	m_camera->setNearClipDistance(1.0f);
	m_viewport = m_render_window->addViewport(m_camera);
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	m_camera->setPosition(0.0f, -1.5f, 0.0f);

	m_paged_geometry = new Forests::PagedGeometry;
	m_paged_geometry->setCamera(m_camera);
	m_paged_geometry->setPageSize(60);
	m_paged_geometry->setInfinite();
	m_paged_geometry->addDetailLevel<Forests::WindBatchPage>(40,10);
	m_paged_geometry->addDetailLevel<Forests::ImpostorPage>(100, 10);

	Forests::TreeLoader3D* tree_loader = new Forests::TreeLoader3D(m_paged_geometry, Forests::TBounds(0,0,500,500));
	m_paged_geometry->setPageLoader(tree_loader);

	m_scene_manager->setShadowTextureSelfShadow(false);
	m_scene_manager->setShadowCasterRenderBackFaces(false);
	m_scene_manager->setShadowTextureCount(1);
	m_scene_manager->setShadowTextureSize(2048);
	m_scene_manager->setShadowColour(Ogre::ColourValue(0.5f,0.5f,0.6f,1.0f));
	m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

	Ogre::Light* mLight = m_scene_manager->createLight( "Light" );
	//mLight->setType( Ogre::Light::LT_POINT );
	mLight->setType( Ogre::Light::LT_DIRECTIONAL );
	mLight->setDirection(Ogre::Vector3(dir_x, dir_y, dir_z).normalisedCopy());
	Ogre::ColourValue diffuse(0.5, 0.5, 0.5, 0.5);
	Ogre::ColourValue specular(0.5, 0.5, 0.5, 0.5);
	mLight->setDiffuseColour(diffuse);
	mLight->setSpecularColour(specular);
	//mLight->setPosition(mLightPosX, mLightPosY, mLightPosZ);
	mLight->setCastShadows(true);
	
	m_physics_engine = new PhysicsEngine;
	m_physics_engine->Init();
	m_game_object_manager = new GameObjectManager;
	m_game_object_manager->Init(m_physics_engine, m_scene_manager, m_input_manager, m_viewport, m_sound_manager, m_message_system, NULL);
	
	//Ogre::Plane plane(Vector3::UNIT_Y, -0.9);
	//Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 20, 20, 5, 5, true, 1, 5, 5, Vector3::UNIT_Z);
	m_yomi_ent = m_scene_manager->createEntity("Yomi.mesh");
	Ogre::Entity* menu_scene = m_scene_manager->createEntity("MenySceen.mesh");
	Ogre::Entity* m_tott = m_scene_manager->createEntity("Hidehog.mesh");
	Ogre::Entity* yomi_staff = m_scene_manager->createEntity("Staff.mesh");
	m_yomi_ent->attachObjectToBone("CATRigLArmDigit21", yomi_staff, Ogre::Quaternion(1.0f, 1.0f, 0.7f, 0.0f), Ogre::Vector3(0.01f, -0.02f, 0.0f));
	Ogre::Entity* menu_grass = m_scene_manager->createEntity("MainMenyGrass.mesh");
	Ogre::Entity* menu_bush = m_scene_manager->createEntity("MenuBushes.mesh");
	
	/////////////////////////////////////////////////////////////////////////////////77
	m_walk_speed = 35.0f;
	m_direction = Ogre::Vector3::ZERO;
	m_destination = Ogre::Vector3::ZERO;
	
	m_old_destination = Ogre::Vector3::ZERO; //node->getPosition();
	m_loop_waypoints = true;

	m_walk_list.push_back(Ogre::Vector3(0.3f, -2.0f, -2.0f));
	m_walk_list.push_back(Ogre::Vector3(0.3f, -2.0f, -10.0f));
	
	m_paged_geometry->setCustomParam(menu_grass->getName(), "windFactorX", 0.175f);
	m_paged_geometry->setCustomParam(menu_grass->getName(), "windFactorY", 0.001f);
	m_paged_geometry->setCustomParam(menu_bush->getName(), "windFactorX", 0.125f);
	m_paged_geometry->setCustomParam(menu_bush->getName(), "windFactorY", 0.002f);

	tree_loader->addTree(menu_grass, Ogre::Vector3(0.0f, -2.0f, -3.0f));
	tree_loader->addTree(menu_bush, Ogre::Vector3(0.0f, -2.0f, -3.0f));
	
	FixYomiWeights();
	m_yomi_node = m_scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.3f, -2.0f, -2.0f));
	m_yomi_node->attachObject(m_yomi_ent);

	m_scene_node = m_scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.0f, -2.0f, -3.0f));
	m_scene_node->attachObject(menu_scene);

	m_tott_node = m_scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.0f, -2.0f, -10.0f));
	m_tott_node->yaw(Radian(4));
	m_tott_node->attachObject(m_tott);
	m_follow_node = m_tott_node;


	m_menu_grass = m_scene_manager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(m_scene_node->getPosition()));
	m_menu_grass->attachObject(menu_grass);
	

	m_yomi_base = m_yomi_ent->getAnimationState("Base_Idle");
	m_yomi_top = m_yomi_ent->getAnimationState("Top_Idle");
	m_yomi_base->setEnabled(true);
	m_yomi_top->setEnabled(true);
	m_tott_animation = m_tott->getAnimationState("Run");
	m_tott_animation->setEnabled(true);

	Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "Bloom");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", true);

	OverlayDef menuBackground;
	menuBackground.overlay_name = "Menu";
	menuBackground.cont_name = "Menu/Background";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	menuBackground.overlay_name = "Menu";
	menuBackground.cont_name = "Menu/BackgroundBubbles";
	m_game_object_manager->CreateGameObject(GAME_OBJECT_OVERLAY, Ogre::Vector3(0,0,0), &menuBackground);

	ButtonDef buttonDef;
	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Start";
	buttonDef.mat_start_hover = "Menu/StartHover";
	buttonDef.mat_start = "Menu/Start";
	buttonDef.func = [this] { m_fade->FadeOut(VariableManager::GetSingletonPtr()->GetAsFloat("Fade_out_timer"));};
	
	m_buttons[0] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);

	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Options";
	buttonDef.mat_start_hover = "Menu/OptionsHover";
	buttonDef.mat_start = "Menu/Options";
	buttonDef.func = [this] {  };
	m_buttons[1] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);

	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Credits";
	buttonDef.mat_start_hover = "Menu/CreditsHover";
	buttonDef.mat_start = "Menu/Credits";
	buttonDef.func = [this] {  };
	m_buttons[2] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);

	buttonDef.overlay_name = "Menu";
	buttonDef.cont_name = "Menu/Quit";
	buttonDef.mat_start_hover = "Menu/QuitHover";
	buttonDef.mat_start = "Menu/Quit";
	buttonDef.func = [this] { ChangeStateToExit(); };
	m_buttons[3] = m_game_object_manager->CreateGameObject(GAME_OBJECT_BUTTON, Ogre::Vector3(0,0,0), &buttonDef);

}

void MenuState::Exit(){
	delete m_paged_geometry->getPageLoader();
	delete m_paged_geometry;
	m_game_object_manager->Shut();
	delete m_game_object_manager;
	m_game_object_manager = NULL;
	m_physics_engine->CloseDebugDraw();
	m_physics_engine->Shut();
	delete m_physics_engine;
	m_physics_engine = NULL;
	m_sound_manager->Stop2DMusic("Menu_theme");
	m_sound_manager->Exit();
	delete m_sound_manager;
	m_sound_manager = NULL;
	m_render_window->removeAllViewports();
	Ogre::Root::getSingleton().destroySceneManager(m_scene_manager);
	
}

bool MenuState::Update(float dt){
	m_sound_manager->Update(m_scene_manager, dt);
	m_sound_manager->Play2DMusic("Menu_theme");
	m_game_object_manager->Update(dt);
	m_physics_engine->Step(dt);
	m_anim_timer += dt;
	m_paged_geometry->update();
	//std::cout << m_anim_timer << std::endl;
	//m_game_object_manager->LateUpdate(dt);
	if (m_input_manager->IsButtonPressed(BTN_BACK))
		return false;
	
	m_yomi_base->addTime(dt);
	m_yomi_top->addTime(dt);
	m_tott_animation->addTime(dt);


	
	if(m_anim_timer >= m_target_time){
		if(m_yomi_base == m_yomi_ent->getAnimationState("Base_Idle")){
			m_yomi_base = m_yomi_ent->getAnimationState("Base_Idle2");
			m_yomi_top = m_yomi_ent->getAnimationState("Top_Idle2");
			m_yomi_base->setEnabled(true);
			m_yomi_top->setEnabled(true);
			m_anim_timer = 0.0f;
			m_target_time = 6.6f;
		}
		else{
			m_yomi_base = m_yomi_ent->getAnimationState("Base_Idle");
			m_yomi_top = m_yomi_ent->getAnimationState("Top_Idle");
			m_yomi_base->setEnabled(true);
			m_yomi_top->setEnabled(true);
			m_anim_timer = 0.0f;
			m_target_time = (float)Ogre::Math::RangeRandom(Ogre::Real(2.0f), Ogre::Real(4.0f));
		}
	}

	//m_fade->Update(dt);

	if (m_input_manager->IsButtonPressed(BTN_DOWN)) 
		m_current_selected_button++;
	if (m_input_manager->IsButtonPressed(BTN_UP)) 
		m_current_selected_button--;

	if (m_current_selected_button < 0) 
		m_current_selected_button = 3;
	if (m_current_selected_button > 3) 
		m_current_selected_button = 0;

	for (int i = 0; i < 4; i++){
		if(i != m_current_selected_button) {
			m_buttons[i]->GetComponentMessenger()->Notify(MSG_OVERLAY_HOVER_EXIT, NULL);
		}
		else {
			m_buttons[i]->GetComponentMessenger()->Notify(MSG_OVERLAY_HOVER_ENTER, NULL);
		}
	}

	if(m_input_manager->IsButtonPressed(BTN_A))
	{
		m_buttons[m_current_selected_button]->GetComponentMessenger()->Notify(MSG_OVERLAY_CALLBACK, NULL);
		m_buttons[m_current_selected_button]->GetComponentMessenger()->Notify(MSG_OVERLAY_HIDE, NULL);
	}

	return !m_quit;
}

void MenuState::ChangeStateToPlayState()
{
	ChangeState(FindByName("PlayState"));
}

void MenuState::ChangeStateToOptions(){}

void MenuState::ChangeStateToCredits(){}

void MenuState::ChangeStateToExit(){
	m_quit = true;
}

void MenuState::FixYomiWeights(){
	std::vector<std::string> base_anims;
	base_anims.push_back("CATRigLLegPlatform");
	base_anims.push_back("CATRigLLegAnkle");
	base_anims.push_back("CATRigLLeg2");
	base_anims.push_back("CATRigLLeg1");
	base_anims.push_back("Yomi_Pelvis");
	base_anims.push_back("CATRigRLeg1");
	base_anims.push_back("CATRigRLeg2");
	base_anims.push_back("CATRigRLegAnkle");
	base_anims.push_back("CATRigAnkle001");
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

	for (int i = 0; i < m_yomi_ent->getSkeleton()->getNumAnimations(); i++){
		Ogre::Animation* anim = m_yomi_ent->getSkeleton()->getAnimation(i);
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

void MenuState::RemoveWeights(std::vector<std::string>& list, Ogre::Animation* anim){
	Ogre::Skeleton::BoneIterator bone_it = m_yomi_ent->getSkeleton()->getBoneIterator();
	while (bone_it.hasMoreElements()){
		Ogre::Bone* bone = bone_it.getNext();
		Ogre::String bone_id = bone->getName();
		std::vector<std::string>::iterator it = std::find(list.begin(), list.end(), bone_id);
		if (it != list.end()){
			anim->destroyNodeTrack(bone->getHandle());
		}
	}
}

bool MenuState::NextLocation(){

	if ( m_walk_list.empty() ) { 
		m_direction = Ogre::Vector3::ZERO; return false; 
	}
	
	m_destination = m_walk_list.front();  // this gets the front of the deque
	if (m_loop_waypoints) m_walk_list.push_back(m_walk_list.front()); //if looping keep the list rotating so that it never ends
	m_walk_list.pop_front();             // this removes the front of the deque
 
	m_direction = m_destination - m_yomi_node->getPosition();
	//m_direction.normalise();

	return true;
}

bool MenuState::WithinDistance(float meters){
	if (m_yomi_node->getPosition().x >= m_destination.x - meters
		&& m_yomi_node->getPosition().x <= m_destination.x + meters)
	{
		if (m_yomi_node->getPosition().z >= m_destination.z - meters
		&& m_yomi_node->getPosition().z <= m_destination.z + meters)
		{
			return true;
		}
	}
	return false;
}
