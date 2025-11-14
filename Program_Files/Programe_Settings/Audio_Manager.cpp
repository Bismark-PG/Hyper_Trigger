/*==============================================================================

    Manage Audio with Singleton pattern [Audio_Manager.cpp]

    Author : Choi HyungJoon

    - Note
        > Make With Singleton Pattern
        > Function Type  : Pointer

==============================================================================*/
#include "Audio_Manager.h"
#include <assert.h>
#include <mmsystem.h>
#include <algorithm>

// Reset Pointer
Audio_Manager* Audio_Manager::Sound_Instance = nullptr;

// SFX Lifetime Manager
// If SFX Play Done, Destroy Voice
class Audio_Manager::Voice_Callback : public IXAudio2VoiceCallback
{
public:
    Voice_Callback(std::vector<IXAudio2SourceVoice*>& activeVoices, std::mutex& mutex)
        : m_ActiveVoices(activeVoices), m_Mutex(mutex) {
    }
    ~Voice_Callback() {}

    // If End Buffer, Call This
    void OnBufferEnd(void* pBufferContext) override
    {
        IXAudio2SourceVoice* pSourceVoice = (IXAudio2SourceVoice*)pBufferContext;

        std::lock_guard<std::mutex> lock(m_Mutex);

        // Find the right Voice
        auto Find = std::find(m_ActiveVoices.begin(), m_ActiveVoices.end(), pSourceVoice);

        if (Find != m_ActiveVoices.end())
            m_ActiveVoices.erase(Find);

        // If No More Needed, Destroy Voice
        pSourceVoice->DestroyVoice();
    }

    // Unused used Callback Function
    void OnStreamEnd() override {}
    void OnVoiceProcessingPassEnd() override {}
    void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
    void OnBufferStart(void* pBufferContext) override {}
    void OnLoopEnd(void* pBufferContext) override {}
    void OnVoiceError(void* pBufferContext, HRESULT Error) override {}

private:
    // Make Creator In Private << Most Important
    std::vector<IXAudio2SourceVoice*>& m_ActiveVoices;
    std::mutex& m_Mutex;
};

// Singleton Instance Accessor
Audio_Manager* Audio_Manager::GetInstance()
{
    if (Sound_Instance == nullptr)
        Sound_Instance = new Audio_Manager();

    return Sound_Instance;
}

// Creator / Extinction
Audio_Manager::Audio_Manager()
    : X_Audio(nullptr), m_pMasteringVoice(nullptr),
	Current_BGM_Volume(0.5f), Target_BGM_Volume(0.5f), Target_SFX_Volume(0.5f), // Default Volume : 50%
    Voice_Call_back(nullptr), Now_Playing_BGM_Name("")
{


}

Audio_Manager::~Audio_Manager()
{
    // Safety Device Code
    if (X_Audio)
        Final();
}

// Initialize
void Audio_Manager::Init()
{
    // Prevent Multiple Initialization
    if (X_Audio) return;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        MessageBoxW(nullptr, L"Failed To Set Function Int Audio_Manager.cpp : [CoInitializeEx]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
    }

    XAudio2Create(&X_Audio, 0);
    assert(X_Audio);
    X_Audio->CreateMasteringVoice(&m_pMasteringVoice);
    assert(m_pMasteringVoice);

    Voice_Call_back = new Voice_Callback(Active_SFX_Voices, Voice_Mutex);
}

// Finalize
void Audio_Manager::Final()
{
    if (!X_Audio) return;

    Stop_All_SFX();

    // Unload All BGM
    for (auto const& Find : BGMs)
    {
        if (Find.second.Source)
        {
            Find.second.Source->Stop();
            Find.second.Source->DestroyVoice();
        }

        delete[] Find.second.Data;
    }

    BGMs.clear();

    Now_Playing_BGM_Name = "";

    // Unload All SFX
    for (auto const& Find : SFXs)
        delete[] Find.second.Data;

    SFXs.clear();

    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }

    if (X_Audio)
    {
        X_Audio->Release();
        X_Audio = nullptr;
    }

    delete Voice_Call_back;
    Voice_Call_back = nullptr;

    CoUninitialize();
}

// Get Sound Data for Audio file
bool Audio_Manager::Load_BGM(const std::string& name, const char* pFilePath)
{
    // Prevent Multiple Loading
    if (BGMs.find(name) != BGMs.end())
    {
        return false;
    }

    Sound_Data New_BGMData;
    if (Load_Wave_File(pFilePath, New_BGMData))
    {
        // For BGM, Create Permanent Use.
        X_Audio->CreateSourceVoice(&New_BGMData.Source, &New_BGMData.WAVE_Format);
        assert(New_BGMData.Source);
        BGMs[name] = New_BGMData;
        return true;
    }
    return false;
}

bool Audio_Manager::Load_SFX(const std::string& name, const char* pFilePath)
{
    // Prevent Multiple Loading
    if (SFXs.find(name) != SFXs.end())
    {
        return false;
    }

    Sound_Data New_SFXData;
    if (Load_Wave_File(pFilePath, New_SFXData))
    {
        SFXs[name] = New_SFXData;
        return true;
    }
    return false;
}

// Unload Sound Data
void Audio_Manager::Unload_BGM(const std::string& name)
{
    auto Find = BGMs.find(name);
    if (Find != BGMs.end())
    {
        if (Find->second.Source)
        {
            Find->second.Source->Stop();
            Find->second.Source->DestroyVoice();
        }
        delete[] Find->second.Data;
        BGMs.erase(Find);

        if (name == Now_Playing_BGM_Name)
            Now_Playing_BGM_Name = "";
    }
}

void Audio_Manager::Unload_SFX(const std::string& name)
{
    auto Find = SFXs.find(name);
    if (Find != SFXs.end())
    {
        delete[] Find->second.Data;
        SFXs.erase(Find);
    }
}

// Manage Sound Player
void Audio_Manager::Play_BGM(const std::string& name, bool bLoop)
{
    auto Find = BGMs.find(name);
    if (Find == BGMs.end() || !Find->second.Source)
    {
        return;
    }

    Sound_Data& BGM_Data = Find->second;

    BGM_Data.Source->Stop();
    BGM_Data.Source->FlushSourceBuffers();

    XAUDIO2_BUFFER Buffer{};
    Buffer.AudioBytes = BGM_Data.Length;
    Buffer.pAudioData = BGM_Data.Data;
    Buffer.PlayBegin = 0;
    Buffer.PlayLength = BGM_Data.Play_Length;

    if (bLoop)
    {
        Buffer.LoopBegin = 0;
        Buffer.LoopLength = BGM_Data.Play_Length;
        Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    BGM_Data.Source->SubmitSourceBuffer(&Buffer, NULL);
    BGM_Data.Source->SetVolume(Current_BGM_Volume);
    BGM_Data.Source->Start();

    Now_Playing_BGM_Name = name;
}

void Audio_Manager::Stop_BGM(const std::string& name)
{
    auto Find = BGMs.find(name);

    if (Find != BGMs.end() && Find->second.Source)
        Find->second.Source->Stop();

    if (name == Now_Playing_BGM_Name)
        Now_Playing_BGM_Name = "";
}

void Audio_Manager::Stop_BGM()
{
    for (auto const& Find : BGMs)
    {
        if (Find.second.Source)
            Find.second.Source->Stop();
    }

    Now_Playing_BGM_Name = "";
}

void Audio_Manager::Play_SFX(const std::string& name)
{
    auto Find = SFXs.find(name);

    if (Find == SFXs.end())
        return;

    Sound_Data& SFX_Data = Find->second;

    // For SFX, Create Temporary, Will be Destroyed After Playback.
    IXAudio2SourceVoice* Source_Voice{};
    X_Audio->CreateSourceVoice(&Source_Voice, &SFX_Data.WAVE_Format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, Voice_Call_back);

    XAUDIO2_BUFFER Buffer{};
    Buffer.AudioBytes = SFX_Data.Length;
    Buffer.pAudioData = SFX_Data.Data;
    Buffer.PlayBegin = 0;
    Buffer.PlayLength = SFX_Data.Play_Length;
    Buffer.pContext = Source_Voice; // Deliver Voice Pointers to Callback

    Source_Voice->SubmitSourceBuffer(&Buffer, NULL);
    Source_Voice->SetVolume(Target_SFX_Volume);

    {
        std::lock_guard<std::mutex> lock(Voice_Mutex);
        Active_SFX_Voices.push_back(Source_Voice);
    }

    Source_Voice->Start();
}

void Audio_Manager::Stop_All_SFX()
{
    std::lock_guard<std::mutex> Lock(Voice_Mutex);
    for (auto Voice : Active_SFX_Voices)
    {
        Voice->Stop();
        Voice->DestroyVoice();
    }

    Active_SFX_Voices.clear();
}

// Manage Volume
void Audio_Manager::Set_Target_BGM_Volume(float volume)
{
    Target_BGM_Volume = std::max(0.0f, std::min(1.0f, volume));
    Update_Current_BGM_Volume(Target_BGM_Volume);
}

void Audio_Manager::Set_Target_SFX_Volume(float volume)
{
    Target_SFX_Volume = std::max(0.0f, std::min(1.0f, volume));
}

void Audio_Manager::Update_Current_BGM_Volume(float volume)
{
    Current_BGM_Volume = std::max(0.0f, std::min(1.0f, volume));

    for (auto const& pair : BGMs)
    {
        if (pair.second.Source)
            pair.second.Source->SetVolume(Current_BGM_Volume);
    }
}

float Audio_Manager::Get_Target_BGM_Volume() const
{
    return Target_BGM_Volume;
}

float Audio_Manager::Get_Current_BGM_Volume() const
{
    return Current_BGM_Volume;
}

float Audio_Manager::Get_Target_SFX_Volume() const
{
    return Target_SFX_Volume;
}

std::string Audio_Manager::Get_Playing_BGM_Name() const
{
    return Now_Playing_BGM_Name;
}

// Private Helper Function
bool Audio_Manager::Load_Wave_File(const char* pFilePath, Sound_Data& soundData)
{
    // Open 'wav' File
    HMMIO HMMIO_Open = mmioOpenA((LPSTR)pFilePath, NULL, MMIO_READ);
    if (!HMMIO_Open) return false;

    // Going down to 'RIFF' chunk
    MMCKINFO RIFF_Chunk = {};
    RIFF_Chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(HMMIO_Open, &RIFF_Chunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Going down to 'FMT' chunk
    MMCKINFO FMT_Chunk = {};
    FMT_Chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(HMMIO_Open, &FMT_Chunk, &RIFF_Chunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Read Format Info
    mmioRead(HMMIO_Open, (HPSTR)&soundData.WAVE_Format, sizeof(soundData.WAVE_Format));
    mmioAscend(HMMIO_Open, &FMT_Chunk, 0);

    // Going down to 'data' chunk
    MMCKINFO DTAT_Chunk = {};
    DTAT_Chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(HMMIO_Open, &DTAT_Chunk, &RIFF_Chunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Read Sound Data
    UINT32 Buffer_Len = DTAT_Chunk.cksize;
    soundData.Data = new BYTE[Buffer_Len];
    mmioRead(HMMIO_Open, (HPSTR)soundData.Data, Buffer_Len);

    soundData.Length = Buffer_Len;
    soundData.Play_Length = Buffer_Len / soundData.WAVE_Format.nBlockAlign;

    mmioClose(HMMIO_Open, 0);
    return true;
}
