
#include "ShaderInfo.fx"

struct VertexUV
{
	// ���� �ڿ� : ���������̸� + ��ȣ �� �����Ѵ�.
	// ��ȣ�� �Ⱥ��̸� 0���� �����ȴ�.
	float3 Pos : POSITION; // Vector3Ÿ��.
	float2 UV : TEXCOORD;
};

struct VertexUVOutput
{
	// SV�� ������ System Value�̴�. �� ���� �������Ϳ� ���常 �ϰ�
	// ������ ����ϸ� �ȵȴ�.
	float4 Pos : SV_POSITION;
	float4 ProjPos : POSITION;
	float2 UV : TEXCOORD;
	float2 OriginUV : TEXCOORD1;
};

cbuffer Animation2D : register(b10)
{
	float2 g_Animation2DStartUV;
	float2 g_Animation2DEndUV;
	int g_Animation2DType;
	float3 g_Animation2DEmpty;
};

Texture2DMS<float4> g_GBufferDepth : register(t10);

#define	TextureAtlas	0
#define	TextureFrame	1
#define	TextureArray	2

float2 ComputeAnimation2DUV(float2 UV)
{
	float2 result = (float2) 0.f;

	if (UV.x == 0.f)
		result.x = g_Animation2DStartUV.x;
	else
		result.x = g_Animation2DEndUV.x;

	if (UV.y == 0.f)
		result.y = g_Animation2DStartUV.y;
	else
		result.y = g_Animation2DEndUV.y;

	return result;
}

VertexUVOutput BillboardVS(VertexUV input)
{
	VertexUVOutput output = (VertexUVOutput) 0;

	float3 Pos = input.Pos - g_Pivot * g_MeshSize;

	output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
	output.Pos = output.ProjPos;

	if (g_Animation2DEnable == 0)
		output.UV = input.UV;
	else
		output.UV = ComputeAnimation2DUV(input.UV);

	output.OriginUV = input.UV;

	return output;
}

PSOutput_Single BillboardPS(VertexUVOutput input)
{
	PSOutput_Single output = (PSOutput_Single) 0;

	float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);

	if (BaseTextureColor.a == 0.f || g_MtrlOpacity == 0.f)
		clip(-1);

	float2 ScreenUV = input.ProjPos.xy / input.ProjPos.w;
	ScreenUV = ScreenUV * float2(0.5f, -0.5f) + float2(0.5, 0.5f);

	int2 TargetPos = (int2)0;
	TargetPos.x = (int) (ScreenUV.x * g_Resolution.x);
	TargetPos.y = (int) (ScreenUV.y * g_Resolution.y);

	float4 Depth = g_GBufferDepth.Load(TargetPos, 0);

	float Alpha = 1.f;

	// ���� ���� ���̰� �۴ٴ� ����, ���� �پ��ִٴ� ��
	// �̶�, Bill Board �� ���� �Ⱥ����� �Ѵ�.
	// Depth�� Alphaä���� 0�� ��� -> GBuffer�� ������ �ʾҴٴ� ��
	// z���� ����������� Alpha�� �پ���.
	if (Depth.a > 0.f)
	{
		Alpha = (Depth.a - input.ProjPos.w) / 0.5f;
	}

	Alpha = clamp(Alpha, 0, 1.f);
    
	output.Color = BaseTextureColor * g_MtrlBaseColor;

	output.Color.a = BaseTextureColor.a * g_MtrlOpacity * Alpha;

	return output;
}
