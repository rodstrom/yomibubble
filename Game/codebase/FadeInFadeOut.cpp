#include "stdafx.h"
#include "FadeInFadeOut.h"
#include "OgreMaterialManager.h"
#include "OgreOverlayManager.h"
#include "OgreTechnique.h"
#include "OgreBlendMode.h"


FadeInFadeOut::FadeInFadeOut(const Ogre::String& p_overlay_name, const Ogre::String& p_material_name){
	m_current_dur = 0.0;
	m_is_fading = false;
	m_fade_state = FADE_NONE;
	m_alpha = 0.0f;

	Ogre::ResourcePtr res_ptr = Ogre::MaterialManager::getSingleton().getByName(p_material_name);
	Ogre::Material* material = static_cast<Ogre::Material*>(res_ptr.getPointer());

	Ogre::Technique* technique = material->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	m_texture = pass->getTextureUnitState(0);

	m_overlay = Ogre::OverlayManager::getSingleton().getByName(p_overlay_name);
	m_overlay->hide();
}


FadeInFadeOut::~FadeInFadeOut(void){
	
}

void FadeInFadeOut::Update(float dt){

	if( m_fade_state != FADE_NONE && m_texture )
     {
         // Set the _alpha value of the _overlay
		 m_texture->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, m_alpha);    // Change the _alpha operation
 
         // If fading in, decrease the _alpha until it reaches 0.0
		 if( m_fade_state == FADE_IN )
         {
			 if( dt < 0.0 )
				dt = -dt;
			if( dt < 0.000001 )
				dt = 1.0;

             m_current_dur -= dt;
             m_alpha = m_current_dur / m_total_dur;
             if( m_alpha < 0.0 )
             {
                 m_overlay->hide();
                 m_fade_state = FADE_NONE;
				 if( m_fade_in_callback != NULL ){
					 m_fade_in_callback();
					 m_fade_in_callback = NULL;
				 }

             }
         }
 
         // If fading out, increase the _alpha until it reaches 1.0
		 else if( m_fade_state == FADE_OUT )
         {

			 	if( dt < 0.0 )
					dt = -dt;
				if( dt < 0.000001 )
					dt = 1.0;
			
             m_current_dur += dt;
             m_alpha = m_current_dur / m_total_dur;
             if( m_alpha > 1.0 )
             {
				 m_is_fading = true;
                 m_fade_state = FADE_NONE;
				 if(m_fade_out_callback != NULL ){
					 m_fade_out_callback();
					 m_fade_out_callback = NULL;
				 }
             }
         }
     }
}

void FadeInFadeOut::FadeIn(float duration){
	if(m_fade_state == FADE_IN)
		return;
	m_is_fading = true;
	 m_alpha = 1.0f;
	 m_total_dur = duration;
	 m_current_dur = duration;
     m_fade_state = FADE_IN;
	 m_overlay->show();
	 
}


void FadeInFadeOut::FadeOut(float duration){
	if(m_fade_state == FADE_OUT)
		return;

	m_is_fading = true;
     m_alpha = 0.0;
	 m_total_dur = duration;
	  //m_current_dur = duration;
	 m_current_dur = 0.0;
     m_fade_state = FADE_OUT;
     m_overlay->show();
	
}