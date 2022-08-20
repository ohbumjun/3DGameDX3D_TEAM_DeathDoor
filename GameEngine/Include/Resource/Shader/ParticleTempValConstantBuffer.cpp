#include "ParticleTempValConstantBuffer.h"
#include "ConstantBuffer.h"
#include "../ResourceManager.h"

CParticleTempValConstantBuffer::CParticleTempValConstantBuffer()
{
	m_BufferData.ResetParticleSharedInfoSumSpawnCnt = 0;
}

CParticleTempValConstantBuffer::CParticleTempValConstantBuffer(const CParticleTempValConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CParticleTempValConstantBuffer::~CParticleTempValConstantBuffer()
{
}

bool CParticleTempValConstantBuffer::Init()
{
	SetConstantBuffer("ParticleTempValCBuffer"); 

	// Clone �ؼ� ����� ���̴�.
	// CParticleTempValConstantBuffer* Buffer =  dynamic_cast<CParticleTempValConstantBuffer*>CResourceManager::GetInst()->FindConstantBuffer("ParticleTempValCBuffer");
	// m_Buffer = Buffer->Clone()->m_Buffer;

	// ó�� Init �� �� Update Buffer ó���� �ѹ� ���ش�.
	// ó���� ������ ������ 
	m_Buffer->UpdateBuffer(&m_BufferData);

	return true;
}

void CParticleTempValConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CParticleTempValConstantBuffer* CParticleTempValConstantBuffer::Clone()
{
	return new CParticleTempValConstantBuffer(*this);
}
