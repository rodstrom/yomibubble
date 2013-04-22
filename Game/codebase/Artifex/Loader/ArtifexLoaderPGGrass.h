#ifndef ARTIFEX_PG_GRASSLOADER_H
#define ARTIFEX_PG_GRASSLOADER_H

#include "ArtifexLoader.h"
class ArtifexLoader;

using namespace Artifex;
using namespace Ogre;
using namespace std;

#include "PagedGeometry.h"
#include "GrassLoader.h"

using namespace Forests;


class ArtifexLoaderPGGrass {
public:
	ArtifexLoaderPGGrass(ArtifexLoader *artifexLoader);
	~ArtifexLoaderPGGrass();
	
	ArtifexLoader *mArtifexLoader;
	
	PagedGeometry *mGrass;
	GrassLoader *mGrassLoader;
	GrassLayer *mGrassLayer[4];
	
	Image* mCoverMap;
	Image* mColourMap;
	
	Image* mGrassTexture;	
	TexturePtr mGrassTextureTex;
	
	void init();
	void update();	
	void unload();	

};
#endif