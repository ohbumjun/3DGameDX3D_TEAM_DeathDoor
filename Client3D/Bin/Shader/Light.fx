
#include "ShaderInfo.fx"

struct PS_OUTPUT_LIGHTACC
{
    float4 Dif : SV_TARGET;
    float4 Spc : SV_TARGET1;
    float4 Emv : SV_TARGET2;
};

struct VS_OUTPUT_LIGHTACC
{
    float4 Pos : SV_POSITION;
    float4 ProjPos : POSITION;
};

Texture2DArray g_RampTexArray : register(t4);

Texture2DMS<float4> g_GBufferDiffuseTex : register(t14);
Texture2DMS<float4> g_GBuffer1Tex : register(t15);
Texture2DMS<float4> g_GBuffer2Tex : register(t16);
Texture2DMS<float4> g_GBuffer3Tex : register(t17);
Texture2DMS<float4> g_GBuffer4Tex : register(t18);
Texture2DMS<float4> g_GBuffer5Tex : register(t19); //Mtrl Metallic 여부

Texture2DMS<float4> g_LightDifTex : register(t18);
Texture2DMS<float4> g_LightSpcTex : register(t19);
Texture2DMS<float4> g_LightEmvTex : register(t20);

Texture2DMS<float4> g_LightBlendTex : register(t21);
Texture2DMS<float4> g_Depth			: register(t22);
Texture2DMS<float4> g_PlayerStencil : register(t23);

Texture2DMS<float4> g_ShadowMapTex : register(t22);

TextureCube g_Sky : register(t23);

// Texture2DMS<float4> g_OutlineTex : register(t23);

cbuffer ShadowCBuffer : register(b10)
{
	matrix g_matShadowVP;
	matrix g_matShadowInvVP;
	float g_ShadowBias;
	float2 g_ShadowResolution;
	float g_ShadowEmpty;
};

Texture2DMS<float4> g_AnimEditorRenderTex : register(t55);

void ComputeLightDirAndAttn(out float3 outLightDir, out float outAttn, float3 Pos)
{
    if (g_LightType == LightTypeDir)
    {
		outLightDir = -g_LightDir;
		outLightDir = normalize(outLightDir);
		outAttn = 1.f;
	}
	
    else if (g_LightType == LightTypePoint)
    {
		outLightDir = g_LightPos - Pos;
		outLightDir = normalize(outLightDir);
		
        float Dist = distance(g_LightPos, Pos);
		
        if (Dist > g_LightDistance)
			outAttn = 0.f;
        else
            outAttn = 1.f / (g_LightAtt1 + g_LightAtt2 * Dist + g_LightAtt3 * (Dist * Dist));
    }
}

float3 ComputeMetalDiffuse(float3 LightColor, float3 Env, float3 MtrlDiffuse, float Attn)
{
	return LightColor * MtrlDiffuse * Env * Attn;
}

float3 ComputeMetalSpc(float3 LightColor, float3 MtrlDiffuse, float PowSpcMulAttn)
{
	return LightColor * MtrlDiffuse * PowSpcMulAttn;
}

LightResult ComputeLightToon(float3 Pos, float3 Normal, float4 MaterialColor,float SpcPower, float Metallic, float ToonType)
{
    LightResult result = (LightResult) 0;
	
    float3 LightDir = (float3) 0.f;
    float Attn = 1.f;

	ComputeLightDirAndAttn(LightDir, Attn, Pos);
	
    float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));

    // Half Rambert
	Intensity = (Intensity * 0.5f) + 0.5f;

    // Wraped Diffuse
	int TexIndex = (int)ceil(ToonType);
	float3 Wrapped = g_RampTexArray.Sample(g_BaseSmp, float3(Intensity, 0.f, ToonType)).rgb;
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;
	
    float3 View = -Pos;
    View = normalize(View);

	// 퐁 쉐이딩
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);
	
    // n dot l 대신 Ramp Texture의 값을 샘플링한 값을 색상으로 취함
    // Diffuse : Linear Space에서 연산
	float3 ToonDif = g_LightColor.xyz * MtrlDif * Wrapped * Attn;
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

    // Metallic 색상과 원래 색상 사이 보간
	result.Dif = lerp(ToonDif, MetalDif, Metallic);

    // Linear Space To Gamma Space 
    // 다시 Gamma Space로 되돌림
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic 물체는 앰비언트 주지 않음
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    // Metallic Specular는 재질의 색을 따라간다.
    float3 ToonSpc = g_LightColor.xyz * MtrlSpc * PowSpcAttn; 
	float3 MetalSpc = ComputeMetalSpc(g_LightColor.xyz, MtrlDif, PowSpcAttn);

	result.Spc = lerp(ToonSpc, MetalSpc, Metallic);
	
    result.Emv = MtrlEmv;
	
    return result;
}

LightResult ComputeLight(float3 Pos, float3 Normal, float4 MaterialColor, float SpcPower, float Metallic)
{
    LightResult result = (LightResult) 0;
	
    float3 LightDir = (float3) 0.f;
    float Attn = 1.f;

	ComputeLightDirAndAttn(LightDir, Attn, Pos);
	
    float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;

    float3 View = -Pos;
    View = normalize(View);
	
	// 퐁 쉐이딩
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);
	
    // Metallic 색상과 보간
	float3 StandardDif = g_LightColor.xyz * MtrlDif * Intensity * Attn; 
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

	result.Dif = lerp(StandardDif, MetalDif, Metallic);

    // Linear Space To Gamma Space
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic 물체는 앰비언트 주지 않음
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
    // Specular 보간
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    float3 StandardSpc = g_LightColor.xyz * MtrlSpc * PowSpcAttn;
	float3 MetalSpc = ComputeMetalSpc(g_LightColor.xyz, MtrlDif, PowSpcAttn);

	result.Spc = lerp(StandardSpc, MetalSpc, Metallic);
	
    result.Emv = MtrlEmv;
	
    return result;
}

VS_OUTPUT_LIGHTACC LightAccVS(uint VertexID : SV_VertexID)
{
    VS_OUTPUT_LIGHTACC output = (VS_OUTPUT_LIGHTACC) 0;
    
    output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.Pos = output.ProjPos;
    
    return output;
}

PS_OUTPUT_LIGHTACC LightAccPS(VS_OUTPUT_LIGHTACC input)
{
	PS_OUTPUT_LIGHTACC output = (PS_OUTPUT_LIGHTACC) 0;
    
	float2 UV;
	UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
	UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
	int2 TargetPos = (int2) 0;
	
	TargetPos.x = (int) (UV.x * g_Resolution.x);
	TargetPos.y = (int) (UV.y * g_Resolution.y);
	
	float4 GBufferDiffuseColor = g_GBufferDiffuseTex.Load(TargetPos, 0);
    
	if (GBufferDiffuseColor.a == 0.f)
		clip(-1);
    
	float4 GBuffer1Color = g_GBuffer1Tex.Load(TargetPos, 0);
	float4 GBuffer2Color = g_GBuffer2Tex.Load(TargetPos, 0);
	float4 GBuffer3Color = g_GBuffer3Tex.Load(TargetPos, 0);
	float4 GBuffer4Color = g_GBuffer4Tex.Load(TargetPos, 0);
	float4 GBuffer5Color = g_GBuffer5Tex.Load(TargetPos, 0); // Mtrl Metallic 정보
    
    // uv를 이용해서 위치를 구해준다.
    // 먼저 투영 공간의 위치를 구해준다.
	float4 ProjPos;
	ProjPos.x = UV.x * 2.f - 1.f;
	ProjPos.y = UV.y * -2.f + 1.f;
	ProjPos.z = GBuffer2Color.r;
	ProjPos.w = 1.f;
    
	ProjPos *= GBuffer2Color.g;
    
    // 뷰공간의 위치를 구한다.
	float3 ViewPos = mul(ProjPos, g_matInvProj).xyz;
    
	float3 ViewNormal = GBuffer1Color.xyz;    

	float PixelType = GBuffer4Color.a;

	LightResult Result;

    if (PixelType == Pixel_Type_Default)
	{
		Result = ComputeLight(ViewPos, ViewNormal, GBuffer3Color, GBuffer2Color.b, GBuffer5Color.a);
	}
    else if (PixelType == Pixel_Type_Toon)
	{
		Result = ComputeLightToon(ViewPos, ViewNormal, GBuffer3Color, GBuffer2Color.b, GBuffer5Color.a, GBuffer4Color.r);
	}
    
	output.Dif.rgb = Result.Dif + Result.Amb;
	output.Spc.rgb = Result.Spc;
	output.Emv.rgb = Result.Emv;

	output.Dif.a = 1.f;
    // Speclular의 Alpha Channel에 Metallic 여부 담아서 넘긴다.
	output.Spc.a = GBuffer5Color.a;
	output.Emv.a = 1.f;
    
	return output;
}

PSOutput_Single LightBlendPS(VS_OUTPUT_LIGHTACC input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
    int2 TargetPos = (int2) 0;
	
    TargetPos.x = (int) (UV.x * g_Resolution.x);
    TargetPos.y = (int) (UV.y * g_Resolution.y);

	float4 GBufferDiffuseColor = g_GBufferDiffuseTex.Load(TargetPos, 0);

    if (GBufferDiffuseColor.a == 0.f)
	{
		clip(-1);
	}
	
    float4 LightDiffuseColor = g_LightDifTex.Load(TargetPos, 0);
    float4 LightSpecularColor = g_LightSpcTex.Load(TargetPos, 0);
    float4 LightEmissiveColor = g_LightEmvTex.Load(TargetPos, 0);

	float4 GBuffer2Color = g_GBuffer2Tex.Load(TargetPos, 0);

	float4 ProjPos;
	ProjPos.x = input.ProjPos.x;
	ProjPos.y = input.ProjPos.y;
	ProjPos.z = GBuffer2Color.r;
	ProjPos.w = 1.f;

	ProjPos *= GBuffer2Color.g;

    // 월드공간 변환
	float3 WorldPos = mul(ProjPos, g_matInvVP).xyz;

    // 조명공간 변환
	float4 ShadowPos = mul(float4(WorldPos, 1.f), g_matShadowVP);

	float2 ShadowUV;
	ShadowUV.x = ShadowPos.x / ShadowPos.w * 0.5f + 0.5f;
	ShadowUV.y = ShadowPos.y / ShadowPos.w * -0.5f + 0.5f;

	int2 ShadowTargetPos = (int2)0;

	ShadowTargetPos.x = (int) (ShadowUV.x * g_ShadowResolution.x);
	ShadowTargetPos.y = (int) (ShadowUV.y * g_ShadowResolution.y);

	float4 ShadowMap = g_ShadowMapTex.Load(ShadowTargetPos, 0);

    if (ShadowMap.a > 0.f)
	{
        if (ShadowPos.z - g_ShadowBias > ShadowMap.r * ShadowPos.w)
		{
			LightDiffuseColor.rgb *= 0.6f;
			LightSpecularColor.rgb *= 0.f;
		}
	}

	output.Color.rgb = GBufferDiffuseColor.rgb * LightDiffuseColor.rgb + LightSpecularColor.rgb + LightEmissiveColor.rgb;

	output.Color.a = GBufferDiffuseColor.a;
    
    return output;
}

PSOutput_Single LightBlendRenderPS(VS_OUTPUT_LIGHTACC input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
    int2 TargetPos = (int2) 0;
	
    TargetPos.x = (int) (UV.x * g_Resolution.x);
    TargetPos.y = (int) (UV.y * g_Resolution.y);
	
    float4 LightBlendColor = g_LightBlendTex.Load(TargetPos, 0);
    
    if (LightBlendColor.a == 0.f)
        clip(-1);

	output.Color = LightBlendColor;

	float Depth = g_Depth.Load(TargetPos, 0).g;
	float PlayerStencilDepth = g_PlayerStencil.Load(TargetPos, 0).w;

	if (PlayerStencilDepth > Depth + 3.f)
	{
		output.Color = float4(0.7f, 0.7f, 0.7f, 1.f);
	}

	if (g_FadeStart == 1)
	{
		float3 FadeColor = lerp(g_FadeStartColor, g_FadeEndColor, g_FadeRatio);
		output.Color.rgb *= FadeColor;
	}

    return output;
}

PSOutput_Single AnimEditorRenderPS(VS_OUTPUT_LIGHTACC input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;

    int2 TargetPos = (int2) 0;

    TargetPos.x = (int)(UV.x * g_Resolution.x);
    TargetPos.y = (int)(UV.y * g_Resolution.y);

    float4 AnimRenderColor = g_AnimEditorRenderTex.Load(TargetPos, 0);

    if (AnimRenderColor.a == 0.f)
        clip(-1);

    output.Color = AnimRenderColor;

    return output;
}