/* ===========================================================================
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

/*
	Comments:

	If you want to use ETM as TerrainManager, use ETM 2.3.1, and uncomment
	#define ETM_TERRAIN in the SampleApp.h and ArtifexLoader.h.
	You also have to add the include path for ETM library and EditableTerrainManager.lib
	to the linker options. EditableTerrainManager_d.lib for debug mode.

	We recommend to use Ogre's TSM unless you need or want to edit your terrain at runtime, to
	remove the extra dependency. The terrain shader works with both.
*/

#ifndef SAMPLEAPP_H
#define SAMPLEAPP_H

// include for the masks and Spawn struct, needed for ArtifexLoader
#include "CustomTypes.h"

// utility include, a good collection of small and handy timesavers:
#include "StaticUtils.h"

// uncomment this if you want to use ETM instead of TSM:
//#define ETM_TERRAIN

// includes needed to work with the Enhanced Terrain Manager
#ifdef ETM_TERRAIN
#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"
#endif
//

// include Ogre and the InputManager for OIS
#include "Ogre.h"
#include "InputManager.h"

// include the zoneloader
#include "ArtifexLoader.h"
class ArtifexLoader;
//

class SampleApp : public OIS::MouseListener, public OIS::KeyListener, public Ogre::WindowEventListener
{
public:
	float mTimer1;

	// *** ArtifexLoader Setup ***
	ArtifexLoader* mArtifexLoader;
	// *********************

#ifdef ETM_TERRAIN
	// *** ETM Setup ***
	ET::TerrainManager* mTerrainMgr;
    const ET::TerrainInfo* mTerrainInfo;
    ET::SplattingManager* mSplatMgr;
    // *****************
#endif

	// *** Basic Ogre Setup ***
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mRenderWin;
	Ogre::SceneManager* mSceneMgr;

	InputManager* mInputMgr;
	Ogre::SceneNode* mCamNode;
	Ogre::Camera* mCamera;

	unsigned long mLastTime;
	Ogre::Timer mTimer;

	void parseResources();

	void setupEmptyScene();

	void loadInputSystem();

	bool mShouldQuit;

	bool mLMouseDown, mMMouseDown, mRMouseDown;

	float mRotateSpeed, mMoveSpeed, mRotation, mPitch;

	Ogre::Vector3 mDirection;

	SampleApp();
	~SampleApp();

	void Startup();
	void Update();
	void Shutdown();

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );

	void windowMoved(Ogre::RenderWindow* rw);
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void windowFocusChange(Ogre::RenderWindow* rw);
	// *** End Basic Ogre Setup *** //

};
#endif

