#include "FogCBuffer.h"
#include "ConstantBuffer.h"

CFogCBuffer::CFogCBuffer()
{
	m_BufferData.Color = Vector3(0.f, 0.f, 0.f);
	m_BufferData.Density = 1.f;
	m_BufferData.Type = (int)Fog_Type::Depth;
	m_BufferData.Start = 500.f;
	m_BufferData.End = 950.f;
}

CFogCBuffer::CFogCBuffer(const CFogCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CFogCBuffer::~CFogCBuffer()
{
}

bool CFogCBuffer::Init()
{
	SetConstantBuffer("FogCBuffer");
	return true;
}

void CFogCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CFogCBuffer* CFogCBuffer::Clone()
{
	return new CFogCBuffer(*this);
}
