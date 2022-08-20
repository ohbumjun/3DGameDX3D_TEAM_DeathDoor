#include "ShaderInfo.fx"

struct Vertex3D
{
	// ���� �ڿ� : ���������̸� + ��ȣ �� �����Ѵ�.
	// ��ȣ�� �Ⱥ��̸� 0���� �����ȴ�.
    float3 Pos : POSITION; // Vector3Ÿ��.
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float4 BlendWeight : BLENDWEIGHT;
    float4 BlendIndex : BLENDINDEX;
};

struct Vertex3DOutput
{
	// SV�� ������ System Value�̴�. �� ���� �������Ϳ� ���常 �ϰ�
	// ������ ����ϸ� �ȵȴ�.
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float4 ProjPos : POSITION;
    float3 ViewPos : POSITION1;
	float3 WorldPos : POSITION2;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

// Instancing
struct Vertex3DInstancing
{
	// ���� �ڿ� : ���������̸� + ��ȣ �� �����Ѵ�.
	// ��ȣ�� �Ⱥ��̸� 0���� �����ȴ�.
	float3 Pos : POSITION; // Vector3Ÿ��.
	float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float4 BlendWeight : BLENDWEIGHT;
	float4 BlendIndex : BLENDINDEX;
	uint InstanceID : SV_InstanceID;
};

struct Vertex3DOutputInstancing
{
	// SV�� ������ System Value�̴�. �� ���� �������Ϳ� ���常 �ϰ�
	// ������ ����ϸ� �ȵȴ�.
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD;
	float4 ProjPos : POSITION;
	float3 ViewPos : POSITION1;
	float3 WorldPos : POSITION2;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	uint InstanceID : TEXCOORD1;
};

struct InstancingInfo
{
	matrix matWV;
	matrix matWVP;
	float4 g_MtrlBaseColor;
	float4 g_MtrlAmbientColor;
	float4 g_MtrlSpecularColor;
	float4 g_MtrlEmissiveColor;
	float g_MtrlOpacity;
	int g_MtrlPaperBurnEnable;
	int g_MtrlBumpEnable;
	int g_MtrlAnimation3DEnable;
	int g_MtrlSpecularTex;
	int g_MtrlEmissiveTex;
	int g_MtrlReceiveDecal;
	float g_PaperBurnFilter;
	int g_PaperBurnInverse;
	float g_PaperBurnInFilter;
	float g_PaperBurnOutFilter;
	float g_PaperBurnCenterFilter;
	float4 g_PaperBurnInLineColor;
	float4 g_PaperBurnOutLineColor;
	float4 g_PaperBurnCenterLineColor;
	int g_MtrlMetallic;
	float2 g_MtrlUVScale;
	float g_Instancing3DEmpty;
};

StructuredBuffer<InstancingInfo> g_InstancingInfoArray : register(t40);
StructuredBuffer<matrix> g_SkinningBoneMatrixArray : register(t106);

struct SkinningInfo
{
    float3 Pos;
    float3 Normal;
    float3 Tangent;
    float3 Binormal;
};

SkinningInfo Skinning(float3 Pos, float3 Normal, float3 Tangent, float3 Binormal,
    float4 BlendWeight, float4 BlendIndex)
{
    SkinningInfo Info = (SkinningInfo)0;
    
    if (g_MtrlAnimation3DEnable == 0)
    {
        Info.Pos = Pos;
        Info.Normal = Normal;
        Info.Tangent = Tangent;
        Info.Binormal = Binormal;
        
        return Info;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;
        
        matrix matBone = g_SkinningBoneMatrixArray[(int)BlendIndex[i]];
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
        Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Binormal += (mul(float4(Binormal, 0.f), matBone) * BlendWeight[i]).xyz;
    }
    
    Info.Normal = normalize(Info.Normal);
    Info.Tangent = normalize(Info.Tangent);
    Info.Binormal = normalize(Info.Binormal);
    
    
    return Info;

}

SkinningInfo SkinningInstancing(float3 Pos, float3 Normal, float3 Tangent, float3 Binormal,
    float4 BlendWeight, float4 BlendIndex, uint InstanceID)
{
	SkinningInfo Info = (SkinningInfo) 0;
    
	if (g_InstancingInfoArray[InstanceID].g_MtrlAnimation3DEnable == 0)
	{
		Info.Pos = Pos;
		Info.Normal = Normal;
		Info.Tangent = Tangent;
		Info.Binormal = Binormal;
        
		return Info;
	}
    
	for (int i = 0; i < 4; ++i)
	{
		if (BlendWeight[i] == 0.f)
			continue;
        
		matrix matBone = g_SkinningBoneMatrixArray[(InstanceID * g_InstancingBoneCount) + (int) BlendIndex[i]];
        
		Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
		Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
		Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
		Info.Binormal += (mul(float4(Binormal, 0.f), matBone) * BlendWeight[i]).xyz;
	}
    
	Info.Normal = normalize(Info.Normal);
	Info.Tangent = normalize(Info.Tangent);
	Info.Binormal = normalize(Info.Binormal);
    
    
	return Info;

}

float3 ComputeBumpNormalInstancing(float3 Normal, float3 Tangent, float3 Binormal,
	float2 UV, uint InstanceID)
{
	float3 result = Normal;
	
	if (g_InstancingInfoArray[InstanceID].g_MtrlBumpEnable == 1)
	{
		float4 NormalColor = g_NormalTexture.Sample(g_BaseSmp, UV);
		
		// ������ 0 ~ 1 �����̹Ƿ� -1 ~ 1 ������ �������ͷ� ������ش�.
		float3 ConvertNormal = NormalColor.xyz * 2.f - 1.f;
		// z�� ������ + �������� ������ش�.
		ConvertNormal.z = 1.f;
		ConvertNormal = normalize(ConvertNormal);
		
		// Tangent, Binormal, Normal�� �̿��ؼ� ������ ������� ����
		// ���͸� ��������� ��ȯ���ش�. ������� �Ǵ� ������ Tangent,
		// Binormal, Normal 3�� ��� ��������� ��ȯ�� ���Ͱ� ������
		// �����̴�.
		float3x3 mat =
		{
			Tangent,
			Binormal,
			Normal
		};

		result = normalize(mul(ConvertNormal, mat));
	}
	
	return result;
}

