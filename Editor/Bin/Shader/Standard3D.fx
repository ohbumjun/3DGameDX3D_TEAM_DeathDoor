
#include "Standard3DInfo.fx"

Vertex3DOutput Standard3DVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;

    SkinningInfo Info = Skinning(input.Pos, input.Normal, input.Tangent,
                input.Binormal, input.BlendWeight, input.BlendIndex);
    
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_matWV).xyz;

    // 월드공간의 위치
	output.WorldPos = Pos;
    
    // 뷰 공간의 Normal을 만들어준다.
    output.Normal = normalize(mul(float4(Info.Normal, 0.f), g_matWV).xyz);

    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(Info.Tangent, 0.f), g_matWV).xyz);

    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(Info.Binormal, 0.f), g_matWV).xyz);

    output.UV = input.UV;

    return output;
}

PSOutput_GBuffer Standard3DPS(Vertex3DOutput input)
{
    PSOutput_GBuffer output = (PSOutput_GBuffer) 0;

	float2 ScaledUV = input.UV * g_MtrlUVScale;
    
    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, ScaledUV);

    if (BaseTextureColor.a == 0.f || g_MtrlOpacity == 0.f)
        clip(-1);
    
    output.Diffuse.rgb = BaseTextureColor.rgb;

    output.Diffuse.a = BaseTextureColor.a * g_MtrlOpacity;
    
    output.GBuffer1.rgb = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, ScaledUV);
    output.GBuffer1.a = 1.f;
    
    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
	output.GBuffer2.b = g_MtrlSpecularColor.w;
    output.GBuffer2.a = input.WorldPos.y;    // y fog를 위해 월드 y값 전송

    output.GBuffer3.r = ConvertColor(g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_MtrlAmbientColor);

    output.GBuffer4.rgb = input.Tangent.xyz;
    output.GBuffer4.a = 1.f;
    
    output.GBuffer5.rgb = input.Binormal.xyz;

    // Mtrl Metallic 여부
	output.GBuffer5.a = g_MtrlMetallic;
    
    float4 SpecularColor = g_MtrlSpecularColor.xyzw;
	
    if (g_MtrlSpecularTex)
		SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, ScaledUV).xxxx;
    
    output.GBuffer3.b = ConvertColor(SpecularColor);
    
    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;
	
    if (g_MtrlEmissiveTex)
		EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, ScaledUV).xxxx;

    output.GBuffer3.a = ConvertColor(EmissiveColor);

    if (g_MtrlPaperBurnEnable)
	{
		output.Diffuse = PaperBurn2D(output.Diffuse, ScaledUV);
	}

    return output;
}

// Instancing
Vertex3DOutputInstancing Standard3DInstancingVS(Vertex3DInstancing input)
{
	Vertex3DOutputInstancing output = (Vertex3DOutputInstancing) 0;
    
	SkinningInfo Info = SkinningInstancing(input.Pos, input.Normal, input.Tangent,
                input.Binormal, input.BlendWeight, input.BlendIndex, input.InstanceID);
    
	output.InstanceID = input.InstanceID;
    
	float3 Pos = Info.Pos;

	output.ProjPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWVP);
	output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
	output.ViewPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz;

    // 월드공간의 위치
	output.WorldPos = input.Pos;
    
    // 뷰 공간의 Normal을 만들어준다.
	output.Normal = normalize(mul(float4(Info.Normal, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
	output.Tangent = normalize(mul(float4(Info.Tangent, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
	output.Binormal = normalize(mul(float4(Info.Binormal, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
	output.UV = input.UV;

	return output;
}

PSOutput_GBuffer Standard3DInstancingPS(Vertex3DOutputInstancing input)
{
	PSOutput_GBuffer output = (PSOutput_GBuffer) 0;

	int InstancingIndex = input.InstanceID + (g_InstancingObjecCount * g_InstancingMaterialIndex);
	float2 ScaledUV = input.UV * g_InstancingInfoArray[InstancingIndex].g_MtrlUVScale;

	float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, ScaledUV);

	if (BaseTextureColor.a == 0.f || g_InstancingInfoArray[InstancingIndex].g_MtrlOpacity == 0.f)
		clip(-1);
    
	output.Diffuse.rgb = BaseTextureColor.rgb;

	output.Diffuse.a = BaseTextureColor.a * g_InstancingInfoArray[InstancingIndex].g_MtrlOpacity;
    
	output.GBuffer1.rgb = ComputeBumpNormalInstancing(input.Normal, input.Tangent, input.Binormal, ScaledUV, input.InstanceID);
	output.GBuffer1.a = 1.f;
    
	output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
	output.GBuffer2.g = input.ProjPos.w;
	output.GBuffer2.b = g_InstancingInfoArray[InstancingIndex].g_MtrlSpecularColor.w;
	output.GBuffer2.a = input.WorldPos.y; // for y-fog
    
	output.GBuffer3.r = ConvertColor(g_InstancingInfoArray[InstancingIndex].g_MtrlBaseColor);
	output.GBuffer3.g = ConvertColor(g_InstancingInfoArray[InstancingIndex].g_MtrlAmbientColor);
    
	output.GBuffer4.rgb = input.Tangent.xyz;
	output.GBuffer4.a = 1.f;
    // g_InstancingInfoArray[input.InstanceID].g_MtrlReceiveDecal;
    
	output.GBuffer5.rgb = input.Binormal.xyz;

    // Mtrl Meatllic 여부
	output.GBuffer5.a = g_InstancingInfoArray[InstancingIndex].g_MtrlMetallic;
    
	float4 SpecularColor = g_InstancingInfoArray[InstancingIndex].g_MtrlSpecularColor.xyzw;
	
	if (g_InstancingInfoArray[InstancingIndex].g_MtrlSpecularTex)
		SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, ScaledUV).xxxx;
    
	output.GBuffer3.b = ConvertColor(SpecularColor);
    
	float4 EmissiveColor = g_InstancingInfoArray[InstancingIndex].g_MtrlEmissiveColor.xyzw;
	
	if (g_InstancingInfoArray[InstancingIndex].g_MtrlEmissiveTex)
		EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, ScaledUV).xxxx;
    
	output.GBuffer3.a = ConvertColor(EmissiveColor);

	if (g_InstancingInfoArray[InstancingIndex].g_MtrlPaperBurnEnable)
	{
		output.Diffuse = PaperBurn2DInstancing(output.Diffuse, ScaledUV, g_InstancingInfoArray[InstancingIndex].g_PaperBurnInverse,
        g_InstancingInfoArray[InstancingIndex].g_PaperBurnFilter, g_InstancingInfoArray[InstancingIndex].g_PaperBurnInFilter,
        g_InstancingInfoArray[InstancingIndex].g_PaperBurnCenterFilter, g_InstancingInfoArray[InstancingIndex].g_PaperBurnOutFilter,
        g_InstancingInfoArray[InstancingIndex].g_PaperBurnInLineColor, g_InstancingInfoArray[InstancingIndex].g_PaperBurnCenterLineColor,
        g_InstancingInfoArray[InstancingIndex].g_PaperBurnOutLineColor);
	}

	return output;
}

PSOutput_Single StandardNoLight3DPS(Vertex3DOutput input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    //LightResult LightInfo = ComputeLight(input.ViewPos, input.Normal,
    //    input.Tangent, input.Binormal, input.UV);

    // float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);
    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);

    if (BaseTextureColor.a == 0.f || g_MtrlOpacity == 0.f)
        clip(-1);

    output.Color.rgb = BaseTextureColor.rgb;// * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;

    output.Color.a = BaseTextureColor.a * g_MtrlOpacity;

    return output;
}

Vertex3DOutput Standard3DWireFrameVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;

    float3 Pos = input.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_matWV).xyz;
    
    // 뷰 공간의 Normal을 만들어준다.
	output.Normal = normalize(mul(float4(input.Normal, 0.f), g_matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(input.Tangent, 0.f), g_matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(input.Binormal, 0.f), g_matWV).xyz);
    output.UV = input.UV;

    return output;
}

PSOutput_GBuffer Standard3DWireFramePS(Vertex3DOutput input)
{
    PSOutput_GBuffer output = (PSOutput_GBuffer) 0;
    
	float4 BaseTextureColor = float4(0.f, 1.f, 0.f, 1.f);
    
    output.Diffuse = BaseTextureColor;
    
    output.GBuffer1.rgb = input.Normal;
    output.GBuffer1.a = 1.f;
    
    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
    output.GBuffer2.b = 1.f;
    output.GBuffer2.a = 1.f;
    
	output.GBuffer3.r = float4(0.f, 1.f, 0.f, 1.f);
	output.GBuffer3.r = float4(0.f, 1.f, 0.f, 1.f);
    
    output.GBuffer4.rgb = input.Tangent.xyz;
    output.GBuffer4.a = 1.f;
    
    output.GBuffer5.rgb = input.Binormal.xyz;
    output.GBuffer5.a = 1.f;
    
	float4 SpecularColor = float4(0.f, 1.f, 0.f, 1.f);
	
    output.GBuffer3.b = ConvertColor(SpecularColor);
    
	float4 EmissiveColor = float4(0.f, 1.f, 0.f, 1.f);
    
    output.GBuffer3.a = ConvertColor(EmissiveColor);

    return output;
}
