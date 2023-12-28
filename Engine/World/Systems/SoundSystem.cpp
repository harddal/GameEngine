#include "SoundSystem.h"

#include <irrklang.h>

#include "Engine/Engine.h"

using namespace irrklang;

void SoundComponent::add(sSoundData sound)
{
	sounds.push_back(sound);

	//sounds.back().source = GameEngineInterface::get()->sound()->soundEngine()->addSoundSourceFromFile(sounds.back().file.c_str(), ESM_NO_STREAMING, true);
	//sounds.back().source->setDefaultMinDistance(sounds.back().minDist);
	//sounds.back().source->setDefaultVolume(sounds.back().volume);
}

void SoundSystem::onEntityAdded(anax::Entity& entity)
{
    auto& sounds = entity.getComponent<SoundComponent>().sounds;

    for (auto i = 0U; i < sounds.size(); i++) {
		auto src = SoundManager::Get()->sound()->getSoundSource(sounds[i].file.c_str(), false);
        if (src) {
			sounds[i].source = src;
        }
		else {
			sounds[i].source = SoundManager::Get()->sound()->addSoundSourceFromFile(sounds[i].file.c_str(), ESM_NO_STREAMING, true);
			if (!sounds[i].source) {
				//log::write("Failed to load sound file \'" + sounds[i].file + "\' in entity \'" + entity.getComponent<DescriptorComponent>().name + "\'", LOG_WARNING);
			}
		}
      //sounds[i].source->setDefaultMaxDistance(sounds[i].maxDist);
        sounds[i].source->setDefaultMinDistance(sounds[i].minDist);
        sounds[i].source->setDefaultVolume(sounds[i].volume / 100.0f);

		if (sounds[i].startPaused) {
		    sounds[i].play = false;
		}
		else {
		    sounds[i].play = true;
		}
        
    }
}

void SoundSystem::onEntityRemoved(anax::Entity& entity)
{
    for (auto s : entity.getComponent<SoundComponent>().sounds) {
		if (s.sound) {
            s.sound->stop();
			s.sound->drop();
		}

        // DEBUG: Kepp the sources loaded for later use, will be fixed when a resource manager/precaching system is implemented
		//if (s.source) {
  //          // BUG: Figure out a way to detect if a sound source is still be used, if not remove it
  //          // HACK: Only detects if a source is playing, but not if it is queued to play, temporary solution
  //          // BUG: Doesn't even stop sounds some playing after entity is destroyed 
  //          if (!_sound->soundEngine()->isCurrentlyPlaying(s.source)) {
  //              _sound->soundEngine()->removeSoundSource(s.source);
  //          }
		//}
    }
}

void SoundSystem::update()
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto& sounds = entity.getComponent<SoundComponent>().sounds;

        for (auto i = 0U; i < sounds.size(); i++) {
			if (sounds[i].is3D) {
				if (sounds[i].sound) {
					auto pos = entity.getComponent<TransformComponent>().getPosition();
					sounds[i].sound->setPosition(vec3df(pos.X, pos.Y, pos.Z));
				}
			}
            
            if (sounds[i].play && !sounds[i].loop) {
                if (sounds[i].is3D && entity.hasComponent<TransformComponent>()) {
                    auto pos = entity.getComponent<TransformComponent>().getPosition();

                    if (sounds[i].sound) {
						sounds[i].sound->drop();
                    }
					sounds[i].sound = SoundManager::Get()->sound()->play3D(
						sounds[i].source, vec3df(pos.X, pos.Y, pos.Z), sounds[i].loop, false, true);
                }
                else {
                    SoundManager::Get()->sound()->play2D(sounds[i].source, sounds[i].loop);
                }

                sounds[i].isPlaying = false;
            }
            else 
			if (sounds[i].play && sounds[i].loop && !sounds[i].isPlaying) {
                if (sounds[i].is3D && entity.hasComponent<TransformComponent>()) {
                    auto pos = entity.getComponent<TransformComponent>().getPosition();
                    
					if (sounds[i].sound) {
						sounds[i].sound->drop();
					}
					sounds[i].sound = SoundManager::Get()->sound()->play3D(
						sounds[i].source, vec3df(pos.X, pos.Y, pos.Z), sounds[i].loop, false, true);
                }
                else {
                    SoundManager::Get()->sound()->play2D(sounds[i].source, sounds[i].loop);
                }

                sounds[i].isPlaying = true;
            }
            
			if (!sounds[i].loop) {
				sounds[i].play = false;
			}
        }
    }
}
