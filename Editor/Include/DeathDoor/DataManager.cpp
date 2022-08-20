
#include "DataManager.h"
#include "Resource/ResourceManager.h"
#include "Component/State/StateManager.h"
#include "Scene/SceneManager.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ObjectDataComponent.h"

#include <sstream>

DEFINITION_SINGLE(CDataManager)

CDataManager::CDataManager()
{
}

CDataManager::~CDataManager()
{

}

void CDataManager::Init()
{
	ReadObjectData();

	// LoadSoundData();
}

void CDataManager::ReadObjectData()
{
	CResourceManager::GetInst()->LoadCSV("ObjectData.csv");

	CExcelData* Data = CResourceManager::GetInst()->FindCSV("ObjectData");

	if (Data)
	{
		m_mapData.insert(std::make_pair("ObjectData", Data));
	}
}

void CDataManager::LoadSoundData()
{
	CResourceManager::GetInst()->LoadCSV("Sound.csv");

	CExcelData* Excel = CResourceManager::GetInst()->FindCSV("DeathDoor_Sound");

	if (Excel)
	{
		m_mapData.insert(std::make_pair("SoundData", Excel));
	}

	const Table& table = Excel->GetTable();
	size_t LabelCount = Excel->GetLabels().size();
	size_t Index = 0;

	auto iter = table.begin();
	auto iterEnd = table.end();

	for (; iter != iterEnd; ++iter)
	{
		Row* row = iter->second;
		std::stringstream ss;
		std::string StrFileName;
		std::string ChannelGroup;
		std::string Key = iter->first;
		int Volume = 0;

		char FileName[MAX_PATH] = {};
		bool Loop = false;

		for (size_t i = 0; i < LabelCount; ++i)
		{
			ss << (*row)[i];

			if (i == 0)
			{
				ChannelGroup = ss.str();
			}

			else if (i == 1)
			{
				std::string IsLoop = ss.str();

				if (IsLoop == "1")
					Loop = true;
			}

			else if (i == 2)
			{
				StrFileName = ss.str();
				strcpy_s(FileName, StrFileName.c_str());
			}

			else if (i == 3)
			{
				ss >> Volume;
			}

			ss.str("");
		}
	
		CResourceManager::GetInst()->LoadSound(ChannelGroup, Loop, Key, FileName);
		CSound* Sound = CResourceManager::GetInst()->FindSound(Key);
		Sound->SetVolume(Volume);

		++Index;
	}

}


// TODO : 데이터 추가될 때마다 함수 업데이트 할 것
ObjectData CDataManager::GetObjectData(const std::string& Key)
{
	ObjectData Data = {};

	CExcelData* Excel = FindData("ObjectData");

	Row* row = Excel->GetRow(Key);

	if (!row)
	{
		return Data;
	}

	size_t Count = row->size();

	for (size_t i = 0; i < Count; ++i)
	{
		std::stringstream ss;

		ss << (*row)[i];

		int Frame = 0;

		ss >> Frame;

		if (i == 0)
		{
			Data.HP = Frame;
		}
		else if (i == 1)
		{
			Data.DetectRange = (float)Frame;
		}
		else if (i == 2)
		{
			Data.MoveSpeed = (float)Frame;
		}
		else if (i == 3)
		{
			Data.Attack = (int)Frame;
		}
		else if (i == 4)
		{
			Data.MeleeAttackRange = (float)Frame;
		}
		else if (i == 5)
		{
			Data.JumpAttackRange = (float)Frame;
		}
		else if (i == 6)
		{
			Data.RotateSpeedPerSec = (float)Frame;
		}
		else if (i == 7)
		{
			Data.FlySpeed = (float)Frame;
		}
	}

	return Data;
}

//int CDataManager::GetObjectPoolData(const std::string& Key)
//{
//	int PoolCount = 0;
//
//	CExcelData* Excel = FindData("ObjectPoolData");
//
//	Row* row = Excel->GetRow(Key);
//
//	if (!row)
//	{
//		return PoolCount;
//	}
//
//	size_t Count = row->size();
//
//	for (size_t i = 0; i < Count; ++i)
//	{
//		std::stringstream ss;
//
//		ss << (*row)[i];
//
//		int Frame = 0;
//
//		ss >> Frame;
//
//		if (i == 0)
//		{
//			PoolCount = Frame;
//		}
//	}
//
//	return PoolCount;
//}

CExcelData* CDataManager::FindData(const std::string& Name)
{
	auto iter = m_mapData.find(Name);

	if (iter == m_mapData.end())
		return nullptr;

	return iter->second;
}
