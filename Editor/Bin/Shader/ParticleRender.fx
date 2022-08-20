#include "ShaderInfo.fx"
#include "ParticleData.fx"


struct VertexParticle
{
	float3	Pos	: POSITION;
	uint InstanceID : SV_InstanceID;	// �������ۿ� ���� �־��ִ°��� �ƴϴ�.
};

struct VertexParticleOutput
{
	uint	InstanceID : TEXCOORD;
};

StructuredBuffer<ParticleInfo>		g_ParticleArraySRV	: register(t30);
StructuredBuffer<ParticleInfoShared>	g_ParticleShareSRV	: register(t31);
Texture2DMS<float4> g_GBufferDepth : register(t10);

VertexParticleOutput ParticleVS(VertexParticle input)
{
	VertexParticleOutput	output = (VertexParticleOutput)0;

	output.InstanceID = input.InstanceID;

	return output;
}

struct GeometryParticleOutput
{
	float4	Pos : SV_POSITION;
	float4	Color : COLOR;
	float2	UV	: TEXCOORD;
	float4	ProjPos : POSITION;
	float      LifeTimeMax : NUMBER;
	int			InstanceID : NUMBER1;
	float      LifeTimeRatio : RATIO1;
	float      LocalXPlusMoveDir : RATIO2; // �⺻ 1 �� ���� (���� Local Space ����, X ���� �������� �̵��ϸ�, 0���� ����)
};

static float3	g_ParticleLocalPos[4] =
{
	{-0.5f, 0.5f, 0.f},
	{0.5f, 0.5f, 0.f},
	{-0.5f, -0.5f, 0.f},
	{0.5f, -0.5f, 0.f}
};

// in : ���� �Լ� ������ �Ѱ��ٶ�
// out : �Լ��ȿ��� ����� �Ѱܹ�����. �� �Լ� �ȿ��� �� ���� ����� ������ �� �� ����.
// inout : �׳� ���۷���.
[maxvertexcount(6)]
void ParticleGS(point VertexParticleOutput input[1],
	inout TriangleStream<GeometryParticleOutput> output)
{
	// ���� �簢������ Ȯ���Ѵ�. (�� ��������Ʈ)
	uint	InstanceID = input[0].InstanceID;

	if (g_ParticleArraySRV[InstanceID].Alive == 0)
		return;

	float	Ratio = g_ParticleArraySRV[InstanceID].LifeTime / g_ParticleArraySRV[InstanceID].LifeTimeMax;

	GeometryParticleOutput	OutputArray[4] =
	{
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f
	};

	OutputArray[0].UV = float2(0.f, 0.f);
	OutputArray[1].UV = float2(1.f, 0.f);
	OutputArray[2].UV = float2(0.f, 1.f);
	OutputArray[3].UV = float2(1.f, 1.f);

	// UV Animation ����
	if (g_ParticleShareSRV[0].UVMoveEnable == 1)
	{
		int UVRowN = g_ParticleShareSRV[0].UVRowN;
		int UVColN = g_ParticleShareSRV[0].UVColN;

		// ���� UV ���� ũ��
		float WUVSize = 1.f / (float)UVColN;
		// ���� UV ���� ũ�� 
		float HUVSize = 1.f / (float)UVRowN;

		// ex) 5 * 2 => 10��
		// 9 / 10 => 0.9 
		// 10 * 0.9 => 9��° UV => Idx �����δ� 8��° (�� Idx �� 0 ���� 9���� ����) 
		int TotalUVNum = UVRowN * UVColN;
		float FloatCurUVIdx = ((float)g_ParticleArraySRV[InstanceID].LifeTime / (float)(g_ParticleArraySRV[InstanceID].LifeTimeMax)) * (float)TotalUVNum;
		int CurUVIdx = floor(FloatCurUVIdx);
		// CurUVIdx -= 1;

		// row * col = 2 * 5
		// rowS = 0.5 (height)
		// colS  = 0.2 (width)
		// 0.95 * 10 => 9.5 => floor(9.5) => 9;
		// 10���� ĭ �߿���, ���� ������ ��° ĭ�̴�.
		// 9 / 5 => 1��
		// 9 % 5 => 4��
		int RowUVIdx = CurUVIdx / UVColN;
		int ColUVIdx = (CurUVIdx % UVColN);

		float2 UVStartPos = float2(WUVSize * ColUVIdx, HUVSize * RowUVIdx);
		float2 UVEndPos = UVStartPos + float2(WUVSize, HUVSize);

		OutputArray[0].UV = float2(UVStartPos.x, UVStartPos.y);
		OutputArray[1].UV = float2(UVEndPos.x, UVStartPos.y);
		OutputArray[2].UV = float2(UVStartPos.x, UVEndPos.y);
		OutputArray[3].UV = float2(UVEndPos.x, UVEndPos.y);
	}

	float3	Scale = lerp(
		g_ParticleShareSRV[0].ScaleMin * g_ParticleShareSRV[0].CommonWorldScale,
		g_ParticleShareSRV[0].ScaleMax * g_ParticleShareSRV[0].CommonWorldScale,
		float3(Ratio, Ratio, Ratio));

	float4 ColorStart = g_ParticleShareSRV[0].ColorStart;
	ColorStart.a = g_ParticleArraySRV[InstanceID].AlphaDistinctStart;

	float4	Color = lerp(ColorStart, g_ParticleShareSRV[0].ColorEnd,
		float4(Ratio, Ratio, Ratio, Ratio));

	// ȸ�� (��ü ȸ�� + �� Particle ȸ�� ����)
	float3 FinalRotAngle = g_ParticleShareSRV[0].RotationAngle + g_ParticleArraySRV[InstanceID].FinalSeperateRotAngle;

	float3x3 matRot = ComputeRotationMatrix(FinalRotAngle);

	// 4���� ���� ���������� ������ش�.
	for (int i = 0; i < 4; ++i)
	{
		// g_ParticleArraySRV[InstanceID].WorldPos �� Local Space �󿡼��� 
		float3	WorldPos = g_ParticleArraySRV[InstanceID].LocalPos + mul(g_ParticleLocalPos[i] * Scale, matRot);

		// Particle Component �� World Pos �� ���Ѵ�.
		if (g_ParticleFollowComponentPos == 1)
		{
			WorldPos += g_ParticleShareSRV[0].ParticleComponentWorldPos;
			// WorldPos += g_TVParticleComponentWorldPos;
		}
		else
		{
			WorldPos += g_ParticleArraySRV[InstanceID].InitParticleComponentWorldPos;
			// WorldPos += g_ParticleShareSRV[0].ParticleComponentWorldPos;
		}

		OutputArray[i].ProjPos = mul(float4(WorldPos, 1.f), g_matVP);
		// OutputArray[i].ProjPos.xyz = mul(OutputArray[i].ProjPos.xyz, matRot);

		OutputArray[i].Pos = OutputArray[i].ProjPos;

		OutputArray[i].Color = Color;

		OutputArray[i].InstanceID = InstanceID;

		OutputArray[i].LifeTimeMax = g_ParticleArraySRV[InstanceID].LifeTimeMax;

		OutputArray[i].LifeTimeRatio = Ratio;

		OutputArray[i].LocalXPlusMoveDir = 1;

		if (g_ParticleArraySRV[InstanceID].Dir.x < 0.f)
			OutputArray[i].LocalXPlusMoveDir = 0;
	}

	output.Append(OutputArray[0]);
	output.Append(OutputArray[1]);
	output.Append(OutputArray[3]);
	output.RestartStrip();

	output.Append(OutputArray[0]);
	output.Append(OutputArray[3]);
	output.Append(OutputArray[2]);
	output.RestartStrip();
}

// LifeTime ������ ����, (Texture ����) �����ʿ���, ���� �������� ���� ������� �ϱ�
void ApplyLinearUVClipping(GeometryParticleOutput input)
{
	// UV Cllipping ȿ��
	// ������ ����, ���� �������� ������ ������� ȿ���� �� ���̴�.
	// �̸� ���ؼ��� LifeTime �� ����ؾ� �Ѵ�.
	// �ٽ���, ���ϴ� ������ ���� �κи� ��� �����ְ� �ϴ� ���̴�.
	// UV ��ǥ 0 ~ 1. �󿡼�, ������ ���� ���� �����Ѵٰ� ����
	// �ش� ���� ���� -> ������, ������ -> ����. ���� �̵� �ϴ� ����
	if (g_ParticleLinearUVClipping == 1)
	{
		if (g_ParticleLinearClippingReverseDir == 0)
		{
			if (input.UV.x < input.LifeTimeRatio)
				clip(-1);
		}
		else
		{
			// if (input.UV.x < input.LifeTimeRatio)
			if (input.UV.x > 1 - input.LifeTimeRatio)
				clip(-1);
		}
	}
}

// Apply Noise Texture 
bool ApplyNoiseTextureDestroyEffect(float2 UV, float LifeTimeMax, float LifeTimeRatio, int InstanceID)
{
	// g_ParticleNoiseTextureEffectFilter �� 1���̻����� �ö��� �ʰ� �� ���̴�. (cpu ���� �ش� �ð��� ���� ���̴�)
	if (g_ParticleApplyNoiseTexture == 1)
	{
		// LifeTimeRatio �� g_ParticleNoiseTextureApplyRatio �̻��� �Ǹ� ������� �Ѵ�.
		if (g_ParticleNoiseTextureApplyRatio > LifeTimeRatio)
			return true;

		// InstanceID �� ����, ���� NoiseTexture ���� �����ϴ� UV ������ �ٸ��� �Ѵ�
		// ex) �� 100�� ���� -> 10��° Instance => 0 ~ 0.1 ������ UV ���� �����ϰ� �ϱ�
		float2 NoiseSmpUV = UV + (InstanceID / (g_ParticleSpawnCountMax * 0.1f));

		float4 FinalColor = g_NoiseTexture.Sample(g_BaseSmp, NoiseSmpUV);

		// float ClipLimit = LifeTimeRatio;
		float ClipLimit = (LifeTimeRatio - g_ParticleNoiseTextureApplyRatio) / (1 - g_ParticleNoiseTextureApplyRatio);

		if (FinalColor.x < ClipLimit)
			return false;
	}

	return true;
}

float3 ApplyLinearEmissiveColorChangeEffect(float LifeTimeRatio, float3 OriginEmissive)
{
	if (g_ParticleApplyLinearEmissiveChange == 1)
	{
		float3 ReturnColor;

		ReturnColor.rgb = lerp(g_ParticleStartEmissiveColor,
			g_ParticleEndEmissiveColor, float3(LifeTimeRatio, LifeTimeRatio, LifeTimeRatio));

		return ReturnColor;
	}

	return OriginEmissive;
}


PSOutput_Single ParticlePS(GeometryParticleOutput input)
{
	PSOutput_Single output = (PSOutput_Single)0;

	if (g_DestroyAllExistingLivingParticles == 1)
		clip(-1);

	// ������ Alive �Ǿ��� Particle ���� �ٽ� �����ǰ� �ϱ�
	// if (g_DestroyAllExistingLivingParticles == 1)
	// {
	// 	// ������ ����ִٰ�, ParticleUpdate ���� �ٽ� �����Ƕ�� �� Particle �� �׷����� ���� ���̴�.
	// 	if (g_ParticleArraySRV[input.InstanceID].DestroyedDuringAlive == 1)
	// 		return output;
	// }

	float4 BaseMaterialColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);

	ApplyLinearUVClipping(input);

	if (ApplyNoiseTextureDestroyEffect(input.UV, input.LifeTimeMax, input.LifeTimeRatio, input.InstanceID) == false)
		clip(-1);

	if (BaseMaterialColor.a == 0.f || input.Color.a == 0.f)
		clip(-1);

	float2 ScreenUV = input.ProjPos.xy / input.ProjPos.w;
	ScreenUV = ScreenUV * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	int2 TargetPos = (int2) 0;

	TargetPos.x = (int)(ScreenUV.x * g_Resolution.x);
	TargetPos.y = (int)(ScreenUV.y * g_Resolution.y);

	// Soft Particle
	float4 Depth = g_GBufferDepth.Load(TargetPos, 0);
	float Alpha = 1.f;

	if (Depth.a > 0.f)
		Alpha = (Depth.g - input.ProjPos.w) / 0.5f;

	Alpha = clamp(Alpha, 0.f, 1.f);

	// UV Clipping
	// ������ Texture ����, ���� ��������, ������ ������� ����
	// �ϴ���, Geometry Shader ���� �������� ���̴�.

	// Paper Burn
	BaseMaterialColor = PaperBurn2D(BaseMaterialColor * input.Color, input.UV);

	// float3 EmissiveColor = g_MtrlEmissiveColor.xyz;
	float3 EmissiveColor = g_ParticleStartEmissiveColor.xyz;
	EmissiveColor = ApplyLinearEmissiveColorChangeEffect(input.LifeTimeRatio, EmissiveColor);

	// output.Color = Color;
	output.Color.rgb = (BaseMaterialColor * input.Color).rgb + EmissiveColor.rgb;

	// output.Color = Color;
	// output.Color = Color * input.Color;
	output.Color.a = BaseMaterialColor.a * g_MtrlOpacity * Alpha;

	return output;
}