#ifndef __ETTERRAINMANAGER_H__
#define __ETTERRAINMANAGER_H__

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

#include "ETPrerequisites.h"

#include <string>


// forward declarations
namespace Ogre
{
  class SceneManager;
  class MaterialPtr;
  class Vector3;
};
namespace ET
{
  class TerrainInfo;
  class Brush;
  namespace Impl
  {
    class TerrainImpl;
  }
}



namespace ET
{
  /**
  * The Editable Terrain Manager creates and manages tiles of terrain
  * constructed from heightmap data. The terrain can be edited any time.
  **/
  class _ETManagerExport TerrainManager
  {
  public:
    /**
     * Constructs the Terrain Manager.
     * @param sceneMgr  The Ogre Scene Manager to use for rendering.
     * @param name      Instance name used internally to create unique object
     *                  names for terrain tiles and nodes.
     */
    TerrainManager(Ogre::SceneManager* sceneMgr, const std::string& name = "ETTerrain");
    ~TerrainManager();

    /**
     * Creates terrain as specified by the given TerrainInfo.
     * @param info           TerrainInfo to construct terrain from
     * @param tileSize       the tile size to use, must be (2^n+1)
     * @param maxLOD         the maximal level of detail to be used
     * @param vertexNormals  generate vertex normals? (necessary for dynamic lighting)
     * @param vertexTangents generate vertex tangents? (necessary for adv. dynamic lighting)
     */
    void createTerrain(const TerrainInfo& info, size_t tileSize = 33, unsigned int maxLOD = 255, 
      bool vertexNormals = false, bool vertexTangents = false);

    /** Destroys the currently loaded terrain (if any). */
    void destroyTerrain();

    /**
     * Enables or disables support for terrain LOD morphing.
     * Note that this method has no effect on already loaded terrain, you
     * need to reload your terrain. Also this needs a corresponding
     * vertex shader in the associated material to actually work.
     * @param lodMorph
     * @param startMorphing   percentage of the distance at which to start morphing
     * @param morphParamName  name of the shader parameter which contains the current morph factor
     */
    void setUseLODMorphing(bool lodMorph = true, float startMorphing = 0.25f,
      const std::string& morphParamName = "morphFactor");

    /**
     * Specifies the pixel error tolerance when determining LOD to use for each tile.
     * Note that this call does not affect already loaded terrain data.
     * @param maxPixelError  the maximum estimated pixel error to tolerate
     * @param viewportHeight vertical resolution of the viewport to use to estimate the pixel error
     */
    void setLODErrorMargin(unsigned int maxPixelError, unsigned int viewportHeight);


    /** Retrieves the terrain info of the currently loaded terrain. */
    const TerrainInfo& getTerrainInfo() const;



    /** Sets the material to use for the terrain tiles. You need to call this
      * if you actually want to see anything :)
      */
    void setMaterial(Ogre::MaterialPtr material);

    /** Retrieve the currently used material for the terrain. */
    const Ogre::MaterialPtr& getMaterial() const;

    /**
     * Deforms terrain vertices at the given coordinates with the given brush.
     * Note that the positions specify vertex indexes, NOT scene coordinates.
     * Use TerrainInfo::posToVertex if necessary.
     */
    void deform(int x, int z, const Brush& brush, float intensity = 1.0f);

    /**
     * Sets terrain heights at the given coordinates as specified by the given brush.
     * Note that the positions specify vertex indexes, NOT scene coordinates.
     */
    void setHeights(int x, int z, const Brush& brush);

    /**
     * Retrieves terrain heights at the given coordinates and stores them into the given brush.
     * Note that the positions specify vertex indexes, NOT scene coordinates.
     */
    void getHeights(int x, int z, Brush& brush) const;


  private:
    /** implementation hiding via PIMPL idiom */
    Impl::TerrainImpl* mImpl;
  };
}


#endif
