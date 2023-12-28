#include "ItemDatabase.h"

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include "Utility/INIFile.h"

#include "Engine/Resource/FilePaths.h"

using namespace boost;
using namespace filesystem;

static std::vector<Item> g_ItemList;

void ItemDatabase::Load()
{
    // HACK: Reloads item list every time game is restarted/loaded/editor->game, it gets cleared but not performant
    g_ItemList.clear();
    std::vector<Item>().swap(g_ItemList);

    const path dir = "content/item/";
    recursive_directory_iterator it(dir), end;

    std::vector<std::wstring> files;
    for (auto& entry : make_iterator_range(it, end)) {
        if (is_regular(entry)) {
            files.emplace_back(entry.path().native());
        }
    }

    auto iter = 0U;
    for (auto& file : files) {
        const auto
            sfilepath = std::string(file.begin(), file.end()),
            sfilename = Utility::FilenameFromPath(sfilepath),
            sfileext  = Utility::FileExtensionFromPath(sfilepath);

        if (sfileext == ".item") {
            CIniFile configLoader;
            if (!configLoader.Load("content/item/" + sfilename + sfileext)) {
                continue;
            }

            Item item;
            item.id     = iter;
            item.name   = configLoader.GetKeyValue("item", "name");
            item.desc   = configLoader.GetKeyValue("item", "desc");
            item.icon   = configLoader.GetKeyValue("item", "icon");
            item.entity = configLoader.GetKeyValue("item", "entity");
            item.script = configLoader.GetKeyValue("item", "script");
            item.exec   = configLoader.GetKeyValue("item", "exec");

            item.pickupsound = configLoader.GetKeyValue("item", "pickupsound");

            item.equipIcon = configLoader.GetKeyValue("item", "equipicon");
            item.refEntity = configLoader.GetKeyValue("item", "refentity");

            item.equip1H = Utility::EvalTrueFalse(configLoader.GetKeyValue("item", "equip1H").c_str());
            item.equip2H = Utility::EvalTrueFalse(configLoader.GetKeyValue("item", "equip2H").c_str());

            item.size.X = atoi(configLoader.GetKeyValue("item", "sizex").c_str());
            item.size.Y = atoi(configLoader.GetKeyValue("item", "sizey").c_str());

            // BUG: Causes occasinal access violation in the driver when un/equipping weapons
            item.iconTexture      = RenderManager::Get()->driver()->getTexture(_asset_tex(item.icon).c_str());
            item.iconEquipTexture = RenderManager::Get()->driver()->getTexture(_asset_tex(item.equipIcon).c_str());

            if (!item.script.empty()) {
                item.script_component.script = item.script;
                WorldManager::Get()->scriptSystem()->compile(item.script_component);
            }
			
			item.data = configLoader.GetKeyValue("item", "data");

            g_ItemList.push_back(item);
            iter++;
        }
    }
}

// If ID is not found returns a null item
// item.id = 0xFFFF
Item ItemDatabase::GetItemByID(itemid id)
{
    for (auto item : g_ItemList) {
        if (item.id == id) {
            return item;
        }
    }

    spdlog::error("Item Database failed to load item ID: [0]", id);

    return Item();
}
// If name is not found returns a null item
// item.name = "null_item"
Item ItemDatabase::GetItemByName(std::string name)
{
    for (auto item : g_ItemList) {
        if (item.name == name) {
            return item;
        }
    }

    spdlog::error("Item Database failed to load item: {}", name);

    return Item();
}
