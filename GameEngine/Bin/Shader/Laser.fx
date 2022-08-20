
#include "Transparent3D.fx"

Vertex3DOutput LaserVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;

    float2 RandomUV = float2(g_AccTime, g_AccTime);
    float4 Random = g_NoiseTexture.SampleLevel(g_PointSmp, RandomUV, 0);

    input.Pos.x = input.Pos.x * Random.r / 2.f;
    
    output.Pos = mul(float4(input.Pos, 1.f), g_matWVP);
    
    output.ProjPos = output.Pos;
    output.UV = input.UV;
    
    output.ViewPos = mul(float4(input.Pos, 1.f), g_matWV).xyz;
    
    return output;
}

PSOutput_Single LaserPS(Vertex3DOutput Input)
{
	PSOutput_Single output = (PSOutput_Single) 0;
    
    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, Input.UV);
    
    if (BaseTextureColor.a == 0.f || g_MtrlOpacity == 0.f)
        clip(-1);
    
    output.Color.rgb = BaseTextureColor.rgb * g_MtrlBaseColor.rgb + g_MtrlEmissiveColor.rgb;
    
    output.Color.a = BaseTextureColor.a * g_MtrlOpacity;
    
    return output;
}