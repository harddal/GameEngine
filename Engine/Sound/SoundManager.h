#pragma once

#include <memory>

#include <cereal/cereal.hpp>

#include "irrklang.h"

struct SoundConfiguration
{
	float volume;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(volume));
	}

	SoundConfiguration() : volume(1.0f) {}
};

class SoundManager
{
public:
	SoundManager& operator=(const SoundManager&) = delete;

    SoundManager();
    ~SoundManager();

	SoundConfiguration getConfiguration() const { return m_configuration; }
	void saveConfiguration(SoundConfiguration configuration);

    irrklang::ISoundEngine* sound() const { return m_soundEngine; }

    static SoundManager* Get() { return s_Instance; }

private:
    static SoundManager* s_Instance;

    irrklang::ISoundEngine* m_soundEngine;

	SoundConfiguration m_configuration;

};
