#include "CMaterialSwapper.h"

irr::video::CMaterialSwapper::CMaterialSwapper(scene::ISceneManager* smgr, SMaterials* materials)
    : Smgr(smgr),
      Materials(materials)
{
    updateEntry(EMT_SOLID, Materials->Solid);
    updateEntry(EMT_DETAIL_MAP, Materials->DetailMap);
    updateEntry(EMT_NORMAL_MAP_SOLID, Materials->Normal);
    updateEntry(EMT_PARALLAX_MAP_SOLID, Materials->Parallax);
    updateEntry(EMT_TRANSPARENT_ALPHA_CHANNEL_REF, Materials->TransparentRef);
    updateEntry(EMT_TRANSPARENT_ALPHA_CHANNEL, Materials->Transparent);
}

irr::video::CMaterialSwapper::~CMaterialSwapper()
{
    for (u32 i = 0; i < Entries.size(); i++) { delete Entries[i]; }

    Entries.clear();
}

void irr::video::CMaterialSwapper::swapMaterials(scene::ISceneNode* node) const
{
    if (node) { swapMaterialsOnNode(node); }
    else {
        core::array<scene::ISceneNode*> nodes;
        Smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes);
        for (u32 i = 0; i < nodes.size(); i++) { swapMaterialsOnNode(nodes[i]); }
    }
}

void irr::video::CMaterialSwapper::updateEntry(E_MATERIAL_TYPE swapFrom, E_MATERIAL_TYPE swapTo)
{
    for (u32 i = 0; i < Entries.size(); i++) {
        if (Entries[i]->SwapFrom == swapFrom) {
            Entries[i]->SwapTo = swapTo;
            return;
        }
    }

    SMaterialSwapperEntry* entry = new SMaterialSwapperEntry;
    entry->SwapFrom = swapFrom;
    entry->SwapTo = swapTo;
    Entries.push_back(entry);
}

void irr::video::CMaterialSwapper::removeEntry(E_MATERIAL_TYPE swapFrom)
{
    for (u32 i = 0; i < Entries.size(); i++) {
        if (Entries[i]->SwapFrom == swapFrom) {
            delete Entries[i];
            Entries.erase(i);
            return;
        }
    }
}

// TODO: Optimize
void irr::video::CMaterialSwapper::swapMaterialsOnNode(scene::ISceneNode* node) const
{
    for (u32 i = 0; i < node->getMaterialCount(); i++) {
        for (u32 ii = 0; ii < Entries.size(); ii++) {
            if (node->getMaterial(i).MaterialType == Entries[ii]->SwapFrom) node->getMaterial(i).MaterialType = Entries[
                ii]->SwapTo;
        }
    }
}
