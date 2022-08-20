#include "ShaderInfo.fx"

struct VertexPos
{
	// ���� �ڿ� : ���������̸� + ��ȣ �� �����Ѵ�.
	// ��ȣ�� �Ⱥ��̸� 0���� �����ȴ�.
    float3 Pos : POSITION; // Vector3Ÿ��.
};

struct VertexPosOutput
{
	// SV�� ������ System Value�̴�. �� ���� �������Ϳ� ���常 �ϰ�
	// ������ ����ϸ� �ȵȴ�.
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
