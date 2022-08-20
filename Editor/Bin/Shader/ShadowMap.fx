
#include "Standard3DInfo.fx"

SkinningInfo SkinningShadow(float3 Pos, float4 BlendWeight, float4 BlendIndex)
{
	SkinningInfo Info = (SkinningInfo) 0;
    
	if (g_MtrlAnimation3DEnable == 0)
	{
		Info.Pos = Pos;
        
		return Info;
	}
    
	for (int i = 0; i < 4; ++i)
	{
		if (BlendWeight[i] == 0.f)
			continue;
        
		matrix matBone = g_SkinningBoneMatrixArray[(int) BlendIndex[i]];
        
		Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
	}
    
	return Info;
}


Vertex3DOutput Standard3DShadowMapVS(Vertex3D input)
{
	Vertex3DOutput output = (Vertex3DOutput) 0;

	SkinningInfo Info = SkinningShadow(input.Pos, input.BlendWeight, input.BlendIndex);
    
	float3 Pos = Info.Pos;

	output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
	output.Pos = output.ProjPos;

	return output;
}


PSOutput_Single Standard3DShadowMapPS(Vertex3DOutput input)
{
	PSOutput_Single output = (PSOutput_Single) 0;
    
	// ±íÀÌ¸¸ ÀúÀå
	output.Color = (float4) (input.ProjPos.z / input.ProjPos.w);
	output.Color.w = input.ProjPos.w;

	return output;
}

SkinningInfo SkinningShadowInstancing(float3 Pos,
    float4 BlendWeight, float4 BlendIndex, uint InstanceID)
{
	SkinningInfo Info = (SkinningInfo) 0;
    
	if (g_InstancingInfoArray[InstanceID].g_MtrlAnimation3DEnable == 0)
	{
		Info.Pos = Pos;
        
		return Info;
	}
    
	for (int i = 0; i < 4; ++i)
	{
		if (BlendWeight[i] == 0.f)
			continue;
        
		matrix matBone = g_SkinningBoneMatrixArray[InstanceID * g_InstancingBoneCount + (int) BlendIndex[i]];
        
		Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
	}
    
	return Info;

}

Vertex3DOutputInstancing Standard3DShadowInstancingVS(Vertex3DInstancing input)
{
	Vertex3DOutputInstancing output = (Vertex3DOutputInstancing) 0;
    
	SkinningInfo Info = SkinningShadowInstancing(input.Pos,
                    input.BlendWeight, input.BlendIndex, input.InstanceID);
    
	float3 Pos = Info.Pos;

	output.ProjPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWVP);
	output.Pos = output.ProjPos;

	return output;
}

PSOutput_Single Standard3DShadowInstancingPS(Vertex3DOutputInstancing input)
{
	PSOutput_Single output = (PSOutput_Single) 0;
    
	output.Color = (float4) (input.ProjPos.z / input.ProjPos.w);
	output.Color.w = input.ProjPos.w;

	return output;
}
