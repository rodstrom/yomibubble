/*
===========================================================================
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===========================================================================
*/
#include "SampleApp.h"

using namespace Ogre;
using namespace std;

SampleApp::SampleApp()
{
	mShouldQuit = false;
	mRenderWin = 0;
	mSceneMgr = 0;

	mLMouseDown = mMMouseDown = mRMouseDown = false;

	mRotateSpeed = 0.5f;
	mMoveSpeed = 320;
	mDirection = Vector3::ZERO;
	mPitch = 0.0f;
	mRotation = 0.0f;

	mLastTime = mTimer.getMilliseconds();

	mTimer1 = 0.0;
};

SampleApp::~SampleApp()
{
	delete mArtifexLoader;
};

void SampleApp::Startup()
{
	mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");

	if (!mRoot->restoreConfig())
	{
		mShouldQuit = !mRoot->showConfigDialog();
		if(mShouldQuit) return;
	}

	mRoot->saveConfig();
	mRoot->initialise(true, "ArtifexLoader 0.90 beta RC2");

	parseResources();

	mRenderWin = mRoot->getAutoCreatedWindow();

 	setupEmptyScene();

	loadInputSystem();

	Ogre::WindowEventUtilities::addWindowEventListener(mRenderWin, this);

	// init the loader with the path to your zones
	mArtifexLoader = new ArtifexLoader(mRoot, mSceneMgr, mCamNode, mCamera, "../media/");
	 // load a zone
	mArtifexLoader->loadZone("normaltest");
};


void SampleApp::Update()
{
	Root::getSingleton().renderOneFrame();

	Ogre::WindowEventUtilities::messagePump();
	InputManager::getSingletonPtr()->capture();

	// take time since last loop to adapt movement to system speed
	const float timeFactor = ((float)(mTimer.getMilliseconds()-mLastTime)/1000);

	mTimer1 -= timeFactor;



	if (mCamNode != NULL && mArtifexLoader->isZoneLoaded()) {
		// commit the camera movement & rotation
		mCamNode->translate(mCamera->getOrientation() * mDirection * (mMoveSpeed * timeFactor));

		mCamera->yaw( Degree(mRotation * mRotateSpeed* 50 *timeFactor) );
		mCamera->pitch( Degree(-mPitch * mRotateSpeed* 50 *timeFactor) );

		// normalise the camera orientation to avoid distortion of the perspective
		Quaternion q = mCamera->getOrientation();
		q.normalise();
		mCamera->setOrientation(q);
	}

	mLastTime = mTimer.getMilliseconds();
};

void SampleApp::setupEmptyScene() {

	// choose SceneManager
#ifndef ETM_TERRAIN
	mSceneMgr = mRoot->createSceneManager("TerrainSceneManager", "SampleAppSceneMgr");
#else
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "SampleAppSceneMgr");
#endif
	// set ilumination
	mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));

	// setup mCamera and viewport
	mCamera = mSceneMgr->createCamera("MainCam");
	mCamera->setPosition(500,500,500);

	Viewport* viewport = mRenderWin->addViewport(mCamera);
	viewport->setBackgroundColour(ColourValue(0.5, 0.5, 0.5));

	mCamera->setAspectRatio((float)viewport->getActualWidth() / (float) viewport->getActualHeight());

	mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("camNode");
	mCamNode->attachObject(mCamera);

	mCamera->setNearClipDistance( 0.2f );
};

void SampleApp::Shutdown()
{
	Root::getSingleton().shutdown();
};

void SampleApp::parseResources()
{
    ConfigFile cf;
	try
	{
		cf.load("resources.cfg");
	}
	catch( Exception& e )
	{
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		return;
	}

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
};

void SampleApp::loadInputSystem()
{
	mInputMgr = InputManager::getSingletonPtr();
    mInputMgr->initialise(mRenderWin);
    mInputMgr->addMouseListener(this, "SampleAppMouseListener");
	mInputMgr->addKeyListener(this, "SampleAppKeyListener");
};

bool SampleApp::mouseMoved(const OIS::MouseEvent &arg)
{
	if ( mRMouseDown )
	{
		mCamera->yaw( Degree(-arg.state.X.rel * mRotateSpeed) );
		mCamera->pitch( Degree(-arg.state.Y.rel * mRotateSpeed) );
	}

	return true;
};

bool SampleApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if ( id == OIS::MB_Left )
	{
		mLMouseDown = true;
	}
	else if ( id == OIS::MB_Middle )
	{
		mMMouseDown = true;
	}
	else if (id == OIS::MB_Right)
	{
		mRMouseDown = true;
	}
	return true;
};

bool SampleApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if ( id == OIS::MB_Left )
	{
		mLMouseDown = false;
	}
	else if ( id == OIS::MB_Middle )
	{
		mMMouseDown = false;
	}
	else if (id == OIS::MB_Right)
	{
		mRMouseDown = false;
	}
	return true;
};

bool SampleApp::keyPressed( const OIS::KeyEvent &arg )
{
	switch(arg.key)
	{
		case OIS::KC_UP:
		case OIS::KC_W:
			mDirection.z -= 1;
			if(mDirection.z<-1) mDirection.z=-1;
			break;
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mDirection.z += 1;
			if(mDirection.z>1) mDirection.z=1;
			break;
		case OIS::KC_LEFT:
		case OIS::KC_A:
			mRotation = 3.5f;
			break;
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mRotation = -3.5f;
			break;
	}

	return true;
};

bool SampleApp::keyReleased( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_ESCAPE)
		mShouldQuit = true;

	switch(arg.key)
	{
		case OIS::KC_UP:
		case OIS::KC_W:
			mDirection.z = 0;
			break;
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mDirection.z = 0;
			break;
		case OIS::KC_LEFT:
		case OIS::KC_A:
			mRotation = 0.0f;
			break;
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mRotation = 0.0f;
			break;
		case OIS::KC_U:
			if (mTimer1 > 0.0) break;
			mTimer1 = 1.0;
			mArtifexLoader->unloadZone();
#ifndef ETM_TERRAIN
			// recreate camnode because we used clearScene to remove the terrain.
			mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("camNode");
			mCamNode->attachObject(mCamera);
			mArtifexLoader->mCamNode = mCamNode;
#endif
			break;
		case OIS::KC_L:
			if (mTimer1 > 0.0) break;
			mTimer1 = 1.0;
			mArtifexLoader->loadZone("normaltest");
			break;
	}

	return true;
};

void SampleApp::windowMoved(RenderWindow* rw) {};

void SampleApp::windowResized(RenderWindow* rw)
{
	mInputMgr->setWindowExtents(rw->getWidth(), rw->getHeight());
};

void SampleApp::windowClosed(RenderWindow* rw)
{
	mShouldQuit = true;
};

void SampleApp::windowFocusChange(RenderWindow* rw) {};

