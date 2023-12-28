#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "anax/anax.hpp"

#include "Engine/World/Components.h"

//#include "Utility/FileWatcher/FileWatcher.h"


// TODO: Only works for the base script directory
//       Also MAY cause crash if some how more than one script is changed at the same time
#define SCRIPT_EDIT_CONTINUE_SUPPORT  false
#define SCRIPT_EDIT_CONTINUE_REINIT   false

class ScriptManager;

class ScriptSystem
    : public anax::System<anax::Requires<DescriptorComponent, ScriptComponent>>
{
public:
    // NOTE: A more appropiate place for edit-continue functionality would be the ScriptManager
    /*class FileWatcherUpdateListener : public FW::FileWatchListener
    {
    public:
        FileWatcherUpdateListener() {}

        void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
                              FW::Action action) override
        {
            m_fileHasChanged = true;
            m_modifiedFile = dir + filename;
        }
    };*/

    ScriptSystem();
    ~ScriptSystem();

    void onEntityAdded(anax::Entity& entity) override;
    void onEntityRemoved(anax::Entity& entity) override;

    void update();

    void compile(ScriptComponent &script);
    // ID is optional, defaults to ENTITY_NULL_VALUE
    void execute(ScriptComponent &script, unsigned int id = 0xFFFF);

    //void exportScript(const std::string& path, const std::string& module);
   // void importScript(const std::string& path, const std::string& module);

private:
    //FW::FileWatcher m_fileWatcher;
    //FW::WatchID m_watchid;

    static bool m_fileHasChanged;
    static std::string m_modifiedFile;

    unsigned long m_id_generator;
};
