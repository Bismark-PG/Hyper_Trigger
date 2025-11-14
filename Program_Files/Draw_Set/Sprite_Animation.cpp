/*==============================================================================

	Make Animation [Sprite_Animation.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Sprite_Animation.h"
#include "Sprite.h"
#include "Texture_Manager.h"
using namespace DirectX;

struct AniPatternData
{
	// Set Texture Manage Number
	int TextureID{ -1 };
	// Animation Pattern Count
	int PatternMAX = 0;
	// Animation Horizontal Pattern Count
	int HorizonPatternMAX = 0;
	// Animation Start Range
	XMUINT2 StartPosition = { 0, 0 };
	// One Pattern Width, Height
	XMUINT2 PatternSize = { 0, 0 };
	// Time For One Pattern (Default 0.1 seconds)
	double PatternPlayTime = 0.1;
	// Check Last Pattern
	bool ISLooped{ true };
};

struct AniPlayData
{
	// Animation Pattern ID
	int PatternID = -1;
	// Current Pattern Count
	int PatternNUM = 0;
	// Accumulated Time
	double Accumulated_Time = 0.0;
	bool IsStopped{ false };
};

static constexpr int ANI_PATTERN_MAX = 128;
static AniPatternData g_AniPattern[ANI_PATTERN_MAX];
static constexpr int ANI_PLAY_MAX = 256;
static AniPlayData g_AniPlay[ANI_PLAY_MAX];

void SpriteAni_Initialize()
{
	// Initialize Animation Patterns
	for (AniPatternData& data : g_AniPattern)
		data.TextureID = -1; // Initialize Texture ID

	for (AniPlayData& data : g_AniPlay)
	{
		data.PatternID = -1;
		data.IsStopped = false;
	}
}

void SpriteAni_Finalize()
{

}

void SpriteAni_Update(double elapsed_time)
{
	for (int i = 0; i < ANI_PLAY_MAX; i++)
	{
		if (g_AniPlay[i].PatternID < 0)
			continue; // If Pattern ID Not Set, Can`t Update Animation, Continue Loop

		AniPatternData* pAniPatternData = &g_AniPattern[g_AniPlay[i].PatternID];
		g_AniPlay[i].Accumulated_Time += elapsed_time;

		if (g_AniPlay[i].Accumulated_Time >= pAniPatternData->PatternPlayTime)
		{
			g_AniPlay[i].Accumulated_Time -= pAniPatternData->PatternPlayTime;
			g_AniPlay[i].PatternNUM++;
		
			// Set Pattern Count
			if (g_AniPlay[i].PatternNUM >= pAniPatternData->PatternMAX)
			{
				if (pAniPatternData->ISLooped)  // Loop Animation
				{
					g_AniPlay[i].PatternNUM = 0;
				}
				else // Stop Animation at Last Pattern
				{
					g_AniPlay[i].PatternNUM = pAniPatternData->PatternMAX - 1;
					g_AniPlay[i].IsStopped = true;
				}
			}
		}
	}
}

void SpriteAni_Draw(int PlayID, float dx, float dy, float dw, float dh, float angle, const DirectX::XMFLOAT4& color)
{
	if (PlayID < 0 || PlayID >= ANI_PLAY_MAX || g_AniPlay[PlayID].PatternID < 0) return;

	int Ani_Pattern_ID = g_AniPlay[PlayID].PatternID;
	int Ani_Pattern_Num = g_AniPlay[PlayID].PatternNUM;
	AniPatternData* pAniPatternData = &g_AniPattern[Ani_Pattern_ID];

	if (pAniPatternData->TextureID < 0) return;

	float px = static_cast<float>(pAniPatternData->StartPosition.x
		+ pAniPatternData->PatternSize.x
		* (Ani_Pattern_Num % pAniPatternData->HorizonPatternMAX));

	float py = static_cast<float>(pAniPatternData->StartPosition.y
		+ pAniPatternData->PatternSize.y
		* (Ani_Pattern_Num / pAniPatternData->HorizonPatternMAX));

	float pw = static_cast<float>(pAniPatternData->PatternSize.x);
	float ph = static_cast<float>(pAniPatternData->PatternSize.y);


	Sprite_UV_Draw(pAniPatternData->TextureID, dx, dy, dw, dh, px, py, pw, ph, angle, color);
}

// Read Texture And Make Texture Info
int SpriteAni_Get_Pattern_Info(int TexID, int PMax, int HPatternMax,
	double PatternPlayTime,
	const XMUINT2& PSize, const XMUINT2& SPosition, bool Is_Loop)
{
	for (int i = 0; i < ANI_PATTERN_MAX; i++)
	{
		if (g_AniPattern[i].TextureID >= 0)
			continue;// If Texture ID Already Set, Continue Loop

		// If Not Set Texture ID, Make Texture Pattern Data
		g_AniPattern[i].TextureID = TexID;
		g_AniPattern[i].PatternMAX = PMax;
		g_AniPattern[i].HorizonPatternMAX = HPatternMax;
		g_AniPattern[i].PatternPlayTime = PatternPlayTime;
		g_AniPattern[i].PatternSize = PSize;
		g_AniPattern[i].StartPosition = SPosition;
		g_AniPattern[i].ISLooped = Is_Loop;

		return i; // Return Pattern ID
	}

	return -1; // If No More Space, Return -1
}

// Create Animation Player
int SpriteAni_CreatePlayer(int AniPatternID)
{
	for (int i = 0; i < ANI_PLAY_MAX; i++)
	{	
		if (g_AniPlay[i].PatternID >= 0)
			continue; // If Pattern ID Already Set, Continue Loop

		// If Not Set Pattern ID, Initialize Play Data
		g_AniPlay[i].PatternID = AniPatternID;
		g_AniPlay[i].PatternNUM = 0;
		g_AniPlay[i].Accumulated_Time = 0.0;
		g_AniPlay[i].IsStopped = false;

		return i; // Return Set Pattern ID
	}
	return -1; // If No More Space, Return -1
}

bool SpriteAni_IsStopped(int Index)
{
	if (Index < 0 || Index >= ANI_PLAY_MAX) return true;
	return g_AniPlay[Index].IsStopped;
}

void SpriteAni_DestroyPlayer(int Index)
{
	g_AniPlay[Index].PatternID = -1;
}
