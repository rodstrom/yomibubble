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
#ifndef STATICUTILS_H
#define STATICUTILS_H

#include "Ogre.h"
#include "CustomTypes.h"

using namespace Ogre;
using namespace std;

namespace Artifex {

static std::string iToStr(int val) {
	return Ogre::StringConverter::toString(val);
};

static std::string fToStr(float val, int precision=6) {
	std::ostringstream strstr;
	strstr << std::fixed << std::setprecision(precision) << val;
	return strstr.str();
};

static std::string trim(std::string str) {
	Ogre::StringUtil::trim(str);
	return str;
};

static bool isnum(const std::string &str) {
	return Ogre::StringConverter::isNumber(str);
};

static float strToF(const std::string &str) {
	return Ogre::StringConverter::parseReal(str);
};

static int strToI(const std::string &str) {
	return Ogre::StringConverter::parseInt(str);
};

static void str_replace(std::string &str, const char what, const char with ) {
	std::replace(str.begin(), str.end(), what, with);
};

static void initSpawn(Spawn2& spawn) {
    spawn.name = "";
    spawn.meshfile = "";
	spawn.x = 0.0f;
	spawn.y = 0.0f;
	spawn.z = 0.0f;
	spawn.rx = 0.0f;
	spawn.ry = 0.0f;
	spawn.rz = 0.0f;
	spawn.sx = 0.0f;
	spawn.sy = 0.0f;
	spawn.mask = 0;
	spawn.spawntype = "generic";
	spawn.drop_to_ground = 0;
	spawn.render_static = 0;
	spawn.attributes.clear();
};

static bool fileExists (const string &path) {
	FILE *fp = fopen(path.c_str(),"r");
	if( fp ) {
		// exists
		fclose(fp);
	} else {
		// doesnt exist
		return false;
	}
	return true;
};

};

#endif
