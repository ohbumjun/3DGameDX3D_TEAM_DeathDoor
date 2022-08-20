#include "ShaderInfo.fx"

groupshared float SharedPositions[1024];

Texture2DMS<float4> g_HDRTex : register(t120);
StructuredBuffer<float> AverageValues1D : register(t21);
StructuredBuffer<float> PrevAverageLum : register(t22);
RWStructuredBuffer<float> AverageLum : register(u7);
RWTexture2D<float4> HDRDownScale : register(u6); // 1/4 �̹��� �ػ�

float DownScale4x4(uint2 CurPixel, uint GroupThreadID)
{
	float AvgLum = 0.f;

	if (CurPixel.y < g_DownScaleRS.y)
	{
		uint2 FullResPos = uint2(CurPixel * 4);
		float4 DownScaled = float4(0.f, 0.f, 0.f, 0.f);

		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			[unroll]
			for (int j = 0; j < 4; ++j)
			{
				DownScaled += g_HDRTex.Load(FullResPos, 0, int2(j, i));
			}
		}
		DownScaled /= 16.f;

		HDRDownScale[CurPixel.xy] = DownScaled; // DownScale�� �̹��� ����

		AvgLum = dot(DownScaled, LUM_FACTOR);
		SharedPositions[GroupThreadID] = AvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	return AvgLum;
}

float DownScale1024to4(uint DispatchThreadID, uint GroupThreadID, float AvgLum)
{
	[unroll]
	for (uint GroupSize = 4, Step1 = 1, Step2 = 2, Step3 = 3; GroupSize < 1024; GroupSize *= 4, Step1 *= 4, Step2 *= 4, Step3 *= 4)
	{
		if (GroupThreadID % GroupSize == 0)
		{
			// �ֵ� �� �ջ�
			float StepAvgLum = AvgLum;
			StepAvgLum += DispatchThreadID + Step1 < g_DownScaleDomain ? SharedPositions[GroupThreadID + Step1] : AvgLum;
			StepAvgLum += DispatchThreadID + Step2 < g_DownScaleDomain ? SharedPositions[GroupThreadID + Step2] : AvgLum;
			StepAvgLum += DispatchThreadID + Step3 < g_DownScaleDomain ? SharedPositions[GroupThreadID + Step3] : AvgLum;

			// ��� �� ����
			AvgLum = StepAvgLum;
			SharedPositions[GroupThreadID] = StepAvgLum;
		}

		GroupMemoryBarrierWithGroupSync();
	}

	return AvgLum;
}

void DownScale4To1(uint DispatchThreadID, uint GroupThreadID, uint GroupID, float AvgLum)
{
	if (GroupThreadID == 0)
	{
		// ������ �׷쿡 ���� ��� ���
		float FinalAvgLum = AvgLum;
		FinalAvgLum += DispatchThreadID + 256 < g_DownScaleDomain ? SharedPositions[GroupThreadID + 256] : AvgLum;
		FinalAvgLum += DispatchThreadID + 512 < g_DownScaleDomain ? SharedPositions[GroupThreadID + 512] : AvgLum;
		FinalAvgLum += DispatchThreadID + 768 < g_DownScaleDomain ? SharedPositions[GroupThreadID + 768] : AvgLum;
		FinalAvgLum /= 1024.f;

		AverageLum[GroupID] = FinalAvgLum;
	}
}

[numthreads(1024, 1, 1)]
void DownScaleFirstPass(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID,
	uint GroupThreadID : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(DispatchThreadID.x % g_DownScaleRS.x, DispatchThreadID.x / g_DownScaleRS.x);

	// 16�ȼ� �׷��� �ϳ��� �ȼ��� �ٿ� ���� �޸𸮿� ����
	float AvgLum = DownScale4x4(CurPixel, GroupThreadID);

	// 1024���� 4�� �ٿ����
	AvgLum = DownScale1024to4(DispatchThreadID.x, GroupThreadID, AvgLum);

	// 4���� 1�� �ٿ����
	DownScale4To1(DispatchThreadID.x, GroupThreadID, GroupID.x, AvgLum);
}
