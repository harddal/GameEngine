#pragma once

#include <vector>
#include <vector2d.h>

#include "Game/Item/ItemData.h"

struct InventoryData
{
    irr::core::vector2di size;
    std::vector<Item> contents;

	InventoryData& operator=(InventoryData data)
	{
		std::swap(size, data.size);
		std::swap(contents, data.contents);
		
		return *this;
	}
};
