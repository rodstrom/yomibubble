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
#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H

//#include "Ogre.h"

using namespace std;
using namespace Ogre;

namespace Artifex {

enum QueryFlags
{
   ENTITY_MASK = 1<<8,
   ROAD_MASK = 1<<9,
   INDIFFERENT_MASK = 1<<10,
   WATER_MASK = 1<<11,
   ENEMY_MASK = 1<<12,
};

enum {
	ST_ZERO,
	ST_STATIC,
	ST_MOVABLE,
	ST_MOBILE,
	ST_GROUNDCOVER,
};

typedef std::map<string,string> attributemap;

typedef struct {
	std::string name;
	std::string meshfile;
	float x;
	float y;
	float z;
	float ry;
	float rx;
	float rz;
	float sx;
	float sy;
	float sz;
	uint32 mask;
	std::string spawntype;
	int drop_to_ground;
	int render_static;
	attributemap attributes;
}Spawn2;

enum {
	STATE_NO_SCENE, // 0
	STATE_UI_LOADED, // 1
};

};

#endif

