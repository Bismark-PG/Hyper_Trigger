/*==============================================================================

	Make Animation [Sprite_Animation.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SPRITE_ANIMATION_H
#define SPRITE_ANIMATION_H
#include "DirectXMath.h"

void SpriteAni_Initialize();
void SpriteAni_Finalize();

void SpriteAni_Update(double elapsed_time);
void SpriteAni_Draw(int PlayID, float dx, float dy, float dw, float dh,
	float angle = 0.0f, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

int SpriteAni_Get_Pattern_Info(int TexID, int PMax, int HPatternMax,
	double PatternPlayTime, const DirectX::XMUINT2& PSize,
	const DirectX::XMUINT2& SPosition, bool Is_Loop);

int SpriteAni_CreatePlayer(int AniPatternID);

bool SpriteAni_IsStopped(int Index);
void SpriteAni_DestroyPlayer(int Index);

#endif // SPRITE_ANIMATION_H
