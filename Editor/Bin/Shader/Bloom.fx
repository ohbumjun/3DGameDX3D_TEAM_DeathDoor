#include "ShaderInfo.fx"

// 1. BrightPass
Texture2D<float4> g_HDRDownScaleTex : register(t20);
StructuredBuffer<float> g_AvgLumBuffer : register(t10);
RWTexture2D<float4> g_BloomTex : register(u7);

[numthreads(1024, 1, 1)]
void BrightPass(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	uint2 CurPixel = uint2(DispatchThreadID.x % g_DownScaleRS.x, DispatchThreadID.x / g_DownScaleRS.x);

	if (CurPixel.y < g_DownScaleRS.y)
	{
		float4 Color = g_HDRDownScaleTex.Load(int3(CurPixel, 0));
		float Lum = dot(Color, LUM_FACTOR);
		float AvgLum = g_AvgLumBuffer[0];

		// 색상 스케일 계산
		float ColorScale = saturate(Lum - AvgLum * g_DownScaleBloomThreshold);

		// 블룸 스케일 조정 후 저장
		g_BloomTex[CurPixel.xy] = Color * ColorScale;
	}
}
