
#include "Particle.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/ShaderManager.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneResource.h"
#include "../../Scene/SceneManager.h"
#include "../ResourceManager.h"
#include "../Material/MaterialManager.h"
#include "../../PathManager.h"
#include "../../EngineUtil.h"
// Libraray
#include <random>

CParticle::CParticle() :
	m_CBuffer(nullptr),
	m_SpawnTimeMax(0.001f),
	m_2D(true),
	m_SpawnCountMax(100),
	m_SaveLoadStruct{}
{
}

CParticle::CParticle(const CParticle& particle)
{
	m_CBuffer = particle.m_CBuffer->Clone();
	m_UpdateShader = particle.m_UpdateShader;
}

CParticle::~CParticle()
{
	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		SAFE_DELETE(m_vecStructuredBuffer[i]);
	}

	SAFE_DELETE(m_CBuffer);

}

bool CParticle::Init()
{
	m_CBuffer = new CParticleConstantBuffer;

	if (!m_CBuffer->Init())
		return false;

	if (m_Scene)
		m_UpdateShader = (CParticleUpdateShader*)m_Scene->GetResource()->FindShader("ParticleUpdateShader");

	else
		m_UpdateShader = (CParticleUpdateShader*)CResourceManager::GetInst()->FindShader("ParticleUpdateShader");

	AddStructuredBuffer("ParticleInfo", sizeof(ParticleInfo), m_SpawnCountMax, 0);
	AddStructuredBuffer("ParticleInfoShared", sizeof(ParticleInfoShared), 1, 1);
	
	// CreateNormalDistStructuredBuffer("ParticleNormalDistribution", sizeof(float), m_SpawnCountMax, 20, true);

	return true;
}

bool CParticle::Save(FILE* File)
{
	// 1번째 Save Load
	CRef::Save(File);

	// 2번째 Save , Load => SaveLoad Struct
	m_SaveLoadStruct.MaterialEnable = m_Material ? true : false;
	strcpy_s(m_SaveLoadStruct.MaterialName, m_Material->GetName().c_str());

	m_SaveLoadStruct.UpdateShaderEnable = m_UpdateShader ? true : false;
	if (m_SaveLoadStruct.UpdateShaderEnable)
	{
		strcpy_s(m_SaveLoadStruct.UpdateShaderName, m_UpdateShader->GetName().c_str());
	}

	m_SaveLoadStruct.Info = m_Info;
	m_SaveLoadStruct.InfoShared = m_InfoShare;
	m_SaveLoadStruct.Is2D = m_2D;
	m_SaveLoadStruct.SpawnCountMax = m_SpawnCountMax;
	m_SaveLoadStruct.SpawnTimeMax = m_SpawnTimeMax;

	fwrite(&m_SaveLoadStruct, sizeof(ParticleSaveLoadStruct), 1, File);

	// 현재 저장할 Material 이 Particle 에 있다면
	// if (m_SaveLoadStruct.MaterialEnable && MtrlFileExist == false) => 기존 Particle 용 Material 을 덮어써야 하는 경우도 있다.
	if (m_SaveLoadStruct.MaterialEnable)
	{
		std::string MaterialFullFileName = m_Material->GetName();

		// .mtrl 여부를 확인한다. 없으면 붙여준다.
		std::string MtrlExt = ".mtrl";

		auto iterFind = std::search(MaterialFullFileName.begin(), MaterialFullFileName.end(),
			std::boyer_moore_searcher(MtrlExt.begin(), MtrlExt.end()));

		if (iterFind == MaterialFullFileName.end())
			MaterialFullFileName.append(".mtrl");

		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PARTICLE_PATH, MaterialFullFileName);

		// 만약 Material/Particle 에 존재하지 않는다면
		// Material/Particle 경로에 저장한다.
		if (!FoundResult.has_value())
		{
			const PathInfo* ParticleMaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

			// Material 을 Bin/Material/ParticleMaterial Path 에 저장하기
			std::string MaterialBinPathMutlibyte = ParticleMaterialPath->PathMultibyte;

			MaterialBinPathMutlibyte.append(m_Material->GetName().c_str());

			m_Material->SaveFullPath(MaterialBinPathMutlibyte.c_str());
		}

		// .mtrl 확장자가 붙어서 저장되도록 세팅해야 한다.
		m_Material->SaveFullPath(FoundResult.value().c_str());
	}

	// 4번째 Save, Load : 상수 버퍼 저장 
	m_CBuffer->Save(File);


	return true;
}

bool CParticle::Load(FILE* File)
{
	// 1번째 Save Load
	CRef::Load(File);

	// 2번째 Save, Load
	fread(&m_SaveLoadStruct, sizeof(ParticleSaveLoadStruct), 1, File);

	// 3번째 Save, Load => Material 불러오기 
	// 3_1) Material 이름을 얻어와서 Material Manager 에서 찾기
	m_Material = CResourceManager::GetInst()->FindMaterial(m_SaveLoadStruct.MaterialName);

	// 3_2) 그래도 없으면, 하드디스크에서 찾기
	if (!m_Material)
	{
		bool FileFound = false;

		std::string StrParticleFileName;

		// 해당 파일이 존재하는지 확인한다.
		StrParticleFileName = m_SaveLoadStruct.MaterialName;
		StrParticleFileName.append(".mtrl");

		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PARTICLE_PATH, StrParticleFileName);

		// 해당 경로에 존재한다면
		if (FoundResult.has_value())
		{
			FileFound = true;
		}
		else
		{
			// 3_2_b) 그래도 없으면 Bin//Material 에서 찾아준다.
			// FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PATH, StrParticleFileName);
			// 
			// if (FoundResult.has_value())
			// {
			// 	FileFound = true;
			// 
			// 	const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PATH);
			// 
			// 	// 만약 찾았다면, Bin//Material//ParticleMaterial Path 로 복사해주기
			// 	CEngineUtil::CopyFileToOtherDirectory(MaterialPathInfo, ParticleMaterialPathInfo, StrParticleFileName);
			// }
		}

		if (!FileFound)
		{
			// Particle 이 사용하는 Material 파일이 경로에 존재하지 않습니다.
			assert("Particle 이 사용하는 Material 파일이 Bin//Material//ParticleMaterial 경로에 존재하지 않습니다.");
		}

		if (FileFound)
		{
			const std::string& StrParticleMtrlFullPath = FoundResult.value();

			m_Material = CResourceManager::GetInst()->CreateMaterialEmpty<CMaterial>();

			m_Material->LoadFullPath(StrParticleMtrlFullPath.c_str());

			// Material Manager 의 Material Map 에 추가
			// KeyName 은, Material 의 Name 로 되어 있을 것이다.
			CResourceManager::GetInst()->GetMaterialManager()->AddMaterial(m_Material);
		}
	}

	if (!m_Material)
	{
		assert("Particle Material Load 실패");
	}

	m_UpdateShader = (CParticleUpdateShader*)CResourceManager::GetInst()->FindShader(m_SaveLoadStruct.UpdateShaderName);

	m_Info = m_SaveLoadStruct.Info;
	m_InfoShare = m_SaveLoadStruct.InfoShared;
	m_2D = m_SaveLoadStruct.Is2D;
	m_SpawnCountMax = m_SaveLoadStruct.SpawnCountMax;
	m_SpawnTimeMax = m_SaveLoadStruct.SpawnTimeMax;

	// 4번째 Save, Load : 상수 버퍼 저장 
	m_CBuffer = new CParticleConstantBuffer;
	m_CBuffer->Init();
	m_CBuffer->Load(File);

	// 구조화 버퍼 세팅
	AddStructuredBuffer("ParticleInfo", sizeof(ParticleInfo), m_SpawnCountMax, 0);
	AddStructuredBuffer("ParticleInfoShared", sizeof(ParticleInfoShared), 1, 1);

	// Dynamic True 로 세팅 => Particle.fx 에서 Read Only로만 사용
	// CreateNormalDistStructuredBuffer("ParticleNormalDistribution", sizeof(float), m_SpawnCountMax, 20, true);

	return true;
}

void CParticle::AddStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count, 
	int Register, bool Dynamic, int StructuredBufferShaderType)
{
	CStructuredBuffer* Buffer = new CStructuredBuffer;

	if (!Buffer->Init(Name, Size, Count, Register, Dynamic, StructuredBufferShaderType))
	{
		SAFE_DELETE(Buffer);
		return;
	}

	m_vecStructuredBuffer.push_back(Buffer);

}


bool CParticle::ResizeBuffer(const std::string& Name, unsigned int Size, unsigned int Count,
	int Register, bool Dynamic, int StructuredBufferShaderType)
{
	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		if (m_vecStructuredBuffer[i]->GetName() == Name)
		{
			m_vecStructuredBuffer[i]->Init(Name, Size, Count, Register, Dynamic, StructuredBufferShaderType);

			return true;
		}
	}

	return false;
}


void CParticle::CloneStructuredBuffer(std::vector<CStructuredBuffer*>& vecBuffer)
{
	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		CStructuredBuffer* Buffer = m_vecStructuredBuffer[i]->Clone();

		vecBuffer.push_back(Buffer);
	}
}


bool CParticle::SaveFile(const char* FullPath)
{
	FILE* File;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	bool Result = Save(File);

	fclose(File);

	return Result;
}

bool CParticle::LoadFile(const char* FullPath)
{
	FILE* File;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	bool Result = Load(File);

	fclose(File);

	return Result;
}

void CParticle::SetSpawnCountMax(unsigned int Count)
{
	m_CBuffer->SetSpawnCountMax(Count);

	m_SpawnCountMax = Count;

	ResizeBuffer("ParticleInfo", sizeof(ParticleInfo), m_SpawnCountMax, 0);

	// Dynamic True로 세팅
	// ResizeNormalDistStructuredBuffer("ParticleNormalDistribution", sizeof(float), m_SpawnCountMax, 20, true);
}


/*
void CParticle::CloneNormalDistStructuredBuffer(CStructuredBuffer*& NormalDistBuffer)
{
	if (!m_NormalDistributionBuffer)
		return;

	NormalDistBuffer = m_NormalDistributionBuffer->Clone();
}

void CParticle::GenerateNormalDistribution()
{
	if (!m_NormalDistributionBuffer)
		return;

	// 정규 분포 생성 함수를 이용하여 0 ~ m_SpawnCountMax 개수까지 랜덤값을 계산한다.
	m_vecNormalDistVal.clear();
	m_vecNormalDistVal.reserve(m_SpawnCountMax);

	std::random_device rd{};
	std::mt19937 gen{ rd() };

	// 표준 정규 분포 적용 (0 에서 1 사이의 범위 안에서 나오게 해야 한다.)
	// 평균 0 / 표준편차 : 0.15 -> -0,5 ~ 0.5 사이에 거의 모든 값이 위치할 것이다
	// ex) m - 3 * 표.편 ~ m + 3 * 표.편 => 99% 의 데이터 위치
	// 단, 편법을 조금 사용할 것이다. 정석대로 하면, 너무 가운데에만 위치하게 되는 경향이 있다.
	// 조금 더 퍼지게 하고 싶다. 따라서 표준편차를 0.15 보다 조금 더 크게 줄 것이다.
	// 단, GPU 측에서는 * 2 를 해서 -1 에서 1 사이의 범위를 만들어주는데
	// 이때 당연히 -1 에서 1 사이의 범위를 벗어나게 될 것이다.
	// 범위를 벗어난 값들의 경우, 다시 Random 한 Radius 를 잡아서 세팅해준다.
	// 그러면, 결과적으로 외곽에 있던 것들이, 다시 중간 어딘가로 들어오면서
	// 위치를 잡게 되면, 가운데로 덜 몰린 형태가 될 것이다.
	std::normal_distribution <> dist{0, 0.25};

	std::unordered_map<float, int> hist{};

	for (int i = 0; i < m_SpawnCountMax; ++i)
	{
		float YVal = (float)dist(gen);
		m_vecNormalDistVal.push_back(YVal);

		// 실제 개수 확인방법
		// ++hist[std::round(YVal * 10) / 10];
	}

	// bool True = true;

	// Update Buffer 의 경우, Particle Component 에 세팅할 때 한번 세팅해준다.
	// m_NormalDistributionBuffer->UpdateBuffer(&m_vecNormalDistVal[0], (int)m_vecNormalDistVal.size());
}


bool CParticle::ResizeNormalDistStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count, int Register, bool Dynamic, int StructuredBufferShaderType)
{
	if (!m_NormalDistributionBuffer->Init(Name, Size, Count, Register, Dynamic, StructuredBufferShaderType))
	{
		assert(false);
		return false;
	}

	return true;
}


void CParticle::CreateNormalDistStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count, int Register, bool Dynamic, int StructuredBufferShaderType)
{
	SAFE_DELETE(m_NormalDistributionBuffer);

	CStructuredBuffer* Buffer = new CStructuredBuffer;

	if (!Buffer->Init(Name, Size, Count, Register, Dynamic, StructuredBufferShaderType))
	{
		SAFE_DELETE(Buffer);
		return;
	}

	m_NormalDistributionBuffer = Buffer;

	GenerateNormalDistribution();
}
*/