#include "ShaderInfo.fx"

static const float SampleWeights[13] =
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

#define KernelHalf 6
#define GroupThreads 128

groupshared float4 SharedInput[GroupThreads];

Texture2D<float4> g_InputTex : register(t20);
RWTexture2D<float4> g_OutputTex : register(u7);

// Vertical
[numthreads(GroupThreads, 1, 1)]
void VerticalFilter(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
	int2 Coord = int2(GroupID.x, GroupIndex - KernelHalf + (GroupThreads - KernelHalf * 2) * GroupID.y);

	Coord = clamp(Coord, int2(0, 0), int2(g_DownScaleRS.x - 1, g_DownScaleRS.y - 1));
	SharedInput[GroupIndex] = g_InputTex.Load(int3(Coord, 0));

	GroupMemoryBarrierWithGroupSync();

	// 수직 방향 블러
	if (GroupIndex >= KernelHalf && GroupIndex < (GroupThreads - KernelHalf) &&
		((GroupIndex - KernelHalf + (GroupThreads - KernelHalf * 2) * GroupID.y) < g_DownScaleRS.y))
	{
		float4 Out = 0;
		[unroll]
		for (int i = -KernelHalf; i <= KernelHalf; ++i)
		{
			Out += SharedInput[GroupIndex + i] * SampleWeights[i + KernelHalf];
		}

		g_OutputTex[Coord] = float4(Out.rgb, 1.f);
	}
}

// Horizontal
[numthreads(GroupThreads, 1, 1)]
void HorizontalFilter(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
	int2 Coord = int2(GroupIndex - KernelHalf + (GroupThreads - KernelHalf * 2) * GroupID.x, GroupID.y);
	Coord = clamp(Coord, int2(0, 0), int2(g_DownScaleRS.x - 1, g_DownScaleRS.y - 1));
	SharedInput[GroupIndex] = g_InputTex.Load(int3(Coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (GroupIndex >= KernelHalf && GroupIndex < (GroupThreads - KernelHalf) &&
		((GroupID.x * (GroupThreads - 2 * KernelHalf) + GroupIndex - KernelHalf) < g_DownScaleRS.x))
	{
		float4 Out = 0;
		[unroll]
		for (int i = -KernelHalf; i <= KernelHalf; ++i)
		{
			Out += SharedInput[GroupIndex + i] * SampleWeights[i + KernelHalf];
		}

		g_OutputTex[Coord] = float4(Out.rgb, 1.f);
	}
}
