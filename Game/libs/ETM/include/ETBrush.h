#ifndef __ETBRUSH_H__
#define __ETBRUSH_H__

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

#include <vector>

// forward declarations
namespace Ogre
{
  class Image;
}


namespace ET
{
  /** This class represents a brush used to deform terrain or edit splatting coverage */
  class _ETManagerExport Brush
  {
  public:
    /** Default constructor */
    Brush();
    /** Constructs a brush from a given array of floats */
    Brush(const float* brush, size_t width, size_t height);
    /** Constructs a brush from a given vector of floats */
    Brush(const std::vector<float>& brush, size_t width, size_t height);
    /** Copy constructor */
    Brush(const Brush& other);
    ~Brush();

    /** Copy assignment */
    Brush& operator=(const Brush& other);

    size_t getWidth() const { return mWidth; }
    size_t getHeight() const { return mHeight; }

    /** Access to the brush array */
    float& at(size_t x, size_t y)
    {
      return mBrushArray[x + y*mWidth];
    }
    /** Const access to the brush array */
    const float at(size_t x, size_t y) const
    {
      return mBrushArray[x + y*mWidth];
    }

    /** Exception-safe swap function */
    void swap(Brush& other);

  private:
    /** The actual brush array containing the brush values */
    float* mBrushArray;  // would have used vector<float>, but gives warnings with dll export
    /** The brush's dimensions */
    size_t mWidth, mHeight;
  };

  /** Loads a brush from a grayscale image */
  Brush _ETManagerExport loadBrushFromImage(const Ogre::Image& image);

  /** Saves a brush to a grayscale image */
  void _ETManagerExport saveBrushToImage(const Brush& brush, Ogre::Image& image);
}


#endif
