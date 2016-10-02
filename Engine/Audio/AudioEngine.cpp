#include "AudioEngine.h"

#include <iostream>
#include "../ErrorHandler.h"

namespace Engine
{
    AudioEngine::~AudioEngine()
    {
        destroy();
    }

    bool AudioEngine::init()
    {
        if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) {
            std::cerr << "MixInit failed to initialize - " + std::string(Mix_GetError()) << std::endl;
            return false;
        }

        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            std::cerr << "MixOpenAudio failed to initialize - " + std::string(Mix_GetError()) << std::endl;
            return false;
        }

        /* Starting volume is 10% */
        setVolume(10);

        return true;
    }

    void AudioEngine::destroy()
    {
        for(auto& it : _effectMap)
            Mix_FreeChunk(it.second);
        for(auto& it : _musicMap)
            Mix_FreeMusic(it.second);
        _effectMap.clear();
        _musicMap.clear();

        Mix_CloseAudio();
        Mix_Quit();
    }

    void AudioEngine::setVolume(int volume)
    {
        if (volume < 0)
            volume = 0;
        else if (volume > 100)
            volume = 100;

        /* Avoid dividing with zero */
        if (volume == 0) {
            Mix_VolumeMusic(0);
            return;
        }

        float scale = volume / 100.0f;
        Mix_VolumeMusic((int)round(MIX_MAX_VOLUME * scale));
    }

    int AudioEngine::getVolume()
    {
        int volume = Mix_VolumeMusic(-1);

        /* Avoid dividing with zero */
        if (volume == 0)
            return 0;

        float scaleConverted = (float)volume / MIX_MAX_VOLUME;
        return (int)round(100 * scaleConverted);
    }

    SoundEffect AudioEngine::loadSoundEffect(const std::string& filePath)
    {
        SoundEffect effect;
        auto it = _effectMap.find(filePath);

        if (it == _effectMap.end()) {
            Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());

            if (chunk == nullptr)
                applicationError("MixLoadWav error: " + std::string(Mix_GetError()));

            effect._chunk = chunk;
            _effectMap[filePath] = chunk;
        }
        else
            effect._chunk = it->second;

        return effect;
    }

    Music AudioEngine::loadMusic(const std::string& filePath)
    {
        Music music;
        auto it = _musicMap.find(filePath);

        if (it == _musicMap.end()) {
            Mix_Music* mixMusic = Mix_LoadMUS(filePath.c_str());

            if (mixMusic == nullptr)
                applicationError("MixLoadWav error: " + std::string(Mix_GetError()));

            music._music = mixMusic ;
            _musicMap[filePath] = mixMusic ;
        }
        else
            music._music = it->second;

        return music;
    }

    void SoundEffect::play(int loops)
    {
        if (Mix_PlayChannel(-1, _chunk, loops) == -1)
            if (Mix_PlayChannel(1, _chunk, loops) == -1)
                applicationError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
    }

    void Music::play(int loops)
    {
        if (Mix_PlayMusic(_music, loops) == -1)
            applicationError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
    }

    void Music::pause()
    {
        Mix_PauseMusic();
    }

    void Music::stop()
    {
        Mix_HaltMusic();
    }

    void Music::resume()
    {
        Mix_ResumeMusic();
    }
}