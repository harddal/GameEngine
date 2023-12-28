#pragma once

#include <array>
#include <string>
#include <vector>

enum E_MANAGED_MATERIAL
{
    MAT_INVALID,
    MAT_EARTH,
    MAT_GRAVEL,
	MAT_WATER,
    MAT_STONE,
    MAT_METAL,
    MAT_GLASS,
    MAT_CARPET,
    MAT_WOOD
};

const std::array<std::string, 9> g_managedMaterialName = 
{
    "invalid",
    "earth",
    "gravel",
    "water",
    "stone",
    "metal",
    "glass",
    "carpet",
    "wood"
};

class MaterialBuilder
{
public:
    MaterialBuilder();
    
	void buildMaterialTable();

	std::string getMaterialName(E_MANAGED_MATERIAL material);
	E_MANAGED_MATERIAL getMaterialFromTexture(const std::string& texture);
    std::string getMaterialNameFromTexture(const std::string& texture);
    
private:
	std::vector<std::pair<std::string, E_MANAGED_MATERIAL>> m_managedMaterialList;
	std::vector<std::pair<std::string, E_MANAGED_MATERIAL>> m_regexList;
};
