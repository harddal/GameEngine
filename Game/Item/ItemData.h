#pragma once

#include <string>
#include <vector>
#include <utility>

#include <ITexture.h>

#include "Engine/Engine.h"

#define ITEM_NULL_ID 0xFFFF
#define ITEM_SLOT_FILLED_ID 0xFFFFA
#define ITEM_NULL_NAME "null_item"

typedef unsigned int itemid;

struct ItemDataPair
{
	std::string key, value;

	ItemDataPair() {}
	ItemDataPair(std::string key, std::string value)
	{
		this->key = key;
		this->value = value;
	}
	ItemDataPair& operator=(ItemDataPair pair)
	{
		std::swap(key, pair.key);
		std::swap(value, pair.value);

		return *this;
	}
};

struct Item
{
    unsigned int slot;

    bool equip1H, equip2H, isReference;

    itemid id;
    std::string name, desc, icon, equipIcon, entity, refEntity, script, exec, pickupsound;

    irr::core::vector2di size;

    irr::video::ITexture* iconTexture, *iconEquipTexture;

    ScriptComponent script_component;

    std::string data;

	std::vector<ItemDataPair> entityData;

	Item() : iconTexture(nullptr)
	{
		slot = ITEM_NULL_ID;
		id   = ITEM_NULL_ID;
		name = ITEM_NULL_NAME;
		desc = "";

		equip1H = false;
		equip2H = false;
		isReference = false;

		iconEquipTexture = nullptr;
	}

	Item& operator=(Item item)
	{
		std::swap(equip1H, item.equip1H);
		std::swap(equip2H, item.equip2H);
		std::swap(isReference, item.isReference);
		std::swap(slot, item.slot);
		std::swap(id, item.id);
		std::swap(name, item.name);
		std::swap(desc, item.desc);
		std::swap(icon, item.icon);
		std::swap(entity, item.entity);
		std::swap(script, item.script);
		std::swap(exec, item.exec);
		std::swap(size, item.size);
		std::swap(iconTexture, item.iconTexture);
		std::swap(data, item.data);
		std::swap(script_component, item.script_component);
		std::swap(refEntity, item.refEntity);
		std::swap(equipIcon, item.equipIcon);
		std::swap(iconEquipTexture, item.iconEquipTexture);
		std::swap(pickupsound, item.pickupsound);

		return *this;
	}

	template <class Archive>
	void serialize(Archive& archive)
	{

	}
};
