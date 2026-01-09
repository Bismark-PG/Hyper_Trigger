/*==============================================================================

    AABB Collision [Collision.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Collision.h"
#include <cmath>
#include <algorithm>
using namespace DirectX;

bool Collision_Is_Overlap_Circle(const Circle& a, const Circle& b)
{ 
    float x1 = b.Center.x - a.Center.x;
    float y1 = b.Center.y - a.Center.y;

    return pow((a.Radius + b.Radius), 2) > (pow(x1, 2) + pow(y1, 2));

    // DirectX
    /*
    XMVECTOR ac = XMLoadFloat2(&a.Center);
    XMVECTOR bc = XMLoadFloat2(&b.Center);
    XMVECTOR lsq = XMVector2LengthSq(bc - ac);

    return pow(a.Radius + b.Radius, 2) > XMVectorGetX(lsq);
    */
}

bool Collision_Is_Overlap_Box(const Box& a, const Box& b)
{
    float At = a.Center.y - a.Half_Height;
    float Ab = a.Center.y + a.Half_Height;
    float Al = a.Center.x - a.Half_Width;
    float Ar = a.Center.x + a.Half_Width;
    float Bt = b.Center.y - b.Half_Height;
    float Bb = b.Center.y + b.Half_Height;
    float Bl = b.Center.x - b.Half_Width;
    float Br = b.Center.x + b.Half_Width;

    return Al < Br
        && Ar > Bl
        && At < Bb
        && Ab > Bt;
}

bool Collision_Is_Overlap_AABB(const AABB& a, const AABB& b)
{
    return a.Min.x < b.Max.x
        && a.Max.x > b.Min.x
        && a.Min.y < b.Max.y
        && a.Max.y > b.Min.y
        && a.Min.z < b.Max.z
        && a.Max.z > b.Min.z;
}

IsHit Collision_Is_Hit_AABB(const AABB& a, const AABB& b)
{
    IsHit Hit = { false, {0.0f, 0.0f, 0.0f}, 0.0f };

    // Overlap Check
    if (!Collision_Is_Overlap_AABB(a, b))
    {
		return Hit; // No Hit
    }

    Hit.Is_Hit = true;

	// Overlap Depth Check
    float a_min[3] = { a.Min.x, a.Min.y, a.Min.z };
    float a_max[3] = { a.Max.x, a.Max.y, a.Max.z };
    float b_min[3] = { b.Min.x, b.Min.y, b.Min.z };
    float b_max[3] = { b.Max.x, b.Max.y, b.Max.z };

    float overlap[3] = { 0.0f, 0.0f, 0.0f }; // x >> y >> z

    // Get Depth >> Min(A.max, B.max) - Max(A.min, B.min)
    for (int i = 0; i < 3; ++i)
    {
        float overlap_min = std::max(a_min[i], b_min[i]);
        float overlap_max = std::min(a_max[i], b_max[i]);
        overlap[i] = overlap_max - overlap_min;
    }


	// Smallest Overlap Depth Check
	// Smallest Depth == Push Direction
	// If X is the smallest
    if (overlap[0] < overlap[1] && overlap[0] < overlap[2])
    {
        Hit.Depth = overlap[0];
		// If A center is less than B center, Push left (-1), else right (1)
        float a_center = (a.Min.x + a.Max.x) * 0.5f;
        float b_center = (b.Min.x + b.Max.x) * 0.5f;
        Hit.Normal = { (a_center < b_center) ? -1.0f : 1.0f, 0.0f, 0.0f };
    }
	// If Y is the smallest
    else if (overlap[1] < overlap[0] && overlap[1] < overlap[2])
    {
        Hit.Depth = overlap[1];
        float a_center = (a.Min.y + a.Max.y) * 0.5f;
        float b_center = (b.Min.y + b.Max.y) * 0.5f;
        Hit.Normal = { 0.0f, (a_center < b_center) ? -1.0f : 1.0f, 0.0f };
    }
	// If Z is the smallest
    else
    {
        Hit.Depth = overlap[2];
        float a_center = (a.Min.z + a.Max.z) * 0.5f;
        float b_center = (b.Min.z + b.Max.z) * 0.5f;
        Hit.Normal = { 0.0f, 0.0f, (a_center < b_center) ? -1.0f : 1.0f };
    }

    return Hit;
}