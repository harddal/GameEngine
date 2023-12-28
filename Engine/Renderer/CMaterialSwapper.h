#ifndef CMATERIALSWAPPER_H
#define CMATERIALSWAPPER_H

#include <irrlicht.h>
#include "SMaterials.h"

namespace irr
{
    namespace video
    {
        struct SMaterialSwapperEntry
        {
            E_MATERIAL_TYPE SwapFrom;
            E_MATERIAL_TYPE SwapTo;
        };


        class CMaterialSwapper
        {
        public:

            CMaterialSwapper(scene::ISceneManager* smgr, SMaterials* materials);
            ~CMaterialSwapper();

  
            void swapMaterials(scene::ISceneNode* node = 0) const;

     
            void updateEntry(E_MATERIAL_TYPE swapFrom, E_MATERIAL_TYPE swapTo);

   
            void removeEntry(E_MATERIAL_TYPE swapFrom);

        private:
            scene::ISceneManager* Smgr;
            SMaterials* Materials;
            core::array<SMaterialSwapperEntry*> Entries;

            void swapMaterialsOnNode(scene::ISceneNode* node) const;
        };
    }
}

#endif
