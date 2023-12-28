#pragma once

#include <string>

#include "anax/Component.hpp"
#include "irrklang.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

using namespace irrklang;

struct sSoundData
{
	ISound* sound;
    ISoundSource* source;

    std::string file, name;

    bool
        play, 
        is3D, 
        startPaused, 
        loop, 
        isPlaying;

    float
        volume, minDist, maxDist;

    sSoundData() : 
        sound(nullptr), source(nullptr), 
        play(false), is3D(false), startPaused(true), loop(false), isPlaying(false),
        volume(100.0f), minDist(0.5f), maxDist(1.0f) {}
    sSoundData(std::string file, std::string name, bool is3D = false, bool loop = false,
        float volume = 100.0f, float minDist = 0.5f, float maxDist = 1.0f, bool startpaused = true)
    {
        sound = nullptr;
        source = nullptr;

        this->file = file;
        this->name = name;
        this->is3D = is3D;
        this->loop = loop;
        this->volume = volume;
        this->minDist = minDist;
        this->maxDist = maxDist;
        this->startPaused = startpaused;
    }
};

struct SoundComponent : anax::Component
{
    std::vector<sSoundData> sounds;

	void add(sSoundData sound);

    void play(std::string name)
    { 
		for (auto i = 0U; i < sounds.size(); i++) {
		    if (sounds[i].name == name) {
		        sounds[i].play = true; 
		        return;
		    }
		}
    }

	template <class Archive>
	void serialize(Archive& archive)
	{
		for (auto s : sounds)
            archive(CEREAL_NVP(s.name), CEREAL_NVP(s.file),
				    CEREAL_NVP(s.loop), CEREAL_NVP(s.minDist), 
				    CEREAL_NVP(s.volume), CEREAL_NVP(s.is3D), 
				    CEREAL_NVP(s.startPaused));
	}
};
