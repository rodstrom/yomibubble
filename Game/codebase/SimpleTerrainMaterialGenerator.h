#ifndef __SIMPLETMG_H__
#define __SIMPLETMG_H__

#include "OGRE\Terrain\OgreTerrain.h"

// Simple Terrain  Material Generator sample to show how to use Ogre .material files with the new Ogre::Terrain

class SimpleTerrainMaterialGenerator :
      public Ogre::TerrainMaterialGenerator
   {
   public:

      SimpleTerrainMaterialGenerator() :
         Ogre::TerrainMaterialGenerator()
      {
         mProfiles.push_back(OGRE_NEW OgreMaterialProfile(this, "OgreMaterialProfile", "Profile to use Ogre .material files"));
         setActiveProfile("OgreMaterialProfile");
      }

      class OgreMaterialProfile : public Ogre::TerrainMaterialGenerator::Profile
      {
      public:
         OgreMaterialProfile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc) :
            Ogre::TerrainMaterialGenerator::Profile(parent,name,desc)
         {
         }

         virtual ~OgreMaterialProfile() {}
         Ogre::MaterialPtr generate(const Ogre::Terrain* terrain)
         {
            Ogre::MaterialManager& materialManager = Ogre::MaterialManager::getSingleton();
         // Set ETM material
            return materialManager.getByName("ETTerrainMaterial");
         }

      
         Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain)
         {
            return terrain->_getCompositeMapMaterial();
         }

         Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const
         {
            return 0;
         }

         void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain) {}

         void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain) {}

         void requestOptions(Ogre::Terrain* terrain) {}
      
      };
   };
#endif