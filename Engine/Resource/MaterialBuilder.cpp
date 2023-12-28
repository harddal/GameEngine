#include "Engine/Resource/MaterialBuilder.h"

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <spdlog/spdlog.h>

#include "Utility/Utility.h"

using namespace std;
using namespace boost;
using namespace filesystem;

MaterialBuilder::MaterialBuilder()
{
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(dev)"                      ), MAT_INVALID));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(grass|dirt|earth|sand)"    ), MAT_EARTH  ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(gravel|pebbles)"           ), MAT_GRAVEL ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(sludge|mud|goo|water|wet)" ), MAT_WATER  ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(concrete|brick|rock|stone)"), MAT_STONE  ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(metal|iron|steel)"         ), MAT_METAL  ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(glass|tile|ceramic)"       ), MAT_GLASS  ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(carpet|soft|felt)"         ), MAT_CARPET ));
    m_regexList.emplace_back(std::pair<std::string, E_MANAGED_MATERIAL>(std::string("(wood|plank|bark|board)"    ), MAT_WOOD   ));
}

void MaterialBuilder::buildMaterialTable()
{
	const path dir = "content/";
	recursive_directory_iterator it(dir), end;

	vector<std::wstring> files;
	for (auto& entry : make_iterator_range(it, end)) {
		if (is_regular(entry)) {
			files.emplace_back(entry.path().native());
		}
	}

	for (auto& file : files) {
		const auto 
	        filepath = string(file.begin(), file.end()),
	        filename = Utility::FilenameFromPath(filepath),
	        file_ext = Utility::FileExtensionFromPath(filepath);

		auto match = false;
	    for (const auto& mat : m_managedMaterialList) {
	        if (mat.first == filename) {
				match = true;
	        }
	    }
	    
	    if (match) {
		    continue;
	    }

		if (
            file_ext == string(".png") ||
            file_ext == string(".jpg") ||
            file_ext == string(".dds") ||
            file_ext == string(".tga"))
		{
		    for (auto expr : m_regexList) {
		        smatch result;

				if (regex_search(filename, result, regex(expr.first))) {
					m_managedMaterialList.emplace_back(pair<string, E_MANAGED_MATERIAL>(filename, expr.second));
				}
		    }
		}
	}
}

std::string MaterialBuilder::getMaterialName(E_MANAGED_MATERIAL material)
{
    if (static_cast<unsigned int>(material) > g_managedMaterialName.size() - 1 || static_cast<unsigned int>(material) < 0) {
		return g_managedMaterialName.at(0);
    }
    
	return g_managedMaterialName[static_cast<unsigned int>(material)];
}

E_MANAGED_MATERIAL MaterialBuilder::getMaterialFromTexture(const std::string& texture)
{
    for (const auto& pair : m_managedMaterialList) {
        if (pair.first == texture) {
			return pair.second;
        }
    }

	return MAT_INVALID;
}

std::string MaterialBuilder::getMaterialNameFromTexture(const std::string& texture)
{
    for (const auto& pair : m_managedMaterialList) {
        if (pair.first == texture) {
            return g_managedMaterialName[static_cast<unsigned int>(pair.second)];
        }
    }

    return g_managedMaterialName.at(0);
}