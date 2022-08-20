#include "DownScaleCBuffer.h"
#include "ConstantBuffer.h"

CDownScaleCBuffer::CDownScaleCBuffer() :
	m_BufferData{}
{
	m_BufferData.BloomThreshold = 1.f;
}

CDownScaleCBuffer::CDownScaleCBuffer(const CDownScaleCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CDownScaleCBuffer::~CDownScaleCBuffer()
{
}

bool CDownScaleCBuffer::Init()
{
	SetConstantBuffer("DownScaleCBuffer");
	return true;
}

void CDownScaleCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CDownScaleCBuffer* CDownScaleCBuffer::Clone()
{
	return new CDownScaleCBuffer(*this);
}
