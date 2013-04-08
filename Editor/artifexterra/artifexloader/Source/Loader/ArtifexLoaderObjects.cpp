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

void ArtifexLoader::spawnLoader(string which)
{
	string meshName="", meshFile="", xPos="", yPos="", zPos="", rot="", tmp="",
		typeName="", entName="", scale="", rotX="", rotZ="";

	int counter=1;
	bool fileend = false;

	string filename = mZonePath+mZoneName+"/spawns_" + which + ".cfg";

	std::ifstream fp;

	fp.open(filename.c_str(), std::ios::in | std::ios::binary);
	if(!fp) return;

	DataStreamPtr stream(new FileStreamDataStream(filename, &fp, false));

	string line;

	do
	{
		for (int a=0;a<=10;a++)
		{
			line = stream->getLine();
			if(stream->eof()) {
				fileend = true;
				break;
			}
			tmp = line;
			line = "";
			StringUtil::trim(tmp);

			if (StringUtil::startsWith(tmp,"//",false) || tmp.empty())
			{
				a--;
				continue;
			}
			else {
				switch (a)
				{
					case 0:
						meshName = tmp;
						break;
					case 1:
						// bracket
						break;
					case 2:
						meshFile = tmp;
						break;
					case 3:
						xPos = tmp;
						break;
					case 4:
						yPos = tmp;
						break;
					case 5:
						zPos = tmp;
						break;
					case 6:
						rot = tmp;
						break;
					case 7:
						scale = tmp;
						break;
					case 8:
						rotX = tmp;
						break;
					case 9:
						rotZ = tmp;
						break;
					case 10:
						// bracket close
						break;
					default:
						break;
				}
				tmp = "";
			}
		}
		if (fileend) break;
		{
			//***************************
			entName = meshName;
			//***************************
			Entity *newModel = NULL;
			lCreateEntity:
			try {
				newModel = mSceneMgr->createEntity((string) entName, (string) meshFile);
			} catch(Exception &e) {
				if (e.getNumber() != 4) {
					cout << "Troubles creating Entity " << entName.c_str() << ".\n";
				} else if (e.getNumber() == 4) {
					cout << "Entity with the name " << entName.c_str() << " already exists, creating a random name:";
					entName = "Entity"+StringConverter::toString((int)Math::RangeRandom(100000,999999));
					cout << entName.c_str() << ".\n";
					goto lCreateEntity;
				}
				break;
			};
			newModel->setQueryFlags(ENTITY_MASK);
			//****************************
			float x=0,y=0,z=0,r=0,s=0,rx=0,rz=0;
			x = strToF(xPos);
			y = strToF(yPos);
			z = strToF(zPos);

			r = strToF(rot);
			s = strToF(scale);
			rx = strToF(rotX);
			rz = strToF(rotZ);

			Spawn2 spawn;

			spawn.name = entName;
			spawn.meshfile = meshFile;
			spawn.x = x;
			spawn.y = y;
			spawn.z = z;
			spawn.ry = r;
			spawn.sx=spawn.sy=spawn.sz = s;
			spawn.rx = rx;
			spawn.rz = rz;
			spawn.spawntype = which;

			if (which == "static")
				spawn.render_static = true;
			else if (which == "groundcover")
				spawn.drop_to_ground = true;

			// auto drop models on terrain
			if (y==0 || spawn.drop_to_ground) {
				float tmpY = getMeshYAdjust(meshFile)* s;
				// y = (float) mArtifex->mTerrainInfo->getHeightAt(x,z)+tmpY;
				y = getHeightAt(x,z)+tmpY;
				spawn.y = y;
			}

			mObjectFile.push_back(spawn);

			//****************************
			{
				SceneNode *mNode = NULL;
				try {
					mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(entName+"Node",Ogre::Vector3(x,y,z),Quaternion ((Degree(r)), Vector3::UNIT_Y));
					mNode->attachObject(newModel);
					mNode->setScale(s,s,s);
					mNode->setOrientation(Quaternion ((Degree(rx)), Vector3::UNIT_X)*Quaternion ((Degree(r)), Vector3::UNIT_Y)*Quaternion ((Degree(rz)), Vector3::UNIT_Z));
				} catch (Exception &e) {
					cout << "Troubles creating Node" << entName.c_str() << "Node: " << e.what() << "\n";
				};
			}
			//***************************
		}
		counter ++;
	} while (!stream->eof() && !StringUtil::startsWith(tmp,"[END]",false));

}

void ArtifexLoader::staticsLoader(Ogre::String which, float renderDistance)
{
	Ogre::String meshName="", meshFile="", xPos="", yPos="", zPos="", rot="", tmp="",
		typeName="", entName="", scale="", rotX="", rotZ="";

	int counter=1;

	if (which.empty()) typeName = "static";
	else typeName = which;

	StaticGeometry *staticGeometry;
	staticGeometry = mSceneMgr->createStaticGeometry((string)mZoneName+typeName);
	staticGeometry->setRenderingDistance(renderDistance);

	Ogre::String filename = mZonePath+mZoneName+"/spawns_" + typeName + ".cfg";

	std::ifstream fp;

	fp.open(filename.c_str(), std::ios::in | std::ios::binary);
	if(!fp) return;

	DataStreamPtr stream(new FileStreamDataStream(filename, &fp, false));

	Ogre::String line;

	do {
		for (int a=0;a<=10;a++)
		{
			line = stream->getLine();
			if(stream->eof()) break;
			tmp = line;
			line = "";
			Ogre::StringUtil::trim(tmp);

			if (Ogre::StringUtil::startsWith(tmp,"//",false) || tmp.empty())
			{
				a--;
				continue;
			}
			else {
				switch (a)
				{
					case 0:
						meshName = tmp;
						break;
					case 1:
						// bracket
						break;
					case 2:
						meshFile = tmp;
						break;
					case 3:
						xPos = tmp;
						break;
					case 4:
						yPos = tmp;
						break;
					case 5:
						zPos = tmp;
						break;
					case 6:
						rot = tmp;
						break;
					case 7:
						scale = tmp;
						break;
					case 8:
						rotX = tmp;
						break;
					case 9:
						rotZ = tmp;
						break;
					case 10:
						// bracket close
						break;
					default:
						break;
				}
				tmp = "";
			}
		}
		try {
			//***************************
			entName = meshName;
			//***************************
			Entity *newModel = mSceneMgr->createEntity((string) entName, (string) meshFile);
			newModel->setQueryFlags(ENTITY_MASK);
			//****************************
			float x=0,y=0,z=0,r=0,s=0,rx=0,rz=0;
			x = Ogre::StringConverter::parseReal(xPos);
			y = Ogre::StringConverter::parseReal(yPos);
			z = Ogre::StringConverter::parseReal(zPos);
			r = Ogre::StringConverter::parseReal(rot);
			s = Ogre::StringConverter::parseReal(scale);
			rx = Ogre::StringConverter::parseReal(rotX);
			rz = Ogre::StringConverter::parseReal(rotZ);

			// auto drop models on terrain
			if (y==0) {
				float tmpY = getMeshYAdjust(meshFile)* s;
				//y = (float) mTerrainInfo->getHeightAt(x,z)+tmpY;
				y = getHeightAt(x,z)+tmpY;
			}

			//****************************
			staticGeometry->addEntity(newModel,Ogre::Vector3(x,y,z),Quaternion ((Degree(r)), Vector3::UNIT_Y), Vector3(s,s,s));
			mSceneMgr->destroyEntity(newModel);
			//***************************
		} catch (Exception &e){
			cout << "Troubles adding entity " << entName.c_str() << " to static geometry :" << e.what() << "\n";
		};

		counter ++;
	} while (!stream->eof() && !StringUtil::startsWith(tmp,"[END]",false));

	staticGeometry->build();
}

float ArtifexLoader::getMeshYAdjust(Ogre::String meshfile)
{
	Ogre::ConfigFile cfgfile;
	float realY = 0;
	try {
		cfgfile.load("y_correction_tables.cfg");
		realY = Ogre::StringConverter::parseReal(cfgfile.getSetting(meshfile));
	} catch (...){
		cout << "Troubles loading the y_correction_tables.cfg.\n";
		return 0;
	};
	return realY;
};
