#include "ShaderInfo.fx"

struct VertexPos
{
	// 변수 뒤에 : 레지스터이름 + 번호 로 지정한다.
	// 번호를 안붙이면 0으로 지정된다.
    float3 Pos : POSITION; // Vector3타입.
};

struct VertexPosOutput
{
	// SV가 붙으면 System Value이다. 이 값은 레지스터에 저장만 하고
	// 가져다 사용하면 안된다.
    float4 Pos : SV_POSITION;
};


VertexPosOutput NavMeshVS(VertexPos input)
{
    VertexPosOutput output = (VertexPosOutput) 0;

    output.Pos = mul(float4(input.Pos, 1.f), g_matWVP);

    return output;
}

PSOutput_Single NavMeshPS(VertexPosOutput input)
{
    PSOutput_Single output = (PSOutput_Single) 0;

    output.Color.rgba = float4(0.f, 1.f, 0.f, 1.f);

    return output;
}
