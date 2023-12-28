#include "Application.h"

#include "Engine/Engine.h"
#include "Utility/Utility.h"

#ifdef NDEBUG
    int __stdcall WinMain(
        _In_	 HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_	 LPSTR lpCmdLine,
        _In_	 int nCmdShow)
#else
    int main(int argc, char** argv)
#endif
    { 
#ifdef NDEBUG
        auto* app = new Application("Engine", std::string(lpCmdLine));
#else
        std::string args;
        for (auto i = 0; i < argc; i++) { args += argv[i]; }

        auto* app = new Application("Engine", args);
#endif

        app->update();

        delete app;

        return EXIT_SUCCESS;
    }

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, const std::string& args) : m_running(true), m_engine(nullptr)
{
    m_log_console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
    m_log_console_sink->set_level(spdlog::level::trace);

    m_log_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log/output.log", true);
    m_log_file_sink->set_level(spdlog::level::trace);

    m_log = std::make_shared<spdlog::logger>(spdlog::logger("core", { m_log_console_sink, m_log_file_sink } ));
    set_default_logger(m_log);
    m_log->set_level(spdlog::level::trace);

    if (s_Instance)
    {
        Utility::Error("Pointer to class \'Application\' is invalid");
    }
    s_Instance = this;

    m_engine = new Engine(name, args);

	m_engine->init();
}
Application::~Application()
{
    m_log->flush();

    spdlog::shutdown();

    delete m_engine;
    delete s_Instance;
}

void Application::update()
{
    m_engine->update();
}
