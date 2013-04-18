#ifndef __ETTILE_H__
#define __ETTILE_H__

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

#include <OgreString.h>
#include <OgreMaterial.h>

#include "Impl/ETOptions.h"

// forward declarations
namespace Ogre
{
  class SceneManager;
  class RenderOperation;
  class Matrix4;
  class Quaternion;
  class Vector3;
  class Camera;
}

namespace ET
{
  // forward declarations
  class TerrainInfo;


  namespace Impl
  {
    class IndexHandler;
    class TerrainImpl;

    /** A tile is the unit of terrain used for rendering. */
    class Tile : public Ogre::Renderable, public Ogre::MovableObject
    {
    public:
      Tile(const Ogre::String& name, Ogre::SceneManager* sm, TerrainImpl* tm,
        IndexHandler* indexHandler, TerrainInfo& info, Options opts, size_t startx, size_t startz);
      ~Tile();

      Ogre::uint32 getTypeFlags() const;

      const Ogre::MaterialPtr& getMaterial() const;

      void getRenderOperation(Ogre::RenderOperation& op);

      void getWorldTransforms(Ogre::Matrix4* m) const;

      const Ogre::Quaternion& getWorldOrientation() const;

      const Ogre::Vector3& getWorldPosition() const;

      Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

      const Ogre::LightList& getLights() const;

      const Ogre::String& getMovableType() const;

      const Ogre::AxisAlignedBox& getBoundingBox() const;

      Ogre::Real getBoundingRadius() const;

      void _updateRenderQueue(Ogre::RenderQueue* queue);

      /** Determines the LOD to use based on the current camera distance */
      void _notifyCurrentCamera(Ogre::Camera* cam);

      unsigned int getLOD() const;

      void setRenderQueue(Ogre::uint8 qid);

      /** Updates the custom morph factor parameter for the morph vertex shader */
      void _updateCustomGpuParameter(const Ogre::GpuProgramParameters::AutoConstantEntry& constEntry,
        Ogre::GpuProgramParameters* params) const;


      /** Tells the tile to update its contents based on updated heightmap data.
        * This is what makes terrain deforming working.
        */
      void updateTerrain(size_t startx, size_t startz, size_t endx, size_t endz);


#if OGRE_VERSION_MINOR > 4
      /** Shoggoth compatibility function */
      void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables);
#endif

    private:
      /** Initialises the vertices */
      void createVertexData(size_t startx, size_t startz);
      Ogre::IndexData* getIndexData();

      /** Calculates the LOD distances */
      void calculateMinLevelDist2();

      /** Calculates vertex normals */
      void calculateVertexNormals();

      /** Calculates vertex tangents */
      void calculateVertexTangents();

      /** Retrieves the position vector for the given vertex */
      Ogre::Vector3 getVector(size_t x, size_t z) const;

      /** Convenience function to empty a delta buffer */
      void emptyBuffer(Ogre::HardwareVertexBufferSharedPtr buf);

      Ogre::SceneManager* mSceneMgr;
      TerrainImpl* mTerrainMgr;
      IndexHandler* mIndexHandler;
      TerrainInfo& mInfo;
      Options mOpt;

      /** Info about this tile in regards to all tiles */
      size_t mTileX, mTileZ;
      size_t mStartX, mStartZ;

      /** Current LOD */
      unsigned int mLOD;
      /** LOD change distances */
      std::vector<Ogre::Real> mLODChangeMinDistSqr;

      Ogre::AxisAlignedBox mBounds;
      Ogre::Real mBoundingRadius;
      Ogre::Vector3 mCenter;

      mutable bool mLightListDirty;
      mutable Ogre::LightList mLightList;

      /** The tile's vertices */
      Ogre::VertexData* mTerrain;
      Ogre::HardwareVertexBufferSharedPtr mMainBuffer;
      std::vector<Ogre::HardwareVertexBufferSharedPtr> mDeltaBuffers;
      Ogre::Real mLODMorphFactor;
      unsigned int mLastNextLevel;
    };
  }

}


#if 0
#include "OgreTerrainPrerequisites.h"
#include "OgreRenderable.h"
#include "OgreMovableObject.h"
#include "OgreAxisAlignedBox.h"
#include "OgreString.h"
#include "OgreHardwareBufferManager.h"

#include "ETOptions.h"

#include <vector>

namespace Ogre
{
  class ETSceneManager;


  /** A tile is the unit of terrain that gets rendered. */
  class _OgreOctreePluginExport ETTile : public Renderable, public MovableObject
  {
  public:
    ETTile(const String& name, ETSceneManager* etsm, ETOptions& options, int startx, int startz);
    ~ETTile();

    uint32 getTypeFlags() const;

    const MaterialPtr& getMaterial() const;

    void getRenderOperation(RenderOperation& op);

    void getWorldTransforms(Matrix4* m) const;

    const Quaternion& getWorldOrientation() const;

    const Vector3& getWorldPosition() const;

    Real getSquaredViewDepth(const Camera* cam) const;

    const LightList& getLights() const;

    const String& getMovableType() const;

    const AxisAlignedBox& getBoundingBox() const;

    Real getBoundingRadius() const;

    void _updateRenderQueue(RenderQueue* queue);

    /** Determines the LOD to use based on the current camera distance */
    void _notifyCurrentCamera(Camera* cam);

    int getLOD() const;

    void setRenderQueue(uint8 qid);

    /** Updates the custom morph factor parameter for the morph vertex shader */
    void _updateCustomGpuParameter(const GpuProgramParameters::AutoConstantEntry& constEntry,
      GpuProgramParameters* params) const;


    /** Tells the tile to update its contents based on updated heightmap data.
      * This is what makes terrain deforming working.
      */
    void updateTerrain(int startx, int startz, int endx, int endz);


  private:
    /** Initialises the vertices */
    void createVertexData(float* data, int startx, int startz);
    IndexData* getIndexData();

    /** Calculates the LOD distances */
    void _calculateMinLevelDist2();
    Real _calculateCFactor();

    /** Retrieves the position vector for the given vertex */
    Vector3 _getVector(int x, int z) const;

    /** Convenience function to empty a delta buffer */
    void emptyBuffer(HardwareVertexBufferSharedPtr buf);

    ETSceneManager* mSceneMgr;
    ETOptions& mOptions;
    MaterialPtr mMaterial;

    /** Info about this tile in regards to all tiles */
    int mTileX, mTileZ;
    int mStartX, mStartZ;

    /** Current LOD */
    int mLOD;
    /** LOD change distances */
    std::vector<Real> mLODChangeMinDistSqr;

    AxisAlignedBox mBounds;
    Real mBoundingRadius;
    Vector3 mCenter;

    mutable bool mLightListDirty;
    mutable LightList mLightList;

    /** The tile's vertices */
    VertexData* mTerrain;
    HardwareVertexBufferSharedPtr mMainBuffer;
    std::vector<HardwareVertexBufferSharedPtr> mDeltaBuffers;
    Real mLODMorphFactor;
    int mLastNextLevel;
  };
}
#endif
#endif
