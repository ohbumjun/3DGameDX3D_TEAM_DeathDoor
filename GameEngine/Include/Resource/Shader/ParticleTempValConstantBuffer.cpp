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

	// Clone 해서 사용할 것이다.
	// CParticleTempValConstantBuffer* Buffer =  dynamic_cast<CParticleTempValConstantBuffer*>CResourceManager::GetInst()->FindConstantBuffer("ParticleTempValCBuffer");
	// m_Buffer = Buffer->Clone()->m_Buffer;

	// 처음 Init 될 때 Update Buffer 처리를 한번 해준다.
	// 처음에 생성될 때에는 
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
