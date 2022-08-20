#include "GlobalLightCBuffer.h"
#include "ConstantBuffer.h"

CGlobalLightCBuffer::CGlobalLightCBuffer()	:
	m_BufferData{}
{
	m_BufferData.AmbientIntensity = 1.f;
}

CGlobalLightCBuffer::CGlobalLightCBuffer(const CGlobalLightCBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CGlobalLightCBuffer::~CGlobalLightCBuffer()
{
}

bool CGlobalLightCBuffer::Init()
{
	SetConstantBuffer("GlobalLightCBuffer");
	return true;
}

void CGlobalLightCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CGlobalLightCBuffer* CGlobalLightCBuffer::Clone()
{
	return new CGlobalLightCBuffer(*this);
}
