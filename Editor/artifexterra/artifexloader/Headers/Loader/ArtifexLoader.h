/* ===========================================================================
Artifex Loader v0.90 RC2 beta

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
#pragma once
#ifndef ARTIFEXLOADER_H
#define ARTIFEXLOADER_H

#pragma warning (disable : 4530)

#include <cstdlib>
#include <iostream>

// include Ogre
#include "Ogre.h"

// include for the masks and Spawn struct, needed for ArtifexLoader
#include "CustomTypes.h"

// utility include, a good collection of small and handy timesavers:
#include "StaticUtils.h"

// uncomment this if you want to use ETM instead of TSM:
// #define ETM_TERRAIN

// includes needed to work with the Enhanced Terrain Manager
#ifdef ETM_TERRAIN
#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"
#endif
//

// include the Database Manager for SQLite support
#include "DBManager.h"
class DBManager;

#define ETM_GROUP "ETM" // the resourcegroup for the zone

using namespace std;
using namespace Ogre;
using namespace Artifex;


class ArtifexLoader {
public:
	ArtifexLoader(Root *root, SceneManager *scenemgr, SceneNode *camnode, Camera *camera, string zonepath="");
	~ArtifexLoader();

	Root *mRoot;
	SceneManager *mSceneMgr;

	bool isZoneLoaded();
	bool mZoneLoaded;

	float getHeightAt(const float &x, const float &z);

#ifdef ETM_TERRAIN
	// ETM
	ET::TerrainManager* mTerrainMgr;
    ET::TerrainInfo* mTerrainInfo;
    ET::SplattingManager* mSplatMgr;

	void loadTerrain();
	void loadBinaryTerrain();
	void loadImgTerrain();
	void initETM();
	void updateLightmap();
#else
	Image mCoverage[3];
	TexturePtr mCoverageTex[3];
	// TSM
	void initTSM();
	void loadTerrain();
#endif

	// SQLite
	DBManager *mDBManager;

	string mZoneVersion;
	string mLoadObjectsFrom;

	//grass
	string mGrassCoverMap;
	string mGrassCoverChannel;
	string mGrassColourMap;
	string mGrassMaterial;
	bool mGrassAnimate;
	float mGrassDensity;
	float mGrassRange;

	// fog
	float mFogR;
	float mFogG;
	float mFogB;
	float mFogDensity;
	float mFogStart;
	float mFogEnd;
	bool mFogVisible;

	// skye
	string mSkyBoxName;
	float mSkyRadius;

	// camera
	SceneNode *mCamNode;
	Camera *mCamera;
	float mFarClipDistance;
	float mCamX;
	float mCamY;
	float mCamZ;

	//light
	Light *mLight;
	float mLightPosX;
	float mLightPosY;
	float mLightPosZ;
	float mAmbientR;
	float mAmbientG;
	float mAmbientB;

	// water
	int mWaterVisible;
	float mWaterXPos;
	float mWaterYPos;
	float mWaterZPos;
	float mWaterWidth;
	float mWaterLength;
	string mWaterMaterial;
	SceneNode* mWaterNode;
	Entity *mWaterEntity;

	// terrain
	bool mTerrainLoD;

	float mTerrainPixelError;
	float mTerrainLoDMorphingFactor;

	int mTerrainMaxLoD;

	int mTerrainTileSize;
	int mTerrainVertexCount;

	bool mTerrainVertexNormals;
	bool mTerrainVertexTangents;

	// *** terrainDimensions
	float mTerrX, mTerrZ, mTerrY;

	float mSplattScaleX;
	float mSplattScaleZ;
	int mTextureCount;

	Image* mOverlay;
	TexturePtr mOverlayTex;

	Image* mSplatting[9];
	String mTexturePath[9];
	TexturePtr mSplattingTex[9];

	int mTextureSize;
	int mCoverMapSize;

	void setupTextures();

	void loadTerrainSettings();

	void createColourLayer();

	void loadCameraSettings();

	void loadWaterSettings();
	void createWater();

	void loadSkyeSettings();
	void createSkye();

	void loadFogSettings();
	void createFog();

	void loadGrassSettings();

	void loadLightSettings();
	void createLight();

	string mZonePath;
	string mZoneName;

	bool mLightmap;
	float mLightmapShadow;

	bool mLoadGrass;

	vector <Spawn2> mObjectFile;
	vector <Spawn2> mObjectTrashCan;

	float getMeshYAdjust(Ogre::String meshfile);

	void spawnLoader(String which);
	void staticsLoader(String which, float renderDistance);

	bool loadZone(std::string zonename, bool use_cfg_settings=true, bool fog=true, bool water=true, bool skye=true, bool light=true, bool statics=true, bool groundcover=true, bool movables=true, bool mobiles=true);
	void unloadZone();

	void loadZoneCFG();
};
#endif
