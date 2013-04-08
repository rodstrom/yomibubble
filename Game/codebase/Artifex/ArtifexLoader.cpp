/* ===========================================================================
Artifex Loader v0.90 beta

Freeware

Copyright (c) 2008 MouseVolcano (Thomas Gradl, Erik Biermann, Karolina Sefyrin)

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

ArtifexLoader::ArtifexLoader(Root *root, SceneManager *scenemgr, SceneNode *camnode, Camera *camera, string zonepath)
{
	mZonePath = zonepath;

	mRoot = root;
	mSceneMgr = scenemgr;

	mCamNode = camnode;
	mCamera = camera;

	mZoneLoaded = false;
	
	ResourceGroupManager::getSingleton().createResourceGroup(ETM_GROUP);

	mDBManager = new DBManager(this);
};

ArtifexLoader::~ArtifexLoader()
{

};

#ifndef ETM_TERRAIN
float ArtifexLoader::getHeightAt(const float &x, const float &z) {
	float y=0.0f;

	static Ray updateRay;

	updateRay.setOrigin(Vector3(x,9999,z));
	updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);

	RaySceneQuery *mTSMRaySceneQuery =  mSceneMgr->createRayQuery(updateRay);
	RaySceneQueryResult& qryResult = mTSMRaySceneQuery->execute();

	RaySceneQueryResult::iterator i = qryResult.begin();
	if (i != qryResult.end() && i->worldFragment)
	{
		y=i->worldFragment->singleIntersection.y;
	}
	delete mTSMRaySceneQuery;
	return y;
};
#else
float ArtifexLoader::getHeightAt(const float &x, const float &z) {
	return mTerrainInfo->getHeightAt(x,z);
};
#endif
bool ArtifexLoader::loadZone(string zonename, bool use_cfg_settings, bool fog, bool water, bool skye, bool light, bool statics, bool groundcover, bool movables, bool mobiles)
{
	mZoneName = zonename;

	cout << "\n|= ArtifexTerra3D =| Zoneloader 0.90: adding " << zonename.c_str() << " to resources.\n\n ";
	ResourceGroupManager::getSingleton().addResourceLocation(mZonePath+zonename,"FileSystem",ETM_GROUP);
	ResourceGroupManager::getSingleton().initialiseResourceGroup(ETM_GROUP);

	loadZoneCFG();

#ifdef ETM_TERRAIN
	initETM();
#else
	initTSM();
#endif

	createColourLayer();
	setupTextures();

	if (!use_cfg_settings) {
		if (water) createWater();
		if (skye) createSkye();
		if (fog) createFog();
		if (light) createLight();

	} else {
		createLight();
		createSkye();
		createFog();
		if (mWaterVisible==1) createWater();
	}

    loadTerrain();

	string dbpath = mZonePath;
	str_replace(dbpath,'/','\\');

	if (mLoadObjectsFrom=="SQL" && fileExists(dbpath+mZoneName+"\\objects.s3db")) {
		mDBManager->Open(dbpath+mZoneName+"\\objects.s3db");
		mDBManager->Load();
		mDBManager->Close();
	} else {
		spawnLoader("static");
		spawnLoader("groundcover");
		spawnLoader("mobile");
		spawnLoader("movable");
	}

	mCamNode->setPosition(mCamX,mCamY,mCamZ);
	mCamera->pitch( Degree(-5) );
	mCamera->yaw( Degree(75) );

	mZoneLoaded = true;
	cout << "Done loading " << mZoneName.c_str() << ".\n";
	return true;
};
bool ArtifexLoader::isZoneLoaded() {
	return mZoneLoaded;
};
void ArtifexLoader::unloadZone()
{

	mZoneLoaded = false;
	
	try {
		StaticGeometry* staticGeometry = mSceneMgr->getStaticGeometry((string)mZoneName+"static");
		staticGeometry->destroy();
		mSceneMgr->destroyStaticGeometry(staticGeometry);
	} catch (...) {};

	try {
		StaticGeometry* staticGeometry = mSceneMgr->getStaticGeometry((string)mZoneName+"groundcover");
		staticGeometry->destroy();
		mSceneMgr->destroyStaticGeometry(staticGeometry);
	} catch (...) {};

	std::string spawntype = "";
	std::vector <Spawn2> *tmpSpawns;

	SceneNode *tmpNode = NULL;

	tmpSpawns = &mObjectFile;

	std::vector<Spawn2>::iterator tmpIter;

	for (tmpIter = tmpSpawns->begin(); tmpIter != tmpSpawns->end(); ++tmpIter)
	{
		Spawn2 ts = *tmpIter;

		try {
			tmpNode = mSceneMgr->getSceneNode(ts.name+"Node");
			//cout <<"Found "<< ts.name.c_str() <<". \n";
		} catch (Exception &e) {
			cout << "\n|= ArtifexTerra3D =| Zoneloader 0.90: Error - " << e.getDescription().c_str() << "\n";
		};

		if (tmpNode != NULL) {
			try {
				tmpNode->detachAllObjects();
				mSceneMgr->destroySceneNode(tmpNode->getName());
				mSceneMgr->destroyEntity(ts.name);
			} catch(...) {
			};
		}
		tmpNode = NULL;
	}

	tmpSpawns->clear();

/*
	mArtifex->mGrassUpdate = false;
	if (mArtifex->mGrassMgr!=NULL) {
		mArtifex->mGrassMgr->unload();
	}
*/
#ifdef ETM_TERRAIN
	// destory ETM Terrain
	try {
		mTerrainMgr->destroyTerrain();
	} catch (Exception &e) {};

	try {
		mSplatMgr->~SplattingManager();
	} catch (Exception &e) {};
#endif

	try {
		mWaterNode->detachAllObjects();
		mSceneMgr->destroySceneNode("WaterNode");
		mSceneMgr->destroyEntity("water");
	} catch (Exception &e) {};

	try {
		mSceneMgr->setSkyBox(false, mSkyBoxName);
	} catch (Exception &e) {};

	try {
	    mSceneMgr->destroyAllLights();
	} catch (Exception &e) {};

#ifndef ETM_TERRAIN
	//destroy TSM Terrain
	mSceneMgr->clearScene();
#endif

	ResourceGroupManager::getSingleton().destroyResourceGroup(ETM_GROUP);

	mObjectTrashCan.clear();
	cout << "Done unloading " << mZoneName.c_str() << ".\n";
};

void ArtifexLoader::loadZoneCFG()
{
	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");
		mZoneVersion = cfgfile.getSetting("LoaderVersion");
		if (mZoneVersion=="") mZoneVersion = "0.80";
		mLoadObjectsFrom = cfgfile.getSetting("LoadObjectsFrom");
		if (mLoadObjectsFrom=="") mLoadObjectsFrom="CFG";
	} catch (Exception &e) {
		std::cout << "Troubles loading loader settings: taking defaults.\n";
		mZoneVersion = "unknown";
		mLoadObjectsFrom = "CFG";
	}
	loadTerrainSettings();
	loadCameraSettings();
	loadLightSettings();
	loadFogSettings();
	loadGrassSettings();
	loadWaterSettings();
	loadSkyeSettings();
};
