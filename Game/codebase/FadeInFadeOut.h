#ifndef _FADE_IN_FADE_OUT_H
#define _FADE_IN_FADE_OUT_H

#include <functional>
#include "Components\VisualComponents.h"

enum FadeState{
	FADE_NONE = 0,
	FADE_IN,
	FADE_OUT
};

class FaderCallBack{
public:
	virtual void FadeInCallBack(void){}
	virtual void FadeOutCallBack(void){}
};

class FadeInFadeOut
{
public:
	FadeInFadeOut(const Ogre::String& p_overlay_name, const Ogre::String& p_material_name, FaderCallBack* instance = 0);
	virtual ~FadeInFadeOut(void);

	void Update(float dt);
	void FadeIn(float duration);
	void FadeOut(float duration);
	void SetFadeInCallBack(std::function<void()> func){ m_fade_in_callback = func; }
	void SetFadeOutCallBack(std::function<void()> func){ m_fade_out_callback = func; }
	bool IsFading() const { return m_is_fading; }

protected:
	std::function<void()>		m_fade_in_callback;
	std::function<void()>		m_fade_out_callback;
	double						m_alpha;
	double						m_current_dur;
	double						m_total_dur;
	FadeState					m_fade_state;
	Ogre::Overlay*				m_overlay;
	Ogre::TextureUnitState*		m_texture;
	bool						m_is_fading;
};

#endif //_FADE_IN_FADE_OUT_H