/* ===========================================================================
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#include "SampleApp.h"

using namespace std;

int main()
{
	SampleApp ogreApp;

	try
	{
		ogreApp.Startup();

		while(!ogreApp.mShouldQuit)
		{
			ogreApp.Update();
		}
	}
	catch( Ogre::Exception& e )
	{
		cout << "#@$! An exception has occured: " << e.getFullDescription().c_str() << "\n";
		cout << "Press any key to continue...\n";
		char c;
		cin.get(c);
	}
	ogreApp.Shutdown();
	return 0;
}

