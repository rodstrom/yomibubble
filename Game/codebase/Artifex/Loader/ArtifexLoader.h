/* ===========================================================================
Artifex Loader v1.0 RC1 OT beta

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
#ifndef ARTIFEXLOADER_H
#define ARTIFEXLOADER_H

#pragma warning (disable : 4530)

#include <cstdlib>
#include <iostream>

// include Ogre
//#include "Ogre.h"

// include for the masks and Spawn struct, needed for ArtifexLoader
#include "CustomTypes.h"

// utility include, a good collection of small and handy timesavers:
#include "StaticUtils.h"

// uncomment this if you want to use ETM:
// #define ETM_TERRAIN

// uncomment this if you want to use Ogre's TerrainSceneManager:
// note: TSM only works up to Ogre Version 1.7
// #define TSM_TERRAIN

// uncomment this if you want to use Ogre's::Terrain component (Ogre 1.7 and higher)
//#define OT_TERRAIN
#define OT_TERRAIN

// includes needed to work with the Enhanced Terrain Manager
#ifdef ETM_TERRAIN
//#include "ETTerrainManager.h"
//#include "ETTerrainInfo.h"
//#include "ETBrush.h"
//#include "ETSplattingManager.h"
#endif
//

// includes needed to work with Ogre::Terrain
#ifdef OT_TERRAIN
//#include "OgreTerrain.h"
#include "..\OgreTerrainFix.h"
//#include "OgreTerrainGroup.h"
#include "..\TerrainMaterialGeneratorC.h"
#endif

// enable grass loading
#define PG_GRASSLOADER

#ifdef PG_GRASSLOADER
#include "ArtifexLoaderPGGrass.h"
class ArtifexLoaderPGGrass;
#endif

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
// ET::TerrainManager* mTerrainMgr;
// ET::TerrainInfo* mTerrainInfo;
// ET::SplattingManager* mSplatMgr;
/*
void loadTerrain();
void loadBinaryTerrain();
void loadImgTerrain();
void initETM();
void updateLightmap();
*/

#elif defined(TSM_TERRAIN)
// TSM
Image mCoverage[3];
TexturePtr mCoverageTex[3];

void initTSM();
void loadTerrain();

#elif defined(OT_TERRAIN)
// Ogre::Terrain
Ogre::TerrainGlobalOptions* mTerrainGlobals;
Ogre::TerrainGroup* mTerrainGroup;
Ogre::TerrainMaterialGeneratorPtr mMTerrMatGen;
Ogre::Terrain* mTerrain;

// init
void initOgreTerrain();
void configureTerrainDefaults(Ogre::Light* light);

// terrain loaders
void loadTerrain();
void loadBinaryTerrain();
void loadImgTerrain();	

// blendmap stuff
void loadOrCreateBlendMaps(bool saveBlendMaps=false);
void saveConvertedBlendMap(int blendMapNumber, Ogre::TerrainLayerBlendMap *blendMap);

// colourmap loader
void loadGlobalColourMap();

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

#ifdef PG_GRASSLOADER
ArtifexLoaderPGGrass *mArtifexLoaderPGGrass;
#endif

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

// *** ColourMap
Image* mOverlay;
TexturePtr mOverlayTex;

Image* mSplatting[9];
string mTexturePath[9];
TexturePtr mSplattingTex[9];
string mETBlendMapFileName[3];

Image* mHeightMapImage;
string mHeightMapImageFileName;
string mHeightMapBinarayFileName;

bool mLoadTerrainBinary;


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
void createGrass();

void loadLightSettings();
void createLight();

string mZonePath;
string mZoneName;

bool mLightmap;
float mLightmapShadow;

bool mLoadGrass;

std::vector <Spawn2> mObjectFile;
std::vector <Spawn2> mObjectTrashCan;

float getMeshYAdjust(Ogre::String meshfile);

void spawnLoader(String which);
void staticsLoader(String which, float renderDistance);

bool loadZone(std::string zonename, bool use_cfg_settings=true, bool fog=true, bool water=true, bool skye=true, bool light=true, bool grass=true, bool statics=true, bool groundcover=true, bool movables=true, bool mobiles=true);
void unloadZone();

void loadZoneCFG();
};
#endif