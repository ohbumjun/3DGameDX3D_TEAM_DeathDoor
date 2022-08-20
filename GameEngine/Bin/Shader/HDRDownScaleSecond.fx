#include "ShaderInfo.fx"

#define MAX_GROUPS 64

// 공유 메모리 그룹에 중간 값 저장
groupshared float SharedAvgFinal[MAX_GROUPS];

Texture2DMS<float4> g_HDRTex : register(t120);
StructuredBuffer<float> AverageValues1D : register(t21);
StructuredBuffer<float> PrevAverageLum : register(t22);
RWStructuredBuffer<float> AverageLum : register(u7);

[numthreads(MAX_GROUPS, 1, 1)]
void DownScaleSecondPass(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, uint DispatchThreadID : SV_DispatchThreadID)
{
	// 공유 메모리에 1D 값 저장
	float AvgLum = 0.f;

	if (DispatchThreadID < g_DownScaleGroupSize)
	{
		AvgLum = AverageValues1D[DispatchThreadID.x];
	}
	SharedAvgFinal[DispatchThreadID.x] = AvgLum;

	GroupMemoryBarrierWithGroupSync();

	// 64에서 16으로 다운스케일
	if (DispatchThreadID.x % 4 == 0)
	{
		float StepAvgLum = AvgLum;
		StepAvgLum += DispatchThreadID.x + 1 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 1] : AvgLum;
		StepAvgLum += DispatchThreadID.x + 2 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 2] : AvgLum;
		StepAvgLum += DispatchThreadID.x + 3 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 3] : AvgLum;

		AvgLum = StepAvgLum;
		SharedAvgFinal[DispatchThreadID.x] = StepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	// 16에서 4로 다운스케일
	if (DispatchThreadID.x % 16 == 0)
	{
		float StepAvgLum = AvgLum;
		StepAvgLum += DispatchThreadID.x + 4 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 4] : AvgLum;
		StepAvgLum += DispatchThreadID.x + 8 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 8] : AvgLum;
		StepAvgLum += DispatchThreadID.x + 12 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 12] : AvgLum;

		AvgLum = StepAvgLum;
		SharedAvgFinal[DispatchThreadID.x] = StepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	// 4에서 1로 다운스케일
	if (DispatchThreadID.x == 0)
	{
		float FinalLumValue = AvgLum;
		FinalLumValue += DispatchThreadID.x + 16 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 16] : AvgLum;
		FinalLumValue += DispatchThreadID.x + 32 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 32] : AvgLum;
		FinalLumValue += DispatchThreadID.x + 48 < g_DownScaleGroupSize ? SharedAvgFinal[DispatchThreadID.x + 48] : AvgLum;
		FinalLumValue /= 64.f;

		// 적응 휘도 계산
		float AdaptedAverageLum = lerp(PrevAverageLum[0], FinalLumValue, g_DownScaleAdpatation);
		
		// 최종 값 저장
		AverageLum[0] = max(AdaptedAverageLum, 0.0001f);
	}
}
