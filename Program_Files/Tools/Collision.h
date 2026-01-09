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
	float Depth;
};

bool Collision_Is_Overlap_Circle(const Circle& a, const Circle& b);
bool Collision_Is_Overlap_Box(const Box& a, const Box& b);
bool Collision_Is_Overlap_AABB(const AABB& a, const AABB& b);
IsHit Collision_Is_Hit_AABB(const AABB& a, const AABB& b); // use circle??
// use circle >> main midddle is Y, 360, can see rotae

#endif // COLLISION_H