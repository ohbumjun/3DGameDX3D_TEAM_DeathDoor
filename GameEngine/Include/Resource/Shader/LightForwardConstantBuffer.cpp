#include "LightForwardConstantBuffer.h"
#include "ConstantBuffer.h"

CLightForwardConstantBuffer::CLightForwardConstantBuffer()	:
	m_BufferData{}
{
}

CLightForwardConstantBuffer::CLightForwardConstantBuffer(const CLightForwardConstantBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CLightForwardConstantBuffer::~CLightForwardConstantBuffer()
{
}

bool CLightForwardConstantBuffer::Init()
{
	SetConstantBuffer("LightForwardCBuffer");

	return true;
}

void CLightForwardConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CLightForwardConstantBuffer* CLightForwardConstantBuffer::Clone()
{
	return new CLightForwardConstantBuffer(*this);
}
