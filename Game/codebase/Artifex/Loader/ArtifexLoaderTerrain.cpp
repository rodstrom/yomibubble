/* ===========================================================================
Artifex Loader v1.1 beta

Freeware

Copyright (c) 2012 Thomas Gradl alias Nauk

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
THE SOFTWARE.
=========================================================================== */

#include "stdafx.h"
#include "ArtifexLoader.h"

using namespace std;
using namespace Ogre;
using namespace Artifex;

void ArtifexLoader::loadTerrainSettings() {
	Ogre::ConfigFile cfgfile;		
	try {
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");
		
		mTerrX = strToF(cfgfile.getSetting("TerrainWidth_X"));
		mTerrZ = strToF(cfgfile.getSetting("TerrainWidth_Z"));
		mTerrY = strToF(cfgfile.getSetting("TerrainHeight_Y"));
		
		if (mTerrX == 0) mTerrX = 5000;
		if (mTerrZ == 0) mTerrZ = 5000;
		if (mTerrY == 0) mTerrY = 1667;
		
		mTerrainLoD = strToI(cfgfile.getSetting("TerrainLoD"))==1?true:false;;
		if (cfgfile.getSetting("TerrainLoD")=="") mTerrainLoD = true;
		
		mTerrainPixelError = strToF(cfgfile.getSetting("TerrainPixelError"));
		if (mTerrainPixelError == 0) mTerrainPixelError = 2;
		
		mTerrainLoDMorphingFactor = strToF(cfgfile.getSetting("TerrainLoDMorphingFactor"));;
		if (mTerrainLoDMorphingFactor == 0) mTerrainLoDMorphingFactor = 0.2f;
	
		mTerrainMaxLoD = strToI(cfgfile.getSetting("TerrainMaxLoD"));;
		if (mTerrainMaxLoD == 0) mTerrainMaxLoD = 255;
	
		mTerrainTileSize = strToI(cfgfile.getSetting("TerrainTileSize"));;
		if (mTerrainTileSize == 0) mTerrainTileSize = 33;
		
		mTerrainVertexCount = strToI(cfgfile.getSetting("TerrainEdgeVertexCount"));;
		if (mTerrainVertexCount==0) mTerrainVertexCount = 513;
	
		mTerrainVertexNormals = strToI(cfgfile.getSetting("TerrainVertexNormals"))==1?true:false;;
		if (cfgfile.getSetting("TerrainVertexNormals") == "") mTerrainVertexNormals = true;
		
		mTerrainVertexTangents = strToI(cfgfile.getSetting("TerrainVertexTangents"))==1?true:false;;
		if (cfgfile.getSetting("TerrainVertexTangents") == "") mTerrainVertexTangents = true;
		
		mSplattScaleX = strToF(cfgfile.getSetting("TerrainSplattScale_X"));	
		mSplattScaleZ = strToF(cfgfile.getSetting("TerrainSplattScale_Z"));	
		
		if (mSplattScaleX == 0) mSplattScaleX = 192;
		if (mSplattScaleZ == 0) mSplattScaleZ = 192;
		
		mTextureCount = strToI(cfgfile.getSetting("TextureCount"));;
		if (mTextureCount<=0 || mTextureCount>9) mTextureCount = 9;
		
		for (int i=0; i<mTextureCount; i++) {
			mTexturePath[i] = cfgfile.getSetting("Splatting"+StringConverter::toString(i));		
		}		
		
	} catch (Exception &e) {
		cout << "Error loading Terrain Settings: " << e.what() << ".\n";

	}
};

void ArtifexLoader::setupTextures() {
	String num = "";
	mTextureSize = 1024;
	
	for (int a=0;a<mTextureCount;a++) {
		num = StringConverter::toString(a);
		mSplatting[a] = new Image();
		// mZonePath+mZoneName+"/"
		mSplatting[a]->load(mTexturePath[a],ETM_GROUP);
		mTextureSize = (int)mSplatting[a]->getWidth();

		// create a manual texture	  
		mSplattingTex[a] = TextureManager::getSingleton().createManual(
		"Splatting"+num, ETM_GROUP, Ogre::TEX_TYPE_2D, mTextureSize, mTextureSize, 1, mSplatting[a]->getFormat());	
		// load the imagefile 
		mSplattingTex[a]->loadImage(*mSplatting[a]);
	}
};

void ArtifexLoader::createColourLayer() 
{
	mOverlay = new Image();	
	try {	
		if (mZoneName!="") {
			//mZonePath+mZoneName+"/
			mOverlay->load("artifexterra3d_colourmap.png",ETM_GROUP);	  
		}

		mOverlayTex = TextureManager::getSingleton().createManual(
		"ArtifexTerra3D_ColourMap", ETM_GROUP, Ogre::TEX_TYPE_2D, 2048, 2048, 1, mOverlay->getFormat());	

		mOverlayTex->unload();
		mOverlayTex->loadImage(*mOverlay); 	
	  
	}catch(Exception &e) { 
		std::cout <<"Troubles loading colourlayer, DON'T PANIC, rolling my own.\n"; 	
		
		uchar *pImage = new uchar[2048 * 2048 * 4];
        mOverlay->loadDynamicImage(pImage, 2048, 2048, PF_A8R8G8B8);         
		
		mOverlayTex = TextureManager::getSingleton().createManual(
		"ArtifexTerra3D_ColourMap", ETM_GROUP, Ogre::TEX_TYPE_2D, 2048, 2048, 1, mOverlay->getFormat());	

		mOverlayTex->unload();
		mOverlayTex->loadImage(*mOverlay); 	
	}	
};

#ifdef TSM_TERRAIN
// TSM section
void ArtifexLoader::initTSM() {
	for (int a=0;a<3;a++) {
	
		cout << "Creating ETSplatting" << a << "\n";
	
		mCoverage[a].load("ETcoverage."+iToStr(a)+".png",ETM_GROUP);
	
		mCoverageTex[a] = TextureManager::getSingleton().createManual(
			"ETSplatting"+iToStr(a), ETM_GROUP, Ogre::TEX_TYPE_2D, 
			mCoverage[a].getWidth(), mCoverage[a].getHeight(), 1, mCoverage[a].getFormat()
		);
		
		mCoverageTex[a]->unload();
		mCoverageTex[a]->loadImage(mCoverage[a]);
	}	
};
void ArtifexLoader::loadTerrain() {
	try {
		mSceneMgr->setWorldGeometry("terrain.cfg");
	} catch (exception &e) {
		cout << e.what() << "\n";
	}
};
#elif defined(ETM_TERRAIN)
// ETM section
void ArtifexLoader::loadImgTerrain() {
	Image image;
	
	image.load("ETterrain.png", ETM_GROUP);

	ET::TerrainInfo info;
	ET::loadHeightmapFromImage(info, image);
	info.setExtents(AxisAlignedBox(0, 0, 0, mTerrX, mTerrY, mTerrZ));
	mTerrainMgr->createTerrain(info, mTerrainTileSize, mTerrainMaxLoD, mTerrainVertexNormals, mTerrainVertexTangents);
};
void ArtifexLoader::loadBinaryTerrain() {
	size_t width=0, height=0;
	
	ifstream::pos_type size;
	char * memblock;
	
	string path = mZonePath+mZoneName+"/artifex_terrain.bin";
	
	ifstream terrainfile (path.c_str(), ios::in | ios::binary | ios::ate ); 
	
	if (terrainfile.is_open())
	{
		size = terrainfile.tellg();
		memblock = new char [size];
		terrainfile.seekg (0, ios::beg);
		terrainfile.read (memblock, size);
		terrainfile.close();
		
		width = *((size_t*) memblock);
		height = *((size_t*) (memblock+sizeof(size_t)));
		
		ET::TerrainInfo info (width,height,(const float*)(memblock+(2*sizeof(size_t))));			
		info.setExtents(AxisAlignedBox(0, 0, 0, mTerrX, mTerrY, mTerrZ));
		mTerrainMgr->createTerrain(info, mTerrainTileSize, mTerrainMaxLoD, mTerrainVertexNormals, mTerrainVertexTangents);			
		
		delete[] memblock;		

	} else {
		cout << "Troubles opening terrainfile: artifex_terrain.bin - weird.\n"; 
	}	
};
void ArtifexLoader::loadTerrain()
{
	try {
		mTerrainMgr->destroyTerrain();
	} catch (Exception &e) {};
	Image image;
	
	// check if a binary is there then load the binary
	string path = mZonePath+mZoneName+"/artifex_terrain.bin";
	
	if (fileExists(path)) {
		cout << "Loading binary terraindata.\n";
		loadBinaryTerrain();
	} else {
		cout << "Loading image terraindata.\n";
		loadImgTerrain();
	}
	
	cout << "Done.\n";
	
	
	mTerrainInfo = const_cast<ET::TerrainInfo*> (&mTerrainMgr->getTerrainInfo());

	// now load the splatting maps
	
	for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
	{
		// mZonePath+mZoneName+"/
		image.load("ETcoverage."+StringConverter::toString(i)+".png", ETM_GROUP);
		mSplatMgr->loadMapFromImage(i, image);
	}
	
	MaterialPtr material (MaterialManager::getSingleton().getByName("ETTerrainMaterial"));
	mTerrainMgr->setMaterial(material);
	
	updateLightmap();	   
};
void ArtifexLoader::initETM() {	
	mTerrainMgr = new ET::TerrainManager(mSceneMgr);
	mTerrainMgr->setLODErrorMargin(mTerrainPixelError, mCamera->getViewport()->getActualHeight());
	mTerrainMgr->setUseLODMorphing(mTerrainLoD, mTerrainLoDMorphingFactor, "morphFactor");	

	ET::TerrainInfo terrainInfo (mTerrainVertexCount, mTerrainVertexCount, std::vector<float>(mTerrainVertexCount * mTerrainVertexCount, 0.5f));
	terrainInfo.setExtents(AxisAlignedBox(0, 0, 0, mTerrX, mTerrY, mTerrZ));
	mTerrainMgr->createTerrain(terrainInfo);	
	
	mCoverMapSize = 1024;
	mSplatMgr = new ET::SplattingManager("ETSplatting", ETM_GROUP, mCoverMapSize, mCoverMapSize, 3);
	mSplatMgr->setNumTextures(9);

	TexturePtr lightmapTex = TextureManager::getSingleton().createManual(
	"ETLightmap", ETM_GROUP, TEX_TYPE_2D, 128, 128, 1, PF_BYTE_RGB); 
	Image lightmap;	
	
	ET::createTerrainLightmap(terrainInfo, lightmap, 128, 128, Vector3(1, -1, 1), ColourValue::White,
	ColourValue(0.7, 0.7, 0.7),true);
	lightmapTex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
};
void ArtifexLoader::updateLightmap()
{
	Image lightmap;
	ET::createTerrainLightmap(*mTerrainInfo, lightmap, 128, 128, Vector3(1, -1, 1), ColourValue(1,1,1),
		ColourValue(0.7, 0.7,  0.7), true);     

	TexturePtr tex = TextureManager::getSingleton().getByName("ETLightmap");
	tex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
};
#elif defined(OT_TERRAIN)
	// Ogre::Terrain section
void ArtifexLoader::initOgreTerrain()
{

	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 1025, mTerrX);
    mTerrainGroup->setOrigin(Ogre::Vector3(mTerrX/2,0,mTerrZ/2));
    
    mMTerrMatGen.bind( OGRE_NEW  TerrainMaterialGeneratorC() );            
    mTerrainGlobals->setDefaultMaterialGenerator( mMTerrMatGen );    

    configureTerrainDefaults(mLight);
 
    loadTerrain();
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadTerrain(0,0,true);
	
	mTerrain = mTerrainGroup->getTerrain(0,0);
	
	// param true: convert and save blendmaps
	loadOrCreateBlendMaps(false);
	
	loadGlobalColourMap();	
	
    mTerrainGroup->freeTemporaryResources();	
};	
void ArtifexLoader::loadTerrain()
{
	string path = mZonePath+mZoneName+"/"+mHeightMapBinarayFileName;
	
	if (Artifex::fileExists(path)) {
		cout << "Loading binary terrain...\n";
		loadBinaryTerrain();
	} else {
		cout << "Loading image terrain...\n";
		loadImgTerrain();
	}
};
void ArtifexLoader::loadBinaryTerrain() {
	size_t width=0, height=0;
	
	ifstream::pos_type size;
	char * memblock;
	
	string path = mZonePath+mZoneName+"/"+mHeightMapBinarayFileName;
	
	ifstream terrainfile (path.c_str(), ios::in | ios::binary | ios::ate ); 
	
	if (terrainfile.is_open())
	{
		size = terrainfile.tellg();
		memblock = new char [size];
		terrainfile.seekg (0, ios::beg);
		terrainfile.read (memblock, size);
		terrainfile.close();
		
		width = *((size_t*) memblock);
		height = *((size_t*) (memblock+sizeof(size_t)));
		
		// Flip & revert Terrain float array to Ogre::Terrain
		Artifex::flipFArray((float*)(memblock+(2*sizeof(size_t))), width, height, Artifex::FLIPX);
		Artifex::flipFArray((float*)(memblock+(2*sizeof(size_t))), width, height, Artifex::REVERSE);
		
		Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
		defaultimp.inputFloat = (float*)(memblock+(2*sizeof(size_t)));
		defaultimp.terrainSize = width;
		
		mTerrainGroup->defineTerrain(0, 0,(const Ogre::Terrain::ImportData*) &defaultimp);
		
		delete[] memblock;		

	} else {
		cout << "Troubles opening terrainfile: artifex_terrain.bin - weird.\n"; 
	}	
};
void ArtifexLoader::loadImgTerrain()
{	
	if(mHeightMapImage != NULL) delete mHeightMapImage;
	
	mHeightMapImage = new Ogre::Image(); 
	mHeightMapImage->load(mHeightMapImageFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	
	Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = mHeightMapImage->getWidth();
	
	mTerrainGroup->defineTerrain(0, 0, mHeightMapImage);
};
void ArtifexLoader::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(mTerrainPixelError);
    // composite map
    mTerrainGlobals->setCompositeMapDistance(30000);
    
    mTerrainGlobals->setCastsDynamicShadows(false);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data  
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());  
    //mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight()*0.005f);
	//mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour()*0.002f);
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();

    defaultimp.worldSize = mTerrX;
    defaultimp.inputScale = mTerrY;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;     
    
    // textures        
    defaultimp.layerList.resize(mTextureCount+1);
	
	// double the first layer to fix the blendmap order
    defaultimp.layerList[0].worldSize = mSplattScaleX* 0.01f;
    defaultimp.layerList[0].textureNames.push_back("Splatting0"); 
    
    for (int i=0;i<mTextureCount;i++) {
		defaultimp.layerList[i+1].worldSize = mSplattScaleX*0.01f;
		defaultimp.layerList[i+1].textureNames.push_back("Splatting"+StringConverter::toString(i)); 		
	}    
};
void ArtifexLoader::loadOrCreateBlendMaps(bool saveBlendMaps)
{
	// 
	// *** TODO: load existing ones *** 
	//
	int maxLayer = mTextureCount;
	
	Ogre::Image blendMapImage[3];

	blendMapImage[0].load(mETBlendMapFileName[0],ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	blendMapImage[1].load(mETBlendMapFileName[1],ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	blendMapImage[2].load(mETBlendMapFileName[2],ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);	
	
	int blendImageNumber = 0;
	
	std::cout << "\nConverting ETM blendmaps to Ogre::Terrain blendmaps:\n";
	
	for(int i = 1;i <= maxLayer;i++)
    {
        Ogre::TerrainLayerBlendMap *blendMap;
        float *blendData;

        blendMap = mTerrain->getLayerBlendMap(i);
        blendData = blendMap->getBlendPointer();        
       
        std::cout << "Creating layer blendmap: " << i << "\n";

        for(int y = 0;y < mTerrain->getLayerBlendMapSize();y++)
        {
            for(int x = 0;x < mTerrain->getLayerBlendMapSize();x++)
            {
                Ogre::ColourValue blendColour = blendMapImage[blendImageNumber].getColourAt(x, y, 0);
                switch(i)
                {					
					case 1: case 4: case 7:
						{	*blendData = blendColour.r;
							break;
						}					
					case 2: case 5: case 8:
						{
							*blendData = blendColour.g;
							break;
						}
					case 3: case 6: case 9:
						{
							*blendData = blendColour.b;
							break;
						}			
                }               
                blendData++;
            }
        }
     
        blendMap->dirty(); 
        blendMap->update();

        if (i%3==0) blendImageNumber++;
        
        // save converted blendmaps
        if (saveBlendMaps) saveConvertedBlendMap(i,blendMap);
    }  
    
    std::cout << "\n";   
};
void ArtifexLoader::saveConvertedBlendMap(int blendMapNumber, Ogre::TerrainLayerBlendMap *blendMap) {
		Ogre::Image img;
		
		unsigned short *idata = OGRE_ALLOC_T(unsigned short, mTerrain->getLayerBlendMapSize() * mTerrain->getLayerBlendMapSize(), Ogre::MEMCATEGORY_RESOURCE);
		float scale = 65535.0f;
		
		for(unsigned int x = 0; x < mTerrain->getLayerBlendMapSize(); x++)
			for(unsigned int y = 0; y < mTerrain->getLayerBlendMapSize(); y++)
				idata[x + y * mTerrain->getLayerBlendMapSize()] = (unsigned short)(blendMap->getBlendValue(x, mTerrain->getLayerBlendMapSize() - y) * scale);
		
		img.loadDynamicImage((Ogre::uchar*)(idata), mTerrain->getLayerBlendMapSize(), mTerrain->getLayerBlendMapSize(), Ogre::PF_L16);
		
		std::string path = mZonePath + mZoneName + "/" + "OgreTerrainBlendMap" + Ogre::StringConverter::toString(blendMapNumber) + ".png";
		
		img.save(path);
		
		OGRE_FREE(idata, Ogre::MEMCATEGORY_RESOURCE);
};
void ArtifexLoader::loadGlobalColourMap() 
{
	if (!mTerrain->getGlobalColourMapEnabled())
	{
		mTerrain->setGlobalColourMapEnabled(true);		
		
		// Terrain Hack: Change global ColourMap Texture to auto generated MipMaps so you can use common JPG's
		OgreTerrainFix *terrainFix = (OgreTerrainFix*)mTerrain;			
		terrainFix->createFixedColourMap();	
		
		mTerrain->getGlobalColourMap()->loadImage(*mOverlay);	
		mTerrain->dirty();		
	}
};
#endif



