/*==============================================================================

    Manage Audio with Singleton pattern [Audio_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <xaudio2.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>

struct Sound_Data
{
    IXAudio2SourceVoice* Source{};
    WAVEFORMATEX         WAVE_Format{};
    BYTE*                Data{};
    int                  Length{};
    int                  Play_Length{};
};

class Audio_Manager
{
public:
    // Get Audio Manager Instance
    static Audio_Manager* GetInstance();

    // Reset Audio Manager
    void Init();
    void Final();

    // Get Sound Data for Audio file
    // Success : true, Fail or Already load : false
    bool Load_BGM(const std::string& name, const char* pFilePath);
    bool Load_SFX(const std::string& name, const char* pFilePath);

    // Unload Sound Data
    void Unload_BGM(const std::string& name);
    void Unload_SFX(const std::string& name);

    // Manage Sound Player
    void Play_BGM(const std::string& name, bool bLoop = true);
    void Stop_BGM(const std::string& name);
    
    // Stop All BGM
    void Stop_BGM();
    
    void Play_SFX(const std::string& name);
    void Stop_All_SFX();

    // Manage Volume
    void Set_Target_BGM_Volume(float volume);
    void Set_Target_SFX_Volume(float volume);

    void Update_Current_BGM_Volume(float volume);

    float Get_Target_BGM_Volume() const;
    float Get_Current_BGM_Volume() const;
    float Get_Target_SFX_Volume() const;

    std::string Get_Playing_BGM_Name() const;

private:
    Audio_Manager();
    ~Audio_Manager();
    Audio_Manager(const Audio_Manager&) = delete;
    Audio_Manager& operator=(const Audio_Manager&) = delete;

    bool Load_Wave_File(const char* pFilePath, Sound_Data& soundData);

private:
    static Audio_Manager* Sound_Instance;

    IXAudio2* X_Audio;
    IXAudio2MasteringVoice* m_pMasteringVoice;

    std::map<std::string, Sound_Data> BGMs;
    std::map<std::string, Sound_Data> SFXs;

    std::vector<IXAudio2SourceVoice*> Active_SFX_Voices;
    std::mutex                        Voice_Mutex;

    float Current_BGM_Volume;
    float Target_BGM_Volume;
    
    float Target_SFX_Volume;

    // Save Playing BGM Info
    std::string Now_Playing_BGM_Name;

    // Callback Handler for SFX
    class Voice_Callback;
    Voice_Callback* Voice_Call_back;
};

extern Audio_Manager* Sound_M;

#endif // AUDIO_MANAGER_H

