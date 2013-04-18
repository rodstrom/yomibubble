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
#ifndef STATICUTILS_H
#define STATICUTILS_H

//#include "Ogre.h"
#include "CustomTypes.h"

using namespace Ogre;
using namespace std;

namespace Artifex {

static int round(float val) {
	int result = 0;
	int intval = Math::Floor(val);
	float dif = val - intval;
	if (dif >= 0.5f) result = intval ++;
	return intval;
};
static float rnd(float start, float end) {
	float result = 0;
	float range = end - start;	
	result = start + (rand()*range);
	return result;
};
static int rndi(float start, float end) {
	int result=0;
	result = round(rnd (start, end));
	return result;
};

static void CreateImage(Image &img, int width, int height, PixelFormat pf, bool setColour=false, uchar R=0, uchar G=0, uchar B=0 ) {
	uchar *pImage = new uchar[width * height * 4];
	uchar r,g,b;
	
	if (setColour) {
		for (int a=0; a< (width*height*4)-3; a+=4 ) {
			r = R + rndi(0,80)-30;	
			r = std::min<uchar>(255,std::max<uchar>(0,r));

			g = G + rndi(0,80)-20;	
			g = std::min<uchar>(255,std::max<uchar>(0,g));
			
			b = B + rndi(0,10)-10;	
			b = std::min<uchar>(255,std::max<uchar>(0,b));
			
			if (r>g-10) r=g-10;
			if (r<0) r=0;
			
			pImage[a] = r;
			pImage[a+1] = g;
			pImage[a+2] = b;
			pImage[a+3] = (uchar)255;
		}
	}
    img.loadDynamicImage(pImage, width, height, 4, pf, true);   
};

static void flipFArray(float* arr, size_t x, size_t y,FlipOptions options=REVERSE) {
	float f=0.0f;
	switch(options) {
		case REVERSE: {
				for (int i=0;i<(int)((x*y)/2);i++){
					f=*(arr+i);
					*(arr+i)=*(arr+x*y-i-1);
					*(arr+x*y-i-1)=f;
				}
			}
			break;
		/* TODO: fix minor calculation error
		case FLIPY: {
				for (int u=0;u<x;u++) {
					for (int v=0;v<(int)y/2;v++) {
						f=*(arr+v*y+u);
						*(arr+v*y+u) = *(arr+x*y-(v*y+u)-1);
						*(arr+x*y-(v*y+u)-1)=f;
					}
				}
			}
			break;
		*/ 
		case FLIPX: {
				for (int v=0;v<y;v++) {
					for (int u=0;u<(int)x/2;u++) {
						f=*(arr+v*y+u);
						*(arr+v*y+u) = *(arr+v*y+x-u-1);
						*(arr+v*y+x-u-1)=f;
					}
				}
			}
			break;
	}// end switch
};

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
