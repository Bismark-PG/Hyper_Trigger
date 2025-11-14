/*==============================================================================

    フィールド描画用ピクセルシェーダー [shader_pixel_field.hlsl]

    Author : Choi HyungJoon

==============================================================================*/
struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;    // Not Used
    float4 normalW : NORMAL0;   // Not Used
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex0 : register(t0); //テクスチャ (Grass)
Texture2D tex1 : register(t1); //テクスチャ (Rock)


SamplerState samp : register(s0); //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET
{ 
    float2 uv;
    float angle = 3.141592f * 45 / 180.0f;
    uv.x = pi.uv.x * cos(angle) + pi.uv.y * sin(angle);
    uv.y = -pi.uv.x * sin(angle) + pi.uv.y * cos(angle);
    
    // Blender G and R
    return tex0.Sample(samp, uv) * pi.color.g + tex1.Sample(samp, pi.uv) * pi.color.r;
}