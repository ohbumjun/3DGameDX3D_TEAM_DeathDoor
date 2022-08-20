
#include "ParticleConstantBuffer.h"
#include "ConstantBuffer.h"

CParticleConstantBuffer::CParticleConstantBuffer() :
	m_BufferData{}
{
	m_BufferData.SpawnCountMax = 100;
	m_BufferData.ColorMin = Vector4(1.f, 1.f, 1.f, 1.f);
	m_BufferData.ColorMax = Vector4(1.f, 1.f, 1.f, 1.f);
	m_BufferData.LifeTimeMin = 0.5f;
	m_BufferData.LifeTimeMax = 1.f;
	m_BufferData.ScaleMin = Vector3(30.f, 30.f, 1.f);
	m_BufferData.ScaleMax = Vector3(30.f, 30.f, 1.f);
	m_BufferData.SpeedStart = 1.f;
	m_BufferData.SpeedEnd = 3.f;
	m_BufferData.StartMin = Vector3(-10.f, -10.f, 0.f);
	m_BufferData.StartMax = Vector3(10.f, 10.f, 0.f);

	m_BufferData.ParticleBounce = 0;
	m_BufferData.ParticleBounceResistance = 0.98f;

	m_BufferData.GenerateRadius = 20.f;
	m_BufferData.IsLoopGenerateRing = 0;

	m_BufferData.AlphaStart = 1.f;
	m_BufferData.AlphaEnd = 1.f;

	m_BufferData.ParticleShapeType = -1;

	m_BufferData.SpecialMoveDirType = -1;

	m_BufferData.IsLifeTimeLinearFromCenter = 0;

	m_BufferData.AlphaLinearFromCenter = 0;

	m_BufferData.SeperateLinerRotate = 0;

	m_BufferData.UVMoveEnable = 0;

	// Alive �� ���ϴ� �� (��, �ǻ츮�� �ʴ� �ڵ�)
	// �׸���, �ѹ��� SpawnCount ��ŭ �����ع����� 
	m_BufferData.DisableNewAlive = 0;

	// DisableNewAlive �߿��� Spawn Time �� �����Ű�� 
	// m_BufferData.ApplySpawnTimeDuringDisableNewAlive = 0;

	// Restart ��ư�� ���� ��
	// ���� ParticleShareInfo ����ȭ ���ۿ��� CurrentSpawnCntSum ������ 0���� ������� ! ��� GPU ���� �޼����� ������ ��
	// Restart ��ư�� ������ �Ǹ�, �ش� ���� 1�� �ٲ��.
	// ��, ���� Frame ������ �ٽ� 0���� �������ش�.
	// m_BufferData.ResetParticleSharedInfoSumSpawnCnt = 0;

	// ó������ UV Clipping ó���� ���� �ʴ´�.
	m_BufferData.LinearUVClippingEnable = 0;

	// Linear UV Clipping ���� �ݴ� 
	m_BufferData.LinearClippingReverseDir = 0;

	// ó������ Speed St, End ���̿� ������ Speed �� ���õǵ��� �Ѵ�.
	// 1) �ش� ���� 1 �� ���, Linear 
	// 2) �ش� ���� 2 �� ���, ���� 
	m_BufferData.SpeedChangeMethod = 0;

	// Noise Texture �� Ȱ���ϴ� ����, ó������ ���� 
	m_BufferData.ApplyNoiseTexture = 0;

	// Noise Texture Apply Ratio -> ó������ ���� X
	m_BufferData.NoiseTextureApplyRatio = 0.f;

	// ó������ Particle Component �� WorldPos �� �ǽð����� ������ �ʰ� �����Ѵ�.
	m_BufferData.FollowRealTimeParticleComponentPos = 0;

	// �Ʒ��� ���� Object Pool ���� Particle �� ��ܼ� ����ͼ� ����� �� �����Ѵ�.
	m_BufferData.DestroyExistingAllLivingParticles = 0;

	// Linear Emissive Change �� ó������ �ݿ����� �ʴ´�
	m_BufferData.ApplyLinearEmissiveChange = 0;
}

CParticleConstantBuffer::CParticleConstantBuffer(const CParticleConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CParticleConstantBuffer::~CParticleConstantBuffer()
{
}

bool CParticleConstantBuffer::Init()
{
	SetConstantBuffer("ParticleCBuffer");

	// ó�� Init �� �� Update Buffer ó���� �ѹ� ���ش�.
	// ó���� ������ ������ 
	m_Buffer->UpdateBuffer(&m_BufferData);

	return true;
}

void CParticleConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CParticleConstantBuffer* CParticleConstantBuffer::Clone()
{
	return new CParticleConstantBuffer(*this);
}

void CParticleConstantBuffer::Save(FILE* File)
{
	// CRef::Save(File);
	fwrite(&m_BufferData, sizeof(ParticleCBuffer), 1, File);
}

void CParticleConstantBuffer::Load(FILE* File)
{
	// CRef::Load(File);

	fread(&m_BufferData, sizeof(ParticleCBuffer), 1, File);
}
