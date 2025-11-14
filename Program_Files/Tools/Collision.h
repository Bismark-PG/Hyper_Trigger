/*==============================================================================

	Collision [Collision.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef COLLISION_H
#define COLLISION_H

#include <DirectXMath.h>

struct Circle
{
	DirectX::XMFLOAT2 Center;
	float Radius;
};

struct Box
{
	DirectX::XMFLOAT2 Center;
	float Half_Width;
	float Half_Height;
};

struct AABB
{
	DirectX::XMFLOAT3 Max;
	DirectX::XMFLOAT3 Min;
};

struct IsHit
{
	bool Is_Hit;
	DirectX::XMFLOAT3 Normal;
};

bool Collision_IsOverlapCircle(const Circle& a, const Circle& b);
bool Collision_IsOverlapBox(const Box& a, const Box& b);
bool Collision_IsOverlapAABB(const AABB& a, const AABB& b);
IsHit Collision_IsHitlapAABB(const AABB& a, const AABB& b); // use circle??
// use circle >> main midddle is Y, 360, can see rotae

#endif // COLLISION_H