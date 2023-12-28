#pragma once

#include <string>

#include "ItemData.h"

class ItemDatabase
{
public:
    static void Load();

    static Item GetItemByID(itemid id);
    static Item GetItemByName(std::string name);

};
