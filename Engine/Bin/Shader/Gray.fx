#include "ShaderInfo.fx"

Texture2DMS<float4> g_PostProcessTarget : register(t21);

struct VS_OUTPUT_GRAY
{
    float4 Pos : SV_POSITION;
    float4 ProjPos : POSITION;
};

VS_OUTPUT_GRAY GrayVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT_GRAY Output = (VS_OUTPUT_GRAY) 0;

	Output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
	Output.Pos = Output.ProjPos;
    
	return Output;
}

PSOutput_Single GrayPS(VS_OUTPUT_GRAY Input)
{
	PSOutput_Single Output = (PSOutput_Single) 0;

	float2 UV;
	UV.x = (Input.ProjPos.x / Input.ProjPos.w) * 0.5f + 0.5f;
	UV.y = (Input.ProjPos.y / Input.ProjPos.w) * -0.5f + 0.5f;

	int2 TargetPos;
	TargetPos.x = UV.x * g_Resolution.x;
	TargetPos.y = UV.y * g_Resolution.y;

	float4 ScreenColor = g_PostProcessTarget.Load(TargetPos, 0);

	float GrayColor = (ScreenColor.r + ScreenColor.g + ScreenColor.b) / 3.f;

	Output.Color.rgb = float3(GrayColor, GrayColor, GrayColor);
	Output.Color.a = 1.f;
	
	return Output;
}
