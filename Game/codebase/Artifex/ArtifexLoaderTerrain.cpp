/* ===========================================================================
Artifex Loader v0.90 beta

Freeware

Copyright (c) 2008 MouseVolcano (Thomas Gradl, Karolina Sefyrin,  Erik Biermann)

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
		
		for (int i=0; i<9; i++) {
			mTexturePath[i] = cfgfile.getSetting("Splatting"+StringConverter::toString(i));		
		}		
		
	} catch (Exception &e) {
		cout << "Error loading Terrain Settings: " << e.what() << ".\n";

	}
};

void ArtifexLoader::setupTextures() {
	String num = "";
	mTextureSize = 1024;
	
	for (int a=0;a<9;a++) {
		num = StringConverter::toString(a);
		mSplatting[a] = new Image();
		// mZonePath+mZoneName+"/"
		mSplatting[a]->load(mTexturePath[a],ETM_GROUP);

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

#ifndef ETM_TERRAIN
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
#else
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

	ET::TerrainInfo terrainInfo (mTerrainVertexCount, mTerrainVertexCount, vector<float>(mTerrainVertexCount * mTerrainVertexCount, 0.5f));
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
#endif



