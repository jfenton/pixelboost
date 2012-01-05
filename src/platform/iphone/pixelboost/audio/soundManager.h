#pragma once

#ifdef PIXELBOOST_PLATFORM_IOS

#include <map>
#include <string>

namespace pixelboost
{
    
class Sound
{
private:
    Sound(int id, const std::string& name="", float volume=1.f, float pitch=1.f);
    
public:
    int GetId() const;
    
    void Play();
    void Stop();
    bool IsPlaying() const;
    
    const std::string& GetName() const;
    
    bool IsLooping() const;
    void SetLooping(bool looping);
    
    float GetPitch() const;
    void SetPitch(float pitch);
    
    float GetVolume() const;
    void SetVolume(float volume);
    
private:
    int _Id;
    bool _Looping;
    std::string _Name;
    float _Pitch;
    float _Volume;
    
    friend class SoundManager;
};

class SoundManager
{
private:
    SoundManager();
    ~SoundManager();
    
public:
    static SoundManager* Instance();
    
    void Update(float time);
    
    void MuteBgm(bool mute=true);
    void MuteSfx(bool mute=true);
    
    void LoadBgm(const std::string& name);
    void LoadSfx(const std::string& name, bool compressed);
    
    void PlayBgm(const std::string& name, bool loop, float volume=1.f);
    void StopBgm();
    
    Sound CreateSfx(const std::string& name, bool compressed, float volume=1.f, float pitch=1.f);
    Sound PlaySfx(const std::string& name, bool compressed, float volume=1.f, float pitch=1.f);
    
private:
    void SfxPlay(const Sound& sound);
    void SfxStop(const Sound& sound);
    bool SfxIsPlaying(const Sound& sound);
    void SfxUpdateLooping(const Sound& sound);
    void SfxUpdatePitch(const Sound& sound);
    void SfxUpdateVolume(const Sound& sound);
    
private:
    int _SoundId;
    std::map<int, void*> _Sounds;
    
    std::string _CurrentBgmName;
    bool _CurrentBgmLoop;
    bool _CurrentBgmVolume;
    
    bool _MuteBgm;
    bool _MuteSfx;
    
    friend class Sound;
};
    
}

#endif
