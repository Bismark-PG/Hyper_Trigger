/*==============================================================================

    2D描画用ピクセルシェーダー [shader_pixel_2d.hlsl]

    Author : Choi HyungJoon

==============================================================================*/
struct PS_IN
{
    float4 posH  : SV_POSITION;
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

Texture2D tex;
SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{
    //    a(RGBA) * b(RGBA)
    // >> a.R * b.R
    //    a.G * b.G
    //    a.B * b.B
    //    a.A * b.A
    return tex.Sample(samp, pi.uv) * pi.color;
}
