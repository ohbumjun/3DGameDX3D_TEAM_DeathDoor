
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

	// Alive 를 안하는 것 (즉, 되살리지 않는 코드)
	// 그리고, 한번에 SpawnCount 만큼 생성해버리기 
	m_BufferData.DisableNewAlive = 0;

	// DisableNewAlive 중에도 Spawn Time 을 적용시키기 
	// m_BufferData.ApplySpawnTimeDuringDisableNewAlive = 0;

	// Restart 버튼을 위한 것
	// 공유 ParticleShareInfo 구조화 버퍼에서 CurrentSpawnCntSum 정보를 0으로 만들어줘 ! 라고 GPU 측에 메세지를 보내는 것
	// Restart 버튼을 누르게 되면, 해당 값이 1로 바뀐다.
	// 단, 다음 Frame 에서는 다시 0으로 세팅해준다.
	// m_BufferData.ResetParticleSharedInfoSumSpawnCnt = 0;

	// 처음에는 UV Clipping 처리를 하지 않는다.
	m_BufferData.LinearUVClippingEnable = 0;

	// Linear UV Clipping 방향 반대 
	m_BufferData.LinearClippingReverseDir = 0;

	// 처음에는 Speed St, End 사이에 랜덤한 Speed 가 세팅되도록 한다.
	// 1) 해당 값이 1 일 경우, Linear 
	// 2) 해당 값이 2 일 경우, 지수 
	m_BufferData.SpeedChangeMethod = 0;

	// Noise Texture 를 활용하는 것은, 처음에는 방지 
	m_BufferData.ApplyNoiseTexture = 0;

	// Noise Texture Apply Ratio -> 처음부터 적용 X
	m_BufferData.NoiseTextureApplyRatio = 0.f;

	// 처음에는 Particle Component 의 WorldPos 를 실시간으로 따라가지 않게 세팅한다.
	m_BufferData.FollowRealTimeParticleComponentPos = 0;

	// 아래의 값은 Object Pool 에서 Particle 을 당겨서 끌어와서 사용할 때 적용한다.
	m_BufferData.DestroyExistingAllLivingParticles = 0;

	// Linear Emissive Change 는 처음에는 반영하지 않는다
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

	// 처음 Init 될 때 Update Buffer 처리를 한번 해준다.
	// 처음에 생성될 때에는 
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
