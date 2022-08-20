#include "ShaderInfo.fx"
#include "ParticleData.fx"


struct VertexParticle
{
	float3	Pos	: POSITION;
	uint InstanceID : SV_InstanceID;	// 정점버퍼에 만들어서 넣어주는것이 아니다.
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
	float      LocalXPlusMoveDir : RATIO2; // 기본 1 로 세팅 (만약 Local Space 기준, X 음의 방향으로 이동하면, 0으로 세팅)
};

static float3	g_ParticleLocalPos[4] =
{
	{-0.5f, 0.5f, 0.f},
	{0.5f, 0.5f, 0.f},
	{-0.5f, -0.5f, 0.f},
	{0.5f, -0.5f, 0.f}
};

// in : 값을 함수 안으로 넘겨줄때
// out : 함수안에서 결과를 넘겨받을때. 단 함수 안에서 이 값을 사용한 연산은 할 수 없다.
// inout : 그냥 레퍼런스.
[maxvertexcount(6)]
void ParticleGS(point VertexParticleOutput input[1],
	inout TriangleStream<GeometryParticleOutput> output)
{
	// 점을 사각형으로 확장한다. (점 스프라이트)
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

	// UV Animation 적용
	if (g_ParticleShareSRV[0].UVMoveEnable == 1)
	{
		int UVRowN = g_ParticleShareSRV[0].UVRowN;
		int UVColN = g_ParticleShareSRV[0].UVColN;

		// 가로 UV 단일 크기
		float WUVSize = 1.f / (float)UVColN;
		// 세로 UV 단일 크기 
		float HUVSize = 1.f / (float)UVRowN;

		// ex) 5 * 2 => 10개
		// 9 / 10 => 0.9 
		// 10 * 0.9 => 9번째 UV => Idx 상으로는 8번째 (총 Idx 가 0 에서 9까지 존재) 
		int TotalUVNum = UVRowN * UVColN;
		float FloatCurUVIdx = ((float)g_ParticleArraySRV[InstanceID].LifeTime / (float)(g_ParticleArraySRV[InstanceID].LifeTimeMax)) * (float)TotalUVNum;
		int CurUVIdx = floor(FloatCurUVIdx);
		// CurUVIdx -= 1;

		// row * col = 2 * 5
		// rowS = 0.5 (height)
		// colS  = 0.2 (width)
		// 0.95 * 10 => 9.5 => floor(9.5) => 9;
		// 10개의 칸 중에서, 가장 마지막 번째 칸이다.
		// 9 / 5 => 1행
		// 9 % 5 => 4열
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

	// 회전 (전체 회전 + 각 Particle 회전 정도)
	float3 FinalRotAngle = g_ParticleShareSRV[0].RotationAngle + g_ParticleArraySRV[InstanceID].FinalSeperateRotAngle;

	float3x3 matRot = ComputeRotationMatrix(FinalRotAngle);

	// 4개의 최종 정점정보를 만들어준다.
	for (int i = 0; i < 4; ++i)
	{
		// g_ParticleArraySRV[InstanceID].WorldPos 는 Local Space 상에서의 
		float3	WorldPos = g_ParticleArraySRV[InstanceID].LocalPos + mul(g_ParticleLocalPos[i] * Scale, matRot);

		// Particle Component 의 World Pos 도 더한다.
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

// LifeTime 비율에 따라서, (Texture 기준) 오른쪽에서, 왼쪽 방향으로 점점 사라지게 하기
void ApplyLinearUVClipping(GeometryParticleOutput input)
{
	// UV Cllipping 효과
	// 오른쪽 부터, 왼쪽 방향으로 서서히 사라지는 효과를 줄 것이다.
	// 이를 위해서는 LifeTime 을 사용해야 한다.
	// 핵심은, 향하는 방향의 끝점 부분만 계속 남아있게 하는 것이다.
	// UV 좌표 0 ~ 1. 상에서, 가상의 세로 선이 존재한다고 가정
	// 해당 선이 왼쪽 -> 오른쪽, 오른쪽 -> 왼쪽. 으로 이동 하는 개념
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
	// g_ParticleNoiseTextureEffectFilter 는 1초이상으로 올라가지 않게 될 것이다. (cpu 에서 해당 시간을 제한 중이다)
	if (g_ParticleApplyNoiseTexture == 1)
	{
		// LifeTimeRatio 가 g_ParticleNoiseTextureApplyRatio 이상이 되면 사라지게 한다.
		if (g_ParticleNoiseTextureApplyRatio > LifeTimeRatio)
			return true;

		// InstanceID 에 따라서, 실제 NoiseTexture 에서 참조하는 UV 범위도 다르게 한다
		// ex) 총 100개 생성 -> 10번째 Instance => 0 ~ 0.1 사이의 UV 범위 참조하게 하기
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

	// 기존에 Alive 되었던 Particle 들이 다시 생성되게 하기
	// if (g_DestroyAllExistingLivingParticles == 1)
	// {
	// 	// 기존에 살아있다가, ParticleUpdate 에서 다시 생성되라고 한 Particle 은 그려내지 않을 것이다.
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
	// 오른쪽 Texture 부터, 왼쪽 방향으로, 서서히 사라지는 느낌
	// 일단은, Geometry Shader 에서 구현해줄 것이다.

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