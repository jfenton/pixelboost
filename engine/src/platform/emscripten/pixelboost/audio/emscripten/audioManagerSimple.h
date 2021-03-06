#pragma once

#include <string>

#include "pixelboost/audio/audioManagerSimple.h"

namespace pb
{

    class AudioManagerSimpleEmscripten : public AudioManagerSimple
    {
    public:
        AudioManagerSimpleEmscripten();
        ~AudioManagerSimpleEmscripten();
        
    public:
        void Update(float time);
        
        void MuteBgm(bool mute=true);
        void MuteSfx(bool mute=true);
        
        void LoadBgm(const std::string& name);
        void LoadSfx(const std::string& name, bool compressed);
        
        void PlayBgm(const std::string& name, bool loop, float volume=1.f);
        void StopBgm();
        
        void PlaySfx(const std::string& name, bool compressed, float volume=1.f, float pitch=1.f);
        
    private:
        std::string _CurrentBgmName;
        bool _CurrentBgmLoop;
        bool _CurrentBgmVolume;
        
        bool _MuteBgm;
        bool _MuteSfx;
    };
    
}
