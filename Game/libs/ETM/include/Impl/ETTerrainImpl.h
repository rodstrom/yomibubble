#ifndef __ETTERRAINIMPL_H__
#define __ETTERRAINIMPL_H__

/*
EDITABLE TERRAIN MANAGER for Ogre
Copyright (C) 2007  Holger Frydrych <h.frydrych@gmx.de>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

As a special exception, you may use this file as part of a free software
library without restriction.  Specifically, if other files instantiate
templates or use macros or inline functions from this file, or you compile
this file and link it with other files to produce an executable, this
file does not by itself cause the resulting executable to be covered by
the GNU General Public License.  This exception does not however
invalidate any other reasons why the executable file might be covered by
the GNU General Public License.
*/

#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "Impl/ETIndexHandler.h"
#include "Impl/ETOptions.h"

#include <OgreSceneManager.h>
#include <OgreMaterial.h>

#include <string>
#include <vector>


namespace ET
{
  namespace Impl
  {
    class Tile;

    class TerrainImpl
    {
    public:
      TerrainImpl(Ogre::SceneManager* sceneMgr, const std::string& name);
      ~TerrainImpl();

      void createTerrain(const TerrainInfo& info, size_t tileSize, unsigned int maxLOD, bool vertexNormals, bool vertexTangents);
      void destroyTerrain();

      bool isTerrainLoaded() const { return mTerrainLoaded; }

      const TerrainInfo& getTerrainInfo() const { return mInfo; }

      void setLODErrorMargin(unsigned int maxPixelError, unsigned int viewportHeight);

      void setUseLODMorphing(bool lodMorph, float startMorphing, const std::string& morphParamName);

      void setMaterial(Ogre::MaterialPtr material) { mMaterial = material; }
      const Ogre::MaterialPtr& getMaterial() const { return mMaterial; }

      unsigned int getNeighbourState(size_t x, size_t z) const;

      void deform(int x, int z, const Brush& brush, float intensity);
      void setHeights(int x, int z, const Brush& brush);
      void getHeights(int x, int z, Brush& brush) const;


    private:
      void createTiles();
      void updateTiles(int fromX, int fromZ, int toX, int toZ);

      Ogre::SceneManager* mSceneMgr;
      Ogre::SceneNode* mTerrainNode;
      bool mTerrainLoaded;
      IndexHandler* mIndexHandler;
      TerrainInfo mInfo;
      Options mOpt;
      std::string mInstanceName;
      int mViewportHeight;
      Ogre::MaterialPtr mMaterial;

      typedef std::vector<Tile*> TileCol;
      typedef std::vector<TileCol> TileGrid;
      /** Our terrain tile grid.*/
      TileGrid mTiles;

      bool maxLODAtBorders;

    };
  }
}

#endif
