#include "WaterCBuffer.h"
#include "ConstantBuffer.h"

CWaterCBuffer::CWaterCBuffer()	:
	m_BufferData{}
{
	m_BufferData.Speed = 1.f;
	m_BufferData.FoamDepthThreshold = 5.f;
	m_BufferData.Attn1 = 0.3f;
	m_BufferData.Attn2 = 0.35f;
}

CWaterCBuffer::CWaterCBuffer(const CWaterCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CWaterCBuffer::~CWaterCBuffer()
{
}

bool CWaterCBuffer::Init()
{
	SetConstantBuffer("WaterCBuffer");
	return true;
}

void CWaterCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CWaterCBuffer* CWaterCBuffer::Clone()
{
	return new CWaterCBuffer(*this);
}
