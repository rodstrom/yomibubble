#include "stdafx.h"
#include "ArtifexLoaderPGGrass.h"

using namespace Artifex;
using namespace Ogre;
using namespace std;

ArtifexLoaderPGGrass::ArtifexLoaderPGGrass(ArtifexLoader *artifexLoader) {
	mArtifexLoader = artifexLoader;
};

ArtifexLoaderPGGrass::~ArtifexLoaderPGGrass() {

};

static ArtifexLoader* sArtifexLoader=NULL;

static Real getTerrainHeight(const Real x, const Real z, void *userDataL=NULL) 
{
	Real f = sArtifexLoader->getHeightAt(x,z);
	return f;
};

void ArtifexLoaderPGGrass::init() {
	if (trim(mArtifexLoader->mGrassColourMap)=="") mArtifexLoader->mGrassColourMap="ETbase.png";
	if (trim(mArtifexLoader->mGrassMaterial)=="") mArtifexLoader->mGrassMaterial="pg_grass";
	if (mArtifexLoader->mGrassDensity==0.0f) mArtifexLoader->mGrassDensity=0.5f;
	if (mArtifexLoader->mGrassRange==0) mArtifexLoader->mGrassRange = 500;
	
	Forests::MapChannel channel;
	
	StringUtil::toUpperCase(mArtifexLoader->mGrassCoverChannel);
	
	if (mArtifexLoader->mGrassCoverChannel == "R") channel = CHANNEL_RED;
	else if (mArtifexLoader->mGrassCoverChannel == "G") channel = CHANNEL_GREEN;
	else if (mArtifexLoader->mGrassCoverChannel == "B") channel = CHANNEL_BLUE;
	else if (mArtifexLoader->mGrassCoverChannel == "A") channel = CHANNEL_ALPHA;
	else {
		mArtifexLoader->mGrassCoverChannel="R";
		channel = CHANNEL_RED;
	}
	
	// check if colourmap exists, otherwise create
	mColourMap = new Image();
	try {
		mColourMap->load(mArtifexLoader->mGrassColourMap, ETM_GROUP);
		//cout << "colourmap loaded\n";
	} catch (Exception &e) {
		Artifex::CreateImage(*mColourMap,2048,2048,PF_A8B8G8R8,true,60,70,40);
		mColourMap->save(mArtifexLoader->mZonePath+mArtifexLoader->mZoneName+"/"+mArtifexLoader->mGrassColourMap);
		//cout << mArtifexLoader->mUserFilePath.c_str() << " + " << mArtifexLoader->mGrassColourMap.c_str() << "\n";
	};	
	mColourMap->~Image();
	
	mCoverMap = new Image();
	try {
		mCoverMap->load(mArtifexLoader->mGrassCoverMap, ETM_GROUP);
	} catch (Exception &e) {
		Artifex::CreateImage(*mCoverMap,1024,1024,PF_A8B8G8R8);
		mColourMap->save(mArtifexLoader->mZonePath+mArtifexLoader->mZoneName+"/"+mArtifexLoader->mGrassCoverMap);
	};
	mCoverMap->~Image();	

	mGrassTexture = new Image();
	try {
		// "../media/"+mZoneName+"/"+
		mGrassTexture->load(mArtifexLoader->mGrassCoverMap,ETM_GROUP);	  

		mGrassTextureTex = TextureManager::getSingleton().createManual(
		"ArtifexTerra3D_GrassTexture", ETM_GROUP, Ogre::TEX_TYPE_2D, 1024, 1024, 1, mGrassTexture->getFormat());	

		mGrassTextureTex->unload();
		mGrassTextureTex->loadImage(*mGrassTexture); 	
	  
	}catch(Exception &e) { 
		std::cout <<"#-#-#-# Error loading GrassLayer: "<< e.getDescription() <<"\n"; 
		uchar *pImage = new uchar[1024 * 1024 * 4];
        mGrassTexture->loadDynamicImage(pImage, 1024, 1024, PF_A8R8G8B8); 
        
        mGrassTextureTex = TextureManager::getSingleton().createManual(
		"ArtifexTerra3D_GrassTexture", ETM_GROUP, Ogre::TEX_TYPE_2D, 1024, 1024, 1, mGrassTexture->getFormat());	

		mGrassTextureTex->unload();
		mGrassTextureTex->loadImage(*mGrassTexture); 
	}
	
	//-------------------------------------- LOAD GRASS --------------------------------------
	//Create and configure a new PagedGeometry instance for mGrass
	mGrass = new PagedGeometry(mArtifexLoader->mCamera, 100);	
	mGrass->addDetailLevel<GrassPage>(mArtifexLoader->mGrassRange);

	//Create a GrassLoader object
	mGrassLoader = new GrassLoader(mGrass);
	mGrass->setPageLoader(mGrassLoader);	//Assign the "treeLoader" to be used to load geometry for the PagedGeometry instance

	//Supply a height function to GrassLoader so it can calculate mGrass Y values
	sArtifexLoader = const_cast <ArtifexLoader*> (mArtifexLoader);
	mGrassLoader->setHeightFunction(&getTerrainHeight);

	//Add some mGrass to the scene with GrassLoader::addLayer()	
	mGrassLayer[0] = mGrassLoader->addLayer(mArtifexLoader->mGrassMaterial);

	//Configure the mGrass layer properties (size, density, animation properties, fade settings, etc.)
	mGrassLayer[0]->setMinimumSize(2.0f, 2.0f);
	mGrassLayer[0]->setMaximumSize(4.5f, 4.5f);
	mGrassLayer[0]->setAnimationEnabled(mArtifexLoader->mGrassAnimate);		//Enable animations
	mGrassLayer[0]->setSwayDistribution(10.0f);		//Sway fairly unsynchronized
	mGrassLayer[0]->setSwayLength(0.5f);				//Sway back and forth 0.5 units in length
	mGrassLayer[0]->setSwaySpeed(0.25f);				//Sway 1/2 a cycle every second	
	mGrassLayer[0]->setDensity(mArtifexLoader->mGrassDensity);				//Relatively dense mGrass
	mGrassLayer[0]->setFadeTechnique(FADETECH_GROW);	//Distant mGrass should slowly raise out of the ground when coming in range
	mGrassLayer[0]->setRenderTechnique(GRASSTECH_QUAD);	//Draw mGrass as scattered quads
	
	//This sets a color map to be used when determining the color of each mGrass quad. setMapBounds()
	//is used to set the area which is affected by the color map. Here, "terrain_texture.jpg" is used
	//to color the mGrass to match the terrain under it.	
	
	mGrassLayer[0]->setColorMap(mArtifexLoader->mGrassColourMap);	
	mGrassLayer[0]->setDensityMap("ArtifexTerra3D_GrassTexture", channel);
	mGrassLayer[0]->setMapBounds(TBounds(0, 0, mArtifexLoader->mTerrX, mArtifexLoader->mTerrZ));	//(0,0)-(1500,1500) is the full boundaries of the terrain
	mGrassLayer[0]->setLightingEnabled(true);	
};

void ArtifexLoaderPGGrass::update() {
	mGrass->update();
};

void ArtifexLoaderPGGrass::unload() {
	//Delete the GrassLoader instance
	try {
		delete mGrass->getPageLoader();
	} catch(...) {};

	//Delete the PagedGeometry instance
	try {
		delete mGrass;
	} catch(...) {};
};


