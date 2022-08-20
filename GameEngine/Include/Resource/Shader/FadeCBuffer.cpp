#include "FadeCBuffer.h"
#include "ConstantBuffer.h"

CFadeCBuffer::CFadeCBuffer()	:
	m_BufferData{}
{
}

CFadeCBuffer::CFadeCBuffer(const CFadeCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CFadeCBuffer::~CFadeCBuffer()
{
}

bool CFadeCBuffer::Init()
{
	SetConstantBuffer("FadeCBuffer");

	return true;
}

void CFadeCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CFadeCBuffer* CFadeCBuffer::Clone()
{
	return new CFadeCBuffer(*this);
}
