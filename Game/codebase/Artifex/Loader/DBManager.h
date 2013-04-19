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
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "ArtifexLoader.h"
class ArtifexLoader;

#include "CppSQLite3.h"

#include "..\..\Managers\GameObjectManager.h"
#include "..\..\Managers\SoundManager.h"

#include <string>
#include <vector>

class DBManager {
public:
	DBManager(ArtifexLoader *artifexloader, GameObjectManager *game_object_manager, SoundManager *sound_manager);
	~DBManager();

	ArtifexLoader *mArtifexLoader;
	CppSQLite3DB *mDB;

	int Open(const string path);
	int Close(void);

	bool ExecSQL (std::string query);
	bool EmptyTable(std::string tablename);
	bool WriteSpawn(Spawn2 spawn, std::string spawntype="default");
	bool DeleteSpawn(const Spawn2& spawn , bool clear_attributes=true);
	bool ClearAttributes(const Spawn2& spawn);
	bool WriteAttributes(Spawn2& spawn);
	bool EmptyTrash();
	int DBManager::getObjectProperties(Spawn2 &spawn);
	Vector3 DBManager::getWaypoint(string waypoint_id);

	std::vector<std::string> &DBManager::split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> DBManager::split(const std::string &s, char delim);

	int Save();
	int Load();

	bool saving;

	GameObjectManager *m_game_object_manager;
	SoundManager *m_sound_manager;

};
#endif
