#include "Engine/Sound/SoundManager.h"

#include <fstream>

#include <cereal/archives/xml.hpp>
#include <spdlog/spdlog.h>

#include "Utility/Utility.h"

SoundManager* SoundManager::s_Instance = nullptr;

SoundManager::SoundManager() : m_soundEngine(irrklang::createIrrKlangDevice())
{
    if (s_Instance)
    {
        Utility::Error("Pointer to class \'SoundManager\' is invalid");
    }
    s_Instance = this;

    if (!m_soundEngine)
    {
        Utility::Error("Failed to create the sound engine");
    }

    spdlog::info("IrrKlang Version {}", IRR_KLANG_VERSION);

	try
	{
		std::ifstream ifs_render("config/sound.xml");
		cereal::XMLInputArchive sound_config(ifs_render);

		sound_config(m_configuration);
	}
	catch (cereal::Exception& ex)
	{
		spdlog::warn("Failed to load sound configuration: {}, default values used", ex.what());

		m_configuration = SoundConfiguration();

		std::ofstream ofs_render("config/sound.xml");
		cereal::XMLOutputArchive sound_config(ofs_render);

		sound_config(m_configuration);
	}

	m_soundEngine->setSoundVolume(m_configuration.volume);
}
SoundManager::~SoundManager()
{
    m_soundEngine->removeAllSoundSources();
    m_soundEngine->drop();

    delete s_Instance;
}

void SoundManager::saveConfiguration(SoundConfiguration configuration)
{
	std::ofstream ofs_sound("config/sound.xml");
	cereal::XMLOutputArchive sound_config(ofs_sound);

	m_configuration = configuration;
	sound_config(configuration);
}