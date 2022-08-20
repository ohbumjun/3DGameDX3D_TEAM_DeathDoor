
#include "NavMeshConstantBuffer.h"
#include "ConstantBuffer.h"

CNavMeshConstantBuffer::CNavMeshConstantBuffer()	:
	m_BufferData{}
{
}

CNavMeshConstantBuffer::CNavMeshConstantBuffer(const CNavMeshConstantBuffer& Buffer)	:
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CNavMeshConstantBuffer::~CNavMeshConstantBuffer()
{
}

bool CNavMeshConstantBuffer::Init()
{
	SetConstantBuffer("NavMeshCBuffer");

	return true;
}

void CNavMeshConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CNavMeshConstantBuffer* CNavMeshConstantBuffer::Clone()
{
	return new CNavMeshConstantBuffer(*this);
}
