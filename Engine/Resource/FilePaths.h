#pragma once

#include <string>

const std::string g_asset_path   = "content/";
const std::string g_entity_path  = "content/entity/";
const std::string g_prefab_path  = "content/prefab/";
const std::string g_mesh_path    = "content/mesh/";
const std::string g_texture_path = "content/texture/";
const std::string g_sound_path   = "content/sound/";
const std::string g_music_path   = "content/sound/music/";
const std::string g_script_path  = "content/script/";
const std::string g_scene_path   = "content/scene/";
const std::string g_item_path    = "content/item/";

#define _asset_asc(x) (g_script_path  + std::string(x) + std::string(".asc"))
#define _asset_ent(x) (g_entity_path  + std::string(x) + std::string(".ent"))
#define _asset_pre(x) (g_prefab_path  + std::string(x) + std::string(".pre"))
#define _asset_scn(x) (g_scene_path   + std::string(x) + std::string(".scn"))
#define _asset_tex(x) (g_texture_path + std::string(x) + std::string(".png"))
#define _asset_snd(x) (g_sound_path   + std::string(x) + std::string(".wav"))
#define _asset_b3d(x) (g_mesh_path    + std::string(x) + std::string(".b3d"))
#define _asset_itm(x) (g_item_path    + std::string(x) + std::string(".item"))

