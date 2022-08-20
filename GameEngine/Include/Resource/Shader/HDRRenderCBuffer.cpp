#include "HDRRenderCBuffer.h"
#include "ConstantBuffer.h"

CHDRRenderCBuffer::CHDRRenderCBuffer()	:
	m_BufferData{}
{
	m_BufferData.LumWhite = 1.1f;
	m_BufferData.MiddleGray = 1.f;
	m_BufferData.BloomScale = 1.f;
	m_BufferData.DOFMin = 300.f;
	m_BufferData.DOFMax = 600.f;
}

CHDRRenderCBuffer::CHDRRenderCBuffer(const CHDRRenderCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CHDRRenderCBuffer::~CHDRRenderCBuffer()
{
}

bool CHDRRenderCBuffer::Init()
{
	SetConstantBuffer("HDRRenderCBuffer");

	return true;
}

void CHDRRenderCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CHDRRenderCBuffer* CHDRRenderCBuffer::Clone()
{
	return new CHDRRenderCBuffer(*this);
}
