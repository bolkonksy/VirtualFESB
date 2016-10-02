#ifndef _AUDIOENGINE_H
#define _AUDIOENGINE_H

#include <map>
#include <string>
#include <SDL/SDL_mixer.h>

namespace Engine
{
    class SoundEffect {
    public:
        friend class AudioEngine;
        
        /* if loops == -1, plays forever 
           otherwise plays it loops + 1 times */
        void play(int loops = 0);

    private:
        Mix_Chunk* _chunk = nullptr;
    };

    class Music {
    public:
        friend class AudioEngine;

        /* if loops == -1, plays forever 
           otherwise plays it loops times */
        void play(int loops = 1);

        /* Remove static? */
        static void pause();
        static void stop();
        static void resume();

    private:
        Mix_Music* _music = nullptr;
    };

    /* Audio wrapper for caching music and sound effects */
    class AudioEngine {
    public:
        ~AudioEngine();

        bool init();
        void destroy();

        /* Takes volume in range 0-100 */
        void setVolume(int volume);
        int  getVolume();

        SoundEffect loadSoundEffect(const std::string& filePath);
        Music loadMusic(const std::string& filePath);

    private:
        std::map<std::string, Mix_Chunk*> _effectMap;
        std::map<std::string, Mix_Music*> _musicMap;
    };
}

#endif