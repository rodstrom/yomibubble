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
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "ArtifexLoader.h"
class ArtifexLoader;

#include "CppSQLite3.h"

class DBManager {
public:
	DBManager(ArtifexLoader *artifexloader);
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

	int Save();
	int Load();

	bool saving;

};
#endif
