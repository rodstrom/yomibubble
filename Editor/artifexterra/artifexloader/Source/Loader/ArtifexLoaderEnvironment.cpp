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
#include "ArtifexLoader.h"

using namespace std;
using namespace Ogre;
using namespace Artifex;

void ArtifexLoader::createSkye()
{
	try {
		mSceneMgr->setSkyBox(true, mSkyBoxName , mSkyRadius);
	} catch (Exception &e) {

	};
};

void ArtifexLoader::createFog()
{
		ColourValue fadeColour(mFogR, mFogG, mFogB);
		mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, mFogDensity, mFogStart,mFogEnd);
};

void ArtifexLoader::createWater()
{

	Plane mWaterPlane;

	mWaterPlane.normal = Vector3::UNIT_Y;

	MeshManager::getSingleton().createPlane(
		"WaterPlane",
		ETM_GROUP,
		mWaterPlane,
		mWaterWidth, mWaterLength,
		20, 20,
		true, 1,
		10, 10,
		Vector3::UNIT_Z);

	mWaterEntity = mSceneMgr->createEntity("water", "WaterPlane");
	mWaterEntity->setQueryFlags(WATER_MASK);
	mWaterEntity->setMaterialName(mWaterMaterial);

	mWaterNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode");

	mWaterNode->attachObject(mWaterEntity);
	mWaterNode->setPosition(mWaterXPos, mWaterYPos, mWaterZPos);
};

void ArtifexLoader::createLight() {
	mSceneMgr->setAmbientLight(ColourValue(mAmbientR, mAmbientG, mAmbientB));

	mLight = mSceneMgr->createLight( "Light" );
	mLight->setType( Ogre::Light::LT_POINT );
	mLight->setPosition(mLightPosX, mLightPosY, mLightPosZ);

	mLight->setCastShadows(true);
	mLight->setPowerScale(0.5f);
};

void ArtifexLoader::loadGrassSettings()
{
	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mLoadGrass = strToI(cfgfile.getSetting("LoadGrass"))==1?true:false;

		mGrassCoverMap=cfgfile.getSetting("GrassCoverMap");
		if (mGrassCoverMap=="") mGrassCoverMap="ETcoverage.0.png";

		mGrassCoverChannel=cfgfile.getSetting("GrassCoverChannel");
		if (mGrassCoverChannel=="") mGrassCoverChannel = "R";

		mGrassColourMap=cfgfile.getSetting("GrassColourMap");
		if (mGrassColourMap=="") mGrassColourMap = "ETbase.png";

		mGrassMaterial=cfgfile.getSetting("GrassMaterial");
		if (mGrassMaterial=="") mGrassMaterial = "pg_grass";

		mGrassDensity=Ogre::StringConverter::parseReal(cfgfile.getSetting("GrassDensity"));
		if (cfgfile.getSetting("GrassDensity")=="") mGrassDensity=0.5f;

		mGrassRange=Ogre::StringConverter::parseReal(cfgfile.getSetting("GrassRange"));
		if (cfgfile.getSetting("GrassRange")=="") mGrassRange=500;

		mGrassAnimate=strToI(cfgfile.getSetting("GrassAnimate"))==1?true:false;

	} catch (Exception &e) {
		std::cout << "Troubles loading grass settings: taking defaults.\n";

		mLoadGrass = false;
		mGrassCoverChannel="G";
		mGrassColourMap="ETbase.png";
		mGrassMaterial="pg_grass";
		mGrassAnimate=true;
		mGrassDensity=1.5f;
		mGrassRange=150.0f;
	}
};

void ArtifexLoader::loadLightSettings()
{
	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mLightPosX = strToF(cfgfile.getSetting("Light_X"));
		if (cfgfile.getSetting("Light_X")=="") mLightPosX=5000;

		mLightPosY = strToF(cfgfile.getSetting("Light_Y"));
		if (cfgfile.getSetting("Light_Y")=="") mLightPosY=5000;

		mLightPosZ = strToF(cfgfile.getSetting("Light_Z"));
		if (cfgfile.getSetting("Light_Z")=="") mLightPosZ=5000;

	} catch (Exception &e) {
		std::cout << "Troubles loading pointlight settings: taking defaults.\n";
		mLightPosX = 5000;
		mLightPosY = 5000;
		mLightPosZ = 5000;
	}

	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mAmbientR = strToF(cfgfile.getSetting("Ambient_Light_R"));
		if (cfgfile.getSetting("Ambient_Light_R")=="") mAmbientR=0.5f;

		mAmbientG = strToF(cfgfile.getSetting("Ambient_Light_G"));
		if (cfgfile.getSetting("Ambient_Light_G")=="") mAmbientG=0.5f;

		mAmbientB = strToF(cfgfile.getSetting("Ambient_Light_B"));
		if (cfgfile.getSetting("Ambient_Light_B")=="") mAmbientB=0.5f;

	} catch (Exception &e) {
		std::cout << "Troubles loading ambient-scene-light settings: taking defaults.\n";
		mAmbientR = 0.5f;
		mAmbientG = 0.5f;
		mAmbientB = 0.5f;
	}
};

void ArtifexLoader::loadFogSettings()
{
	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mFogR = strToF(cfgfile.getSetting("FogR"));
		if (cfgfile.getSetting("FogR")=="") mFogR=0.7f;

		mFogG = strToF(cfgfile.getSetting("FogG"));
		if (cfgfile.getSetting("FogG")=="") mFogG=0.76f;

		mFogB = strToF(cfgfile.getSetting("FogB"));
		if (cfgfile.getSetting("FogB")=="") mFogR=0.9f;

		mFogDensity = strToF(cfgfile.getSetting("FogDensity"));
		if (cfgfile.getSetting("FogDensity")=="") mFogDensity=0.0001f;

		mFogStart = strToF(cfgfile.getSetting("FogStart"));
		if (cfgfile.getSetting("FogStart")=="") mFogStart=1000.0f;

		mFogEnd = strToF(cfgfile.getSetting("FogEnd"));
		if (cfgfile.getSetting("FogEnd")=="") mFogStart=6000.0f;

	} catch (Exception &e) {
		std::cout << "Troubles loading fog settings: taking defaults.\n";
		mFogR = 0.7f;
		mFogG = 0.76f;
		mFogB = 0.9f;
		mFogDensity = 0.0001f;
		mFogStart = 1000.0f;
		mFogEnd = 6000.0f;
	}
};

void ArtifexLoader::loadWaterSettings() {
	try {
		ConfigFile cfgfile;
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mWaterVisible = (strToI(cfgfile.getSetting("WaterVisible"))==1)?true:false;
		if (cfgfile.getSetting("WaterVisible")=="") mWaterVisible=1;

		mWaterXPos = strToI(cfgfile.getSetting("WaterXPos"));
		if (cfgfile.getSetting("WaterXPos")=="") mWaterXPos=5000.0f;

		mWaterYPos = strToI(cfgfile.getSetting("WaterYPos"));
		if (cfgfile.getSetting("WaterYPos")=="") mWaterYPos=700.0f;

		mWaterZPos = strToI(cfgfile.getSetting("WaterZPos"));
		if (cfgfile.getSetting("WaterZPos")=="") mWaterZPos=5000.0f;

		mWaterWidth = strToI(cfgfile.getSetting("WaterWidth_X"));
		if (cfgfile.getSetting("WaterWidth_X")=="") mWaterWidth=30000.0f;

		mWaterLength = strToI(cfgfile.getSetting("WaterWidth_Z"));
		if (cfgfile.getSetting("WaterWidth_Z")=="") mWaterLength=30000.0f;

		mWaterMaterial = cfgfile.getSetting("WaterMaterial");
		if (cfgfile.getSetting("WaterMaterial")=="") mWaterMaterial="OUTMaterials/Water.02";

	} catch (Exception &e) {
		cout << "Troubles loading water settings: taking defaults.\n";
		mWaterVisible = true;
		mWaterXPos = 5000.0f;
		mWaterYPos = 720.0f;
		mWaterZPos = 5000.0f;
		mWaterWidth = 30000.0f;
		mWaterLength = 30000.0f;
		mWaterMaterial = "OUTMaterials/Water.02";
	}
};

void ArtifexLoader::loadSkyeSettings() {
	Ogre::ConfigFile cfgfile;
	try {
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");
		mSkyBoxName = cfgfile.getSetting("Sky");
		if (mSkyBoxName=="") mSkyBoxName="_MySky";

		mSkyRadius = strToF(cfgfile.getSetting("SkyRadius"));
		if (cfgfile.getSetting("SkyRadius")=="")  mSkyRadius=2300.0f;

	} catch (Exception &e) {
		cout << "Troubles loading skye settings: taking defaults.\n";
		mSkyBoxName = "_MySky";
		mSkyRadius = 2300.0f;
	}
};

void ArtifexLoader::loadCameraSettings() {
	Ogre::ConfigFile cfgfile;
	try {
		cfgfile.load(mZonePath+mZoneName+"/zonesettings.cfg");

		mCamX = strToF(cfgfile.getSetting("Camera_X"));
		mCamZ = strToF(cfgfile.getSetting("Camera_Z"));
		mCamY = strToF(cfgfile.getSetting("Camera_Y"));

		mFarClipDistance = strToF(cfgfile.getSetting("Camera_FarClipDistance"));


	} catch (Exception &e) {
		cout << "Troubles loading camera settings. - No problem DON'T PANIC.\n";
	}
};
