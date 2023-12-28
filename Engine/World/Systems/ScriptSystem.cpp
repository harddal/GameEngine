#include "ScriptSystem.h"

#include <cassert>
#include <string>

//#include "Engine/Script/Bindings/ScriptBindings.h"

#include "Engine/Resource/FilePaths.h"

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "Utility/Utility.h"
#include <spdlog/spdlog.h>

#include "Engine/Engine.h"

using namespace std;
using namespace boost;
using namespace filesystem;
using namespace anax;

bool ScriptSystem::m_fileHasChanged;
string ScriptSystem::m_modifiedFile;

//ScriptSystem::FileWatcherUpdateListener g_FileWatchListener;

ScriptSystem::ScriptSystem()
{
    m_id_generator = 0L;

    if (SCRIPT_EDIT_CONTINUE_SUPPORT) {
        /*    const path directory = "content/script/";
            recursive_directory_iterator it(directory), end;

            vector<std::wstring> files;
            for (auto& entry : make_iterator_range(it, end)) {
                if (is_regular(entry)) {
                    files.emplace_back(entry.path().native());
                }
            }

            vector<std::string> dirs;
            for (auto& file : files) {
                auto
                    filepath = string(file.begin(), file.end()),
                    filename = util::filenameFromPath(filepath),
                    rel_path = filepath.substr(0, filepath.find_last_of('\\'));

                std::replace(rel_path.begin(), rel_path.end(), '\\', '/');

                bool isCopy = false;
                for (auto& dir : dirs) {
                    if (dir == rel_path) {
                        isCopy = true;

                        break;
                    }
                }

                if (!isCopy) {
                    dirs.emplace_back(rel_path);
                }
            }

            for (auto& dir : dirs) {
                m_fileWatcher.addWatch(dir, &g_FileWatchListener);
            }
            */

            //m_fileWatcher.addWatch(gScriptManager::Get()_path, &g_FileWatchListener);
    }

    m_fileHasChanged = false;
}
ScriptSystem::~ScriptSystem()
{
    /*m_fileWatcher.removeWatch(m_watchid);*/
}

void ScriptSystem::onEntityAdded(Entity& entity)
{
    // TODO: *** CRITICAL: Need proper unique module implementation, non-unique modules WILL cause a crash
    entity.getComponent<ScriptComponent>().module = "mod" + std::to_string(m_id_generator++); // <- This is unacceptable, but it works for now
    
    ScriptManager::Get()->compile(entity.getComponent<ScriptComponent>());
}

void ScriptSystem::onEntityRemoved(Entity& entity)
{
    ScriptManager::Get()->removeModule(entity.getComponent<ScriptComponent>().module);
}

void ScriptSystem::update()
{
    if (SCRIPT_EDIT_CONTINUE_SUPPORT) { /*m_fileWatcher.update();*/ }

    auto& entities = getEntities();

    for (auto& entity : entities) {

        auto& descriptorComponent = entity.getComponent<DescriptorComponent>();
        auto& scriptComponent = entity.getComponent<ScriptComponent>();

        if (SCRIPT_EDIT_CONTINUE_SUPPORT) {
            if (m_fileHasChanged && std::string("content/script/" + scriptComponent.script + ".asc") == m_modifiedFile) {

                if (scriptComponent.initFunc) {
                    scriptComponent.initFunc->Release(); 
                    scriptComponent.initFunc = nullptr;
                }
                if (scriptComponent.updateFunc) {
                    scriptComponent.updateFunc->Release();
                    scriptComponent.updateFunc = nullptr;
                }
                if (scriptComponent.destroyFunc) {
                    scriptComponent.destroyFunc->Release();
                    scriptComponent.destroyFunc = nullptr;
                }
                if (scriptComponent.onPlayerInteractionFunc) {
                    scriptComponent.onPlayerInteractionFunc->Release();
                    scriptComponent.onPlayerInteractionFunc = nullptr;
                }
                if (scriptComponent.onKillEventFunc) {
                    scriptComponent.onKillEventFunc->Release();
                    scriptComponent.onKillEventFunc = nullptr;
                }
                if (scriptComponent.onUseEventFunc) {
                    scriptComponent.onUseEventFunc->Release();
                    scriptComponent.onUseEventFunc = nullptr;
                }
                if (scriptComponent.onLogicEventActivate) {
                    scriptComponent.onLogicEventActivate->Release();
                    scriptComponent.onLogicEventActivate = nullptr;
                }

                ScriptManager::Get()->removeModule(scriptComponent.module);

				ScriptManager::Get()->compile(scriptComponent);

                if (SCRIPT_EDIT_CONTINUE_REINIT) {
                    if (scriptComponent.hasInit) {
                        ScriptManager::Get()->execute(
                            scriptComponent, scriptComponent.initFunc, descriptorComponent.id);
                    }
                }

                m_fileHasChanged = false;
            }
        }

        if (scriptComponent.active && scriptComponent.initialized) {

            if (scriptComponent.hasUpdate) {
				ScriptManager::Get()->execute(
                    scriptComponent, scriptComponent.updateFunc, descriptorComponent.id);
            }

            continue;
        }

        if (!scriptComponent.initialized) {
            if (scriptComponent.hasInit) {
				ScriptManager::Get()->execute(
                    scriptComponent, scriptComponent.initFunc, descriptorComponent.id);
            }

            scriptComponent.initialized = true;
            scriptComponent.active = true;
        }
    }
}

void ScriptSystem::compile(ScriptComponent &script)
{
    // TODO: Need proper unique module implementation and checking if a module already exists
    script.module = Utility::FilenameFromPath(_asset_asc(script.script));

    ScriptManager::Get()->compile(script);
}

void ScriptSystem::execute(ScriptComponent &script, unsigned int id)
{
    ScriptManager::Get()->execute(
        script, script.onUseEventFunc, id);
}

//void ScriptSystem::exportScript(const std::string& path, const std::string& module)
//{
//    ScriptManager::Get()->exportScriptByteCode(path, module);
//}
//
//void ScriptSystem::importScript(const std::string& path, const std::string& module)
//{
//    ScriptManager::Get()->importScriptByteCode(path, module);
//}
