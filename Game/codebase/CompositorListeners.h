#ifndef _COMPOSITOR_LISTENERS_H_
#define _COMPOSITOR_LISTENERS_H_

class GlowMaterialListener : public Ogre::MaterialManager::Listener
{
protected:
	Ogre::MaterialPtr mBlackMat;
public:
	GlowMaterialListener()
	{
		mBlackMat = Ogre::MaterialManager::getSingleton().create("mGlowBlack", "Internal");
		mBlackMat->getTechnique(0)->getPass(0)->setDiffuse(0,0,0,0);
		mBlackMat->getTechnique(0)->getPass(0)->setSpecular(0,0,0,0);
		mBlackMat->getTechnique(0)->getPass(0)->setAmbient(0,0,0);
		mBlackMat->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,0);
	}
 
	Ogre::Technique *handleSchemeNotFound(unsigned short, const Ogre::String& schemeName, Ogre::Material*mat, unsigned short, const Ogre::Renderable*)
	{
		if (schemeName == "glow")
		{
			//LogManager::getSingleton().logMessage(">> adding glow to material: "+mat->getName());
			return mBlackMat->getTechnique(0);
		}
		return NULL;
	}
};

#endif // _COMPOSITOR_LISTENERS_H_