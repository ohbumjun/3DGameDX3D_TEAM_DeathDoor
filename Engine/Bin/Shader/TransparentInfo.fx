
#include "Standard3DInfo.fx"

cbuffer LightForwardCBuffer : register(b12)
{
	int g_LightCount;
	float3 g_LightForwardEmpty;
}

struct LightInfo
{
	float4 LightColor;
	int LightType;
	float3 LightPos;
	float3 LightDir;
	float LightDistance;
	float LightAngleIn;
	float LightAngleOut;
	float LightAtt1;
	float LightAtt2;
	float LightAtt3;
	float3 LightEmpty;
};

StructuredBuffer<LightInfo> g_LightInfoArray : register(t13);

LightResult ComputeLight(LightInfo Info, float3 Pos, float3 Normal, float2 UV)
{
	LightResult result = (LightResult) 0;
	
	float3 LightDir = (float3) 0.f;
	float Attn = 1.f;
	
	if (Info.LightType == LightTypeDir)
	{
		LightDir = -Info.LightDir;
		LightDir = normalize(LightDir);
	}
	
	
	if (Info.LightType == LightTypePoint)
	{
		LightDir = Info.LightPos - Pos;
		LightDir = normalize(LightDir);
		
		float Dist = distance(Info.LightPos, Pos);
		
		if (Dist > Info.LightDistance)
			Attn = 0.f;
		else
			Attn = 1.f / (Info.LightAtt1 + Info.LightAtt2 * Dist + Info.LightAtt3 * (Dist * Dist));
	}
	
	
	if (Info.LightType == LightTypeSpot)
	{
	}
	
	float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
	float Intensity = max(0.f, dot(ViewNormal, LightDir));
    
	float3 MtrlDif = g_MtrlBaseColor.rgb;
	float3 MtrlAmb = g_MtrlAmbientColor.rgb;

	float3 MtrlSpc = g_MtrlSpecularColor.rgb;
	if (g_MtrlSpecularTex)
	{
		MtrlSpc = g_SpecularTexture.Sample(g_BaseSmp, UV).rgb;
	}
	
	float3 MtrlEmv = g_MtrlEmissiveColor.rgb;
	if (g_MtrlEmissiveTex)
	{
		MtrlEmv = g_EmissiveTexture.Sample(g_BaseSmp, UV).rgb;
	}

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);

	result.Dif = Info.LightColor.xyz * MtrlDif * Intensity * Attn;
	result.Amb = Info.LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn;

    // Linear Space To Gamma Space 
    // 다시 Gamma Space로 되돌림
	result.Dif = pow(result.Dif, 1 / 2.2f);
	
	float3 View = -Pos;
	View = normalize(View);
	
	// 퐁 쉐이딩
	float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
	Reflect = normalize(Reflect);
	float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
	result.Spc = Info.LightColor.xyz * MtrlSpc *
		pow(SpcIntensity, g_MtrlSpecularColor.w) * Attn;
	
	result.Emv = MtrlEmv;
	
	return result;
}
