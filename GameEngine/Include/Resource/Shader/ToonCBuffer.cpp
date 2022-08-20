#include "ToonCBuffer.h"
#include "ConstantBuffer.h"

CToonCBuffer::CToonCBuffer()	:
	m_BufferData{}
{
	m_BufferData.Type = (int)ToonShaderType::Default;
}

CToonCBuffer::CToonCBuffer(const CToonCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CToonCBuffer::~CToonCBuffer()
{
}

bool CToonCBuffer::Init()
{
	SetConstantBuffer("ToonCBuffer");
	return true;
}

void CToonCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CToonCBuffer* CToonCBuffer::Clone()
{
	return new CToonCBuffer(*this);
}
