#pragma once

#include <string>
#include <memory>

#include "resource.h"

#include "Engine/Engine.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main(int argc, char** argv);

class Application
{
public:
    explicit Application(const std::string& name = std::string(), const std::string& args = std::string());
    ~Application();

    void update();

    void exit() { m_running = false; }

    static Application& Get() { return *s_Instance; }

private:
    static Application* s_Instance;

    bool m_running;

    Engine *m_engine;

    std::shared_ptr<spdlog::logger> m_log;
    std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> m_log_console_sink;
    std::shared_ptr<spdlog::sinks::basic_file_sink_mt> m_log_file_sink;
};