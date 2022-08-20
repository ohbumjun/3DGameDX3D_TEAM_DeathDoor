
#include "CollisionManager.h"
#include "../Resource/ResourceManager.h"
#include "../EngineUtil.h"
#include "../PathManager.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneCollision.h"

DEFINITION_SINGLE(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

CCollisionManager::~CCollisionManager()
{
	auto	iter = m_mapProfile.begin();
	auto	iterEnd = m_mapProfile.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

bool CCollisionManager::Init()
{
	CreateProfile("Object", Collision_Channel::Object, true);
	CreateProfile("Player", Collision_Channel::Player, true);
	CreateProfile("Monster", Collision_Channel::Monster, true);
	CreateProfile("PlayerAttack", Collision_Channel::PlayerAttack, true);
	CreateProfile("MonsterAttack", Collision_Channel::MonsterAttack, true);
	//CreateProfile("MonsterPathFind", Collision_Channel::MonsterPathFind, true);
	//CreateProfile("MinionNormalAttack", Collision_Channel::Monster, true);
	//CreateProfile("MapObject", Collision_Channel::MapObject, true);

	SetCollisionState("Player", Collision_Channel::Player, Collision_Interaction::Ignore);
	SetCollisionState("Player", Collision_Channel::PlayerAttack, Collision_Interaction::Ignore);
	SetCollisionState("Player", Collision_Channel::Monster, Collision_Interaction::CollisionRigid);

	SetCollisionState("Monster", Collision_Channel::Monster, Collision_Interaction::CollisionRigid);
	SetCollisionState("Monster", Collision_Channel::MonsterAttack, Collision_Interaction::Ignore);
	SetCollisionState("Monster", Collision_Channel::Player, Collision_Interaction::CollisionRigid);

	SetCollisionState("PlayerAttack", Collision_Channel::Player, Collision_Interaction::Ignore);
	SetCollisionState("PlayerAttack", Collision_Channel::PlayerAttack, Collision_Interaction::Ignore);
	SetCollisionState("PlayerAttack", Collision_Channel::MonsterAttack, Collision_Interaction::Ignore);

	SetCollisionState("MonsterAttack", Collision_Channel::Monster, Collision_Interaction::Ignore);
	SetCollisionState("MonsterAttack", Collision_Channel::PlayerAttack, Collision_Interaction::Ignore);
	SetCollisionState("MonsterAttack", Collision_Channel::MonsterAttack, Collision_Interaction::Ignore);

	//SetCollisionState("MapObject", Collision_Channel::MapObject, Collision_Interaction::Ignore);
	//SetCollisionState("MapObject", Collision_Channel::PlayerAttack, Collision_Interaction::Ignore);
	//SetCollisionState("MapObject", Collision_Channel::MonsterAttack, Collision_Interaction::Ignore);

	return true;
}

bool CCollisionManager::CreateProfile(const std::string& Name, Collision_Channel Channel, 
	bool Enable, Collision_Interaction State)
{
	CollisionProfile* Profile = FindProfile(Name);

	if (Profile)
		return false;

	Profile = new CollisionProfile;

	Profile->Name = Name;
	Profile->Channel = Channel;
	Profile->CollisionEnable = Enable;
	Profile->vecInteraction.resize((int)Collision_Channel::Max);

	for (int i = 0; i < (int)Collision_Channel::Max; ++i)
	{
		Profile->vecInteraction[i] = State;
	}
	
	m_mapProfile.insert(std::make_pair(Name, Profile));

	return true;
}

bool CCollisionManager::SetCollisionState(const std::string& Name, Collision_Channel Channel,
	Collision_Interaction State)
{
	CollisionProfile* Profile = FindProfile(Name);

	if (!Profile)
		return false;

	Profile->vecInteraction[(int)Channel] = State;

	return true;
}

CollisionProfile* CCollisionManager::FindProfile(const std::string& Name)
{
	auto	iter = m_mapProfile.find(Name);

	if (iter == m_mapProfile.end())
		return nullptr;

	return iter->second;
}

bool CCollisionManager::DeleteProfile(const std::string& Name)
{
	auto iter = m_mapProfile.find(Name);

	CollisionProfile* Profile = iter->second;

	// 현재 씬에 삭제하려는 프로파일을 가진 충돌체가 있는지 체크, 있다면 삭제 수행하지 않는다.
	bool IsExistColliderHasProfile = CSceneManager::GetInst()->GetScene()->GetCollision()->IsExistColliderHasProfile(Profile);

	if (IsExistColliderHasProfile)
	{
		return false;
	}

	// 삭제 수행
	SAFE_DELETE(iter->second);
	m_mapProfile.erase(iter);

	return true;
}

void CCollisionManager::GetProfileNames(std::vector<std::string>& vecOutName)
{
	vecOutName.reserve(m_mapProfile.size());

	auto iter = m_mapProfile.begin();
	auto iterEnd = m_mapProfile.end();

	for (; iter != iterEnd; ++iter)
	{
		vecOutName.push_back(iter->first);
	}
}

bool CCollisionManager::SaveAsCSV(const char* FileName, const std::string& Path)
{
	char FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(Path);

	if (Info)
	{
		strcpy_s(FullPath, Info->PathMultibyte);
	}

	strcat_s(FullPath, FileName);

	return SaveAsCSVFullPath(FullPath);
}

bool CCollisionManager::SaveAsCSVFullPath(const char* FullPath)
{
	// 파일 이름을 ExcelData 이름으로 사용
	std::string CSVName = CEngineUtil::FilterFileName(FullPath);

	// Make ExcelData
	bool Success = CResourceManager::GetInst()->CreateCSV(CSVName);

	if (!Success)
	{
		return false;
	}

	CExcelData* Data = CResourceManager::GetInst()->FindCSV(CSVName);
	Data->Clear();

	// Make Label
	Data->AddLabel("Channel");
	for (int i = 0; i < (int)Collision_Channel::Max; ++i)
	{
		Data->AddLabel(CEngineUtil::CollisionChannelToString((Collision_Channel)i));
	}

	// Write Profile Data
	std::vector<std::string> ProfileData;
	auto iter = m_mapProfile.begin();
	auto iterEnd = m_mapProfile.end();

	for (; iter != iterEnd; ++iter)
	{
		ProfileData.push_back(CEngineUtil::CollisionChannelToString(iter->second->Channel));

		for (int i = 0; i < (int)Collision_Channel::Max; ++i)
		{
			ProfileData.push_back(CEngineUtil::CollisionInteractionToString(iter->second->vecInteraction[i]));
		}

		Data->SetData(iter->first, ProfileData);
		ProfileData.clear();
	}

	// Save As CSV File
	Success = Data->SaveCSVFullPath(FullPath);
	if (!Success)
	{
		return false;
	}

	// Release Data
	CResourceManager::GetInst()->DeleteCSV(CSVName);

	return true;
}

bool CCollisionManager::LoadProfileFromCSV(const char* FileName, const std::string& Path)
{
	char FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(Path);

	if (Info)
	{
		strcpy_s(FullPath, Info->PathMultibyte);
	}

	strcat_s(FullPath, FileName);

	return LoadProfileFromCSVFullPath(FullPath);
}

bool CCollisionManager::LoadProfileFromCSVFullPath(const char* FullPath)
{
	// 기존 프로파일 삭제
	auto iter = m_mapProfile.begin();
	auto iterEnd = m_mapProfile.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_mapProfile.clear();

	std::string CSVName = "";
	bool Success = CResourceManager::GetInst()->LoadCSVFullPath(CSVName, FullPath);

	if (!Success)
	{
		return false;
	}

	const Table Data = CResourceManager::GetInst()->FindCSV(CSVName)->GetTable();

	// Create Profile
	std::string ProfileName;
	Collision_Channel Channel;

	auto iterCSV = Data.begin();
	auto iterCSVEnd = Data.end();

	for (; iterCSV != iterCSVEnd; ++iterCSV)
	{
		ProfileName = iterCSV->first;
		Channel = CEngineUtil::StringToCollisionChannel((*iterCSV->second)[0]);

		CreateProfile(ProfileName, Channel, true);

		for (int i = 1; i <= (int)Collision_Channel::Max; ++i)
		{
			Collision_Channel InteractionChannel = (Collision_Channel)(i - 1);
			SetCollisionState(ProfileName, InteractionChannel, CEngineUtil::StringToCollisionInteraction((*iterCSV->second)[i]));
		}
	}

	CResourceManager::GetInst()->DeleteCSV(CSVName);

	return true;
}
