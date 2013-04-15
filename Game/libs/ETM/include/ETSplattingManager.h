#ifndef __ETSPLATTINGMANAGER_H__
#define __ETSPLATTINGMANAGER_H__

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

#include "OgreColourValue.h"
#include "OgreImage.h"

#include <string>
#include <vector>

// forward declarations
namespace ET
{
  namespace Impl
  {
    struct SplattingImpl;
  }
  class Brush;
}


namespace ET
{
  typedef std::vector<std::string> NameList;
  typedef std::vector<Ogre::ColourValue> ColourList;
  typedef std::vector<Ogre::Image> ImageList;

  /**
   * SplattingManager allows you to create and edit an arbitrary amount of
   * alpha splatting maps which can be used with a splatting shader
   * to render multiple splatting textures per pass onto the terrain.
   * You can use 1 to 4 channels per map.
   */
  class _ETManagerExport SplattingManager
  {
  public:
    /**
     * Constructs the splatting manager.
     * @param baseName   base name for the map textures (will be appended by their number)
     * @param group      the resource group the textures will be placed in
     * @param width      width of the textures in pixels
     * @param height     height of the textures in pixels
     * @param channels   Number of channels per texture (must be in {1, 2, 3, 4})
     */
    SplattingManager(const std::string& baseName, const std::string& group,
      unsigned int width, unsigned int height, unsigned int channels = 4);
    ~SplattingManager();

    /**
     * Sets the amount of splatting textures you need coverage maps for.
     * Note that this will round up to the next multiple of the channels
     * per map.
     */
    void setNumTextures(unsigned int numTextures);

    /** Retrieves the current amount of splatting textures covered. */
    unsigned int getNumTextures() const;

    /** Sets the number of coverage maps to manage. */
    void setNumMaps(unsigned int numMaps);

    /** Retrieves the current amount of coverage maps. */
    unsigned int getNumMaps() const;

    /** Retrieves the names of the map textures for use in materials. */
    NameList getMapTextureNames() const;

    /** Loads a coverage map texture from an Ogre::Image. */
    void loadMapFromImage(unsigned int mapNum, const Ogre::Image& image);

    /** Saves a coverage map texture to an Ogre::Image. */
    void saveMapToImage(unsigned int mapNum, Ogre::Image& image);

    /** Paint with the specified texture and selected brush and intensity. */
    void paint(unsigned int textureNum, int x, int y, const Brush& brush, float intensity = 1.0f);

    /** Create a colour map with a given list of colours for the textures. */
    void createColourMap(Ogre::Image& image, const ColourList& colours);

    /**
     * Create base texture using the given list of splatting texture images.
     * Be aware that this is expensive!
     * @param image    An Ogre image which is used as the output target.
     * @param width    The desired width of the base texture.
     * @param height   The desired height of the base texture.
     * @param textures A list of splatting textures to use for the generation.
     * @param repeatX  How often should the textures be splatted in X direction?
     * @param repeatZ  How often should the textures be splatted in Z direction?
     */
    void createBaseTexture(Ogre::Image& image, size_t width, size_t height, ImageList textures,
      float repeatX, float repeatZ);

  private:
    // Implementation hiding via PIMPL idiom
    Impl::SplattingImpl* mImpl;

  };


  /** Modulates a colour map by a lightmap. Can be used as a minimap of the terrain. */
  Ogre::Image _ETManagerExport createMinimap(const Ogre::Image& colourMap, const Ogre::Image& lightMap);
}

#endif
