/*==============================================================================

    AABB Collision [Collision.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Collision.h"
#include <cmath>
using namespace DirectX;

bool Collision_IsOverlapCircle(const Circle& a, const Circle& b)
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

bool Collision_IsOverlapBox(const Box& a, const Box& b)
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

bool Collision_IsOverlapAABB(const AABB& a, const AABB& b)
{
    return a.Min.x < b.Max.x
        && a.Max.x > b.Min.x
        && a.Min.y < b.Max.y
        && a.Max.y > b.Min.y
        && a.Min.z < b.Max.z
        && a.Max.z > b.Min.z;
}

IsHit Collision_IsHitlapAABB(const AABB& a, const AABB& b)
{
    IsHit Hit;

    Hit.Is_Hit = Collision_IsOverlapAABB(a, b);


    return Hit;
}
