
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
	// 1��° Save Load
	CRef::Save(File);

	// 2��° Save , Load => SaveLoad Struct
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

	// ���� ������ Material �� Particle �� �ִٸ�
	// if (m_SaveLoadStruct.MaterialEnable && MtrlFileExist == false) => ���� Particle �� Material �� ������ �ϴ� ��쵵 �ִ�.
	if (m_SaveLoadStruct.MaterialEnable)
	{
		std::string MaterialFullFileName = m_Material->GetName();

		// .mtrl ���θ� Ȯ���Ѵ�. ������ �ٿ��ش�.
		std::string MtrlExt = ".mtrl";

		auto iterFind = std::search(MaterialFullFileName.begin(), MaterialFullFileName.end(),
			std::boyer_moore_searcher(MtrlExt.begin(), MtrlExt.end()));

		if (iterFind == MaterialFullFileName.end())
			MaterialFullFileName.append(".mtrl");

		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PARTICLE_PATH, MaterialFullFileName);

		// ���� Material/Particle �� �������� �ʴ´ٸ�
		// Material/Particle ��ο� �����Ѵ�.
		if (!FoundResult.has_value())
		{
			const PathInfo* ParticleMaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

			// Material �� Bin/Material/ParticleMaterial Path �� �����ϱ�
			std::string MaterialBinPathMutlibyte = ParticleMaterialPath->PathMultibyte;

			MaterialBinPathMutlibyte.append(m_Material->GetName().c_str());

			m_Material->SaveFullPath(MaterialBinPathMutlibyte.c_str());
		}

		// .mtrl Ȯ���ڰ� �پ ����ǵ��� �����ؾ� �Ѵ�.
		m_Material->SaveFullPath(FoundResult.value().c_str());
	}

	// 4��° Save, Load : ��� ���� ���� 
	m_CBuffer->Save(File);


	return true;
}

bool CParticle::Load(FILE* File)
{
	// 1��° Save Load
	CRef::Load(File);

	// 2��° Save, Load
	fread(&m_SaveLoadStruct, sizeof(ParticleSaveLoadStruct), 1, File);

	// 3��° Save, Load => Material �ҷ����� 
	// 3_1) Material �̸��� ���ͼ� Material Manager ���� ã��
	m_Material = CResourceManager::GetInst()->FindMaterial(m_SaveLoadStruct.MaterialName);

	// 3_2) �׷��� ������, �ϵ��ũ���� ã��
	if (!m_Material)
	{
		bool FileFound = false;

		std::string StrParticleFileName;

		// �ش� ������ �����ϴ��� Ȯ���Ѵ�.
		StrParticleFileName = m_SaveLoadStruct.MaterialName;
		StrParticleFileName.append(".mtrl");

		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PARTICLE_PATH, StrParticleFileName);

		// �ش� ��ο� �����Ѵٸ�
		if (FoundResult.has_value())
		{
			FileFound = true;
		}
		else
		{
			// 3_2_b) �׷��� ������ Bin//Material ���� ã���ش�.
			// FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MATERIAL_PATH, StrParticleFileName);
			// 
			// if (FoundResult.has_value())
			// {
			// 	FileFound = true;
			// 
			// 	const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PATH);
			// 
			// 	// ���� ã�Ҵٸ�, Bin//Material//ParticleMaterial Path �� �������ֱ�
			// 	CEngineUtil::CopyFileToOtherDirectory(MaterialPathInfo, ParticleMaterialPathInfo, StrParticleFileName);
			// }
		}

		if (!FileFound)
		{
			// Particle �� ����ϴ� Material ������ ��ο� �������� �ʽ��ϴ�.
			assert("Particle �� ����ϴ� Material ������ Bin//Material//ParticleMaterial ��ο� �������� �ʽ��ϴ�.");
		}

		if (FileFound)
		{
			const std::string& StrParticleMtrlFullPath = FoundResult.value();

			m_Material = CResourceManager::GetInst()->CreateMaterialEmpty<CMaterial>();

			m_Material->LoadFullPath(StrParticleMtrlFullPath.c_str());

			// Material Manager �� Material Map �� �߰�
			// KeyName ��, Material �� Name �� �Ǿ� ���� ���̴�.
			CResourceManager::GetInst()->GetMaterialManager()->AddMaterial(m_Material);
		}
	}

	if (!m_Material)
	{
		assert("Particle Material Load ����");
	}

	m_UpdateShader = (CParticleUpdateShader*)CResourceManager::GetInst()->FindShader(m_SaveLoadStruct.UpdateShaderName);

	m_Info = m_SaveLoadStruct.Info;
	m_InfoShare = m_SaveLoadStruct.InfoShared;
	m_2D = m_SaveLoadStruct.Is2D;
	m_SpawnCountMax = m_SaveLoadStruct.SpawnCountMax;
	m_SpawnTimeMax = m_SaveLoadStruct.SpawnTimeMax;

	// 4��° Save, Load : ��� ���� ���� 
	m_CBuffer = new CParticleConstantBuffer;
	m_CBuffer->Init();
	m_CBuffer->Load(File);

	// ����ȭ ���� ����
	AddStructuredBuffer("ParticleInfo", sizeof(ParticleInfo), m_SpawnCountMax, 0);
	AddStructuredBuffer("ParticleInfoShared", sizeof(ParticleInfoShared), 1, 1);

	// Dynamic True �� ���� => Particle.fx ���� Read Only�θ� ���
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

	// Dynamic True�� ����
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

	// ���� ���� ���� �Լ��� �̿��Ͽ� 0 ~ m_SpawnCountMax �������� �������� ����Ѵ�.
	m_vecNormalDistVal.clear();
	m_vecNormalDistVal.reserve(m_SpawnCountMax);

	std::random_device rd{};
	std::mt19937 gen{ rd() };

	// ǥ�� ���� ���� ���� (0 ���� 1 ������ ���� �ȿ��� ������ �ؾ� �Ѵ�.)
	// ��� 0 / ǥ������ : 0.15 -> -0,5 ~ 0.5 ���̿� ���� ��� ���� ��ġ�� ���̴�
	// ex) m - 3 * ǥ.�� ~ m + 3 * ǥ.�� => 99% �� ������ ��ġ
	// ��, ����� ���� ����� ���̴�. ������� �ϸ�, �ʹ� ������� ��ġ�ϰ� �Ǵ� ������ �ִ�.
	// ���� �� ������ �ϰ� �ʹ�. ���� ǥ�������� 0.15 ���� ���� �� ũ�� �� ���̴�.
	// ��, GPU �������� * 2 �� �ؼ� -1 ���� 1 ������ ������ ������ִµ�
	// �̶� �翬�� -1 ���� 1 ������ ������ ����� �� ���̴�.
	// ������ ��� ������ ���, �ٽ� Random �� Radius �� ��Ƽ� �������ش�.
	// �׷���, ��������� �ܰ��� �ִ� �͵���, �ٽ� �߰� ��򰡷� �����鼭
	// ��ġ�� ��� �Ǹ�, ����� �� ���� ���°� �� ���̴�.
	std::normal_distribution <> dist{0, 0.25};

	std::unordered_map<float, int> hist{};

	for (int i = 0; i < m_SpawnCountMax; ++i)
	{
		float YVal = (float)dist(gen);
		m_vecNormalDistVal.push_back(YVal);

		// ���� ���� Ȯ�ι��
		// ++hist[std::round(YVal * 10) / 10];
	}

	// bool True = true;

	// Update Buffer �� ���, Particle Component �� ������ �� �ѹ� �������ش�.
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