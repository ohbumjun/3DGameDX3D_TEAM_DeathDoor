#include "ShaderInfo.fx"

cbuffer HDRRenderCBuffer : register(b10)
{
	float g_HDRMiddleGray;
	float g_HDRLumWhite;
	float g_HDRBloomScale;
	float g_HDRDOFMin;
	float g_HDRDOFMax;
	float3 g_HDRDOFEmpty;
};

cbuffer FogCBuffer : register(b11)
{
	float3	g_FogColor;
	int		g_FogType;
	float	g_FogStart;
	float	g_FogEnd;
	float	g_FogDensity;
	float	g_FogEmpty;
}

Texture2DMS<float4> g_HDRTex : register(t10);
StructuredBuffer<float> g_AvgLum : register(t11);
Texture2D<float4> g_BloomTex : register(t12);
Texture2DMS<float4> g_GBufferDepth : register(t13);
Texture2D<float4> g_DownScaleTex : register(t14);
Texture2DMS<float4> g_PlayerStencil : register(t15);

#define NatrualE 2.71828

#define FOG_TYPE_DEPTH	0
#define FOG_TYPE_Y		1

float4 DepthFog(float3 OriginColor, float Depth)
{
	// 안개 얼만큼 끼는지 계산
	float4 Color;
	float FogFactor;

	// 선형 안개
	FogFactor = ((g_FogEnd - Depth) / (g_FogEnd - g_FogStart)) * g_FogDensity;
	FogFactor = saturate(FogFactor);
	
	// 안개 색상과 보간
	Color.rgb = lerp(OriginColor.rgb, g_FogColor, 1.f - FogFactor);

	return Color;
}

float4 YFog(float3 OriginColor, float WorldY)
{
	// 안개 얼만큼 끼는지 계산
	float4 Color;
	float FogFactor;

	// 선형 안개
	FogFactor = ((g_FogEnd - WorldY) / (g_FogEnd - g_FogStart)) * g_FogDensity;
	FogFactor = saturate(FogFactor);
	
	// 안개 색상과 보간
	Color.rgb = lerp(OriginColor.rgb, g_FogColor, 1.f - FogFactor);

	return Color;
}

float3 DepthDOF(float3 ColorFocus, float3 ColorBlurred, float Depth)
{
	// 1. Depth <= DOFMin : Always ColorFocus
	// 2. Depth > DOFMin && Depth < DOFMax : 0 ~ 1
	// 3. Depth > DOFMax : Always Blurred
	float BlurFactor = saturate((Depth - g_HDRDOFMin) / (g_HDRDOFMax - g_HDRDOFMin));
	return lerp(ColorFocus, ColorBlurred, BlurFactor);
}

float3 ToneMapping(float3 HDRColor)
{
 //	// 현재 픽셀에 대한 휘도 스케일 계산
	float LScale = dot(float4(HDRColor, 0.f), LUM_FACTOR);
	LScale *= g_HDRMiddleGray / g_AvgLum[0];
	LScale = (LScale + ((LScale * LScale) / (g_HDRLumWhite * g_HDRLumWhite))) / (1.0 + LScale);
 //	float LScale = dot(float4(HDRColor, 0.f), LUM_FACTOR) / (9.6f * g_AvgLum[0]);
 //	LScale = (LScale * (1.f + (LScale / g_HDRLumWhiteSqr))) / (1.f + LScale);
	
	return HDRColor * LScale;
}

struct VS_OUTPUT_HDR
{
    float4 Pos : SV_POSITION;
    float4 ProjPos : POSITION;
};

VS_OUTPUT_HDR HDRRenderVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT_HDR Output = (VS_OUTPUT_HDR) 0;

	Output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
	Output.Pos = Output.ProjPos;
    
	return Output;
}

PSOutput_Single HDRRenderPS(VS_OUTPUT_HDR Input)
{
	PSOutput_Single Output = (PSOutput_Single)0;

	float2 UV = (float2) 0;
	UV.x = Input.ProjPos.x / Input.ProjPos.w * 0.5f + 0.5f;
	UV.y = Input.ProjPos.y / Input.ProjPos.w * -0.5f + 0.5f;

	int2 TargetPos = (int2) 0;
	TargetPos.x = (int) (UV.x * g_Resolution.x);
	TargetPos.y = (int) (UV.y * g_Resolution.y);

	// 색상 샘플 계산
	float4 Color = g_HDRTex.Load(TargetPos, 0);

	// Bloom 분포 색상
	float4 BloomColor = g_BloomTex.Sample(g_LinearSmp, UV);
	BloomColor *= g_HDRBloomScale * BloomColor;

	float Alpha = Color.a + ((BloomColor.r + BloomColor.g + BloomColor.b) / 3.f);

	float4 Depth = g_GBufferDepth.Load(TargetPos, 0);

	// 가장 먼 평면 제외
	if (Depth.r < 1.f)
	{
		// DownScale된 Texture를 업스케일 하면서 블러 효과
		float3 ColorBlurred = g_DownScaleTex.Sample(g_LinearSmp, UV).rgb;

		// 깊이에 따라 원래 픽셀 색상과 블러 색상을 보간함
		Color.rgb = DepthDOF(Color.rgb, ColorBlurred, Depth.g);
	}

	// 원래 픽셀에 Bloom 추가
	Color += BloomColor;

	// 톤 매핑
	Color = float4(ToneMapping(Color.rgb), 1.f);

	if (Depth.r < 1.f)
	{
		// Fog 계산
		// Background 색이 Fog 색이랑 동일해야 자연스러운 결과 얻을 수 있음

		if (g_FogType == FOG_TYPE_DEPTH)
		{
			Color = DepthFog(Color.rgb, Depth.g);
		}
		else
		{
			Color = YFog(Color.rgb, Depth.a);
		}
	}

	Output.Color = Color;

	// 플레이어 가려진 경우 스텐실 처리
	float PlayerStencilDepth = g_PlayerStencil.Load(TargetPos, 0).w;

	if (PlayerStencilDepth > Depth.g + 3.f)
	{
		Output.Color = float4(0.7f, 0.7f, 0.7f, 1.f);
	}

	if (g_FadeStart == 1)
	{
		float3 FadeColor = lerp(g_FadeStartColor, g_FadeEndColor, g_FadeRatio);
		Output.Color.rgb *= FadeColor;
	}

	return Output;
}

PSOutput_Single RenderBombEffect(VS_OUTPUT_HDR Input)
{
	PSOutput_Single Output = (PSOutput_Single)0;

	float2 UV = (float2) 0;
	UV.x = Input.ProjPos.x / Input.ProjPos.w * 0.5f + 0.5f;
	UV.y = Input.ProjPos.y / Input.ProjPos.w * -0.5f + 0.5f;

	int2 TargetPos = (int2) 0;
	TargetPos.x = (int) (UV.x * g_Resolution.x);
	TargetPos.y = (int) (UV.y * g_Resolution.y);

	// 색상 샘플 계산
	float4 Color = g_HDRTex.Load(TargetPos, 0);

	// Bloom 분포 색상
	float4 BloomColor = g_BloomTex.Sample(g_LinearSmp, UV);
	BloomColor *= g_HDRBloomScale * BloomColor;

	float4 Depth = g_GBufferDepth.Load(TargetPos, 0);

	// 원래 픽셀에 Bloom 추가
	Color += BloomColor;

	// 휘도 계산해서 일정 휘도 이상인 경우에만 하얗게, 나머지는 까만 색상으로 렌더
	float Lum = dot(Color.rgb, LUM_FACTOR.rgb);

	if (Lum >= 0.15f)
	{
		Color = float4(1.f, 1.f, 1.f, 1.f);
	}
	else
	{
		Color = float4(0.f, 0.f, 0.f, 1.f);
	}

	Output.Color = Color;

	return Output;
}
