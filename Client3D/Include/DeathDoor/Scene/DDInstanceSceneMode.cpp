#include "DDInstanceSceneMode.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"
#include "Component/PaperBurnComponent.h"
#include "../Component/GameStateComponent.h"
#include "ObjectPool.h"
#include "Resource/ResourceManager.h"

CDDInstanceSceneMode::CDDInstanceSceneMode()
{
	SetTypeID<CDDInstanceSceneMode>();
}

CDDInstanceSceneMode::~CDDInstanceSceneMode()
{
	auto iter = m_SpawnPhaseList.begin();
	auto iterEnd = m_SpawnPhaseList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}

	m_SpawnPhaseList.clear();

	while (!m_PaperBurnEndSpawnQueue.empty())
	{
		DDSpawnObjectSet Set = m_PaperBurnEndSpawnQueue.front();
		m_DoorPaperburnQueue.pop();

		SAFE_DELETE(Set.Info);
	}
}

void CDDInstanceSceneMode::Start()
{
	CDDSceneMode::Start();

	CGameObject* EnterTriggerObj = m_Scene->FindObject(m_EnterTriggerObjectName);

	if (EnterTriggerObj)
	{
		m_EnterTrigger = EnterTriggerObj->FindComponentFromType<CColliderBox3D>();

		if (m_EnterTrigger)
		{
			m_EnterTrigger->AddCollisionCallback(Collision_State::Begin, this, &CDDInstanceSceneMode::OnCollideEnterTrigger);
		}

		EnterTriggerObj->Enable(true);
	}

	m_BlockerObj = m_Scene->FindObject(m_BlockerObjectName);

	if (m_BlockerObj)
	{
		m_BlockerObj->Enable(false);
		m_BlockerObj->FindComponentFromType<CColliderBox3D>()->ForceUpdateCBuffer();
		m_BlockerOriginY = m_BlockerObj->GetWorldPos().y;
	}

	if (m_ExitPointObj)
	{
		m_ExitPaperBurn = m_ExitPointObj->FindObjectComponentFromType<CPaperBurnComponent>();

		m_ExitPointObj->Enable(false);
	}

	if (!m_vecEndEventObjName.empty())
	{
		size_t Size = m_vecEndEventObjName.size();
		m_vecEndEventObj.reserve(Size);

		for (size_t i = 0; i < Size; ++i)
		{
			CGameObject* Obj = m_Scene->FindObject(m_vecEndEventObjName[i]);
			CPaperBurnComponent* PaperBurn = Obj->FindObjectComponentFromType<CPaperBurnComponent>();

			DDInstanceEndEventObj EndObj = {};
			EndObj.Obj = Obj;
			EndObj.PaperBurnComp = PaperBurn;

			m_vecEndEventObj.push_back(EndObj);

			Obj->Enable(false);
		}
	}

	// 모든 소환될 몬스터의 카운트 스폰 카운터에 추가
	auto iter = m_SpawnPhaseList.begin();
	auto iterEnd = m_SpawnPhaseList.end();

	for (; iter != iterEnd; ++iter)
	{
		m_MonsterCount += int((*iter)->SpawnList.size());
	}
}

void CDDInstanceSceneMode::Update(float DeltaTime)
{
	CDDSceneMode::Update(DeltaTime);

	if (m_BlockerObj && m_BlockerUpMoving)
	{
		Vector3 CurPos = m_BlockerObj->GetWorldPos();

		if (CurPos.y - m_BlockerOriginY >= 7.f)
		{
			m_BlockerUpMoving = false;
			m_SpawnEventRunning = true;
		}
		else
		{
			// Blocker UP Moving
			m_BlockerObj->AddWorldPos(0.f, 7.f * DeltaTime, 0.f);
		}
	}

	if (m_BlockerObj && m_BlockerDownMoving)
	{
		// Blocker Down Moving
		Vector3 CurPos = m_BlockerObj->GetWorldPos();

		if (CurPos.y <= m_BlockerOriginY)
		{
			m_BlockerDownMoving = false;
			m_BlockerObj->Enable(false);
		}
		else
		{
			// Blocker UP Moving
			m_BlockerObj->AddWorldPos(0.f, -7.f * DeltaTime, 0.f);
		}
	}

	// Spawn Event 시작한 경우
	if (m_SpawnEventRunning)
	{
		// 진행할 페이즈가 있는 경우
		if (m_CurPhase)
		{
			m_PhaseTimer += DeltaTime;

			// 페이즈간 시간 간격동안 기다림
			if (m_PhaseTimer <= m_CurPhase->Interval)
			{
				return;
			}

			// 현재 스폰 정보가 있는 경우
			if (m_CurSpawnInfo)
			{
				m_SpawnTimer += DeltaTime;

				// 스폰 간격만큼 기다림
				if (m_SpawnTimer <= m_CurSpawnInfo->Interval)
				{
					return;
				}

				// 몬스터 소환함
				// TODO : 몬스터 소환 파티클 추가
				CGameObject* SpawnDoor = CObjectPool::GetInst()->GetMapObject("SpawnDoor", m_Scene);

				if (SpawnDoor)
				{
					CResourceManager::GetInst()->SoundPlay("SpawnDoorAppear");
					SpawnDoor->Start();
					CPaperBurnComponent* DoorPaperBurn = SpawnDoor->FindObjectComponentFromType<CPaperBurnComponent>();
					DoorPaperBurn->SetFinishCallback(this, &CDDInstanceSceneMode::OnSpawnDoorPaperBurnEnd);
					DoorPaperBurn->SetInverse(true);
					DoorPaperBurn->SetEndEvent(PaperBurnEndEvent::None);
					DoorPaperBurn->StartPaperBurn();

					SpawnDoor->SetWorldPos(m_CurSpawnInfo->SpawnPosition);
					SpawnDoor->SetWorldRotation(m_CurSpawnInfo->SpawnRotation);

					// PaperBurn이 끝난 이후 스폰하기 위해, 큐에 넣어넣고 PaperBurn이 끝나면 메모리 해제한다.
					DDSpawnObjectSet ObjSet;
					ObjSet.DoorPaperBurn = DoorPaperBurn;
					ObjSet.Info = m_CurSpawnInfo;
					m_PaperBurnEndSpawnQueue.push(ObjSet);
				}
				else
				{
					// SpawnDoor 오브젝트를 찾아오지 못한 경우는 잘못된 동작이므로 종료
					SAFE_DELETE(m_CurSpawnInfo);
					assert(false);
					return;
				}

				m_CurSpawnInfo = nullptr;
				m_SpawnTimer = 0.f;
				return;
			}
			// 현재 스폰 정보가 없는 경우
			else
			{
				// 현재 페이즈에 소환할 몬스터를 다 소환한 경우
				// OnDieMonster() 에서 현재 페이즈의 몬스터들을 모두 다 처리한 경우 다시 SpawnEvent 발생시킴
				if (m_CurPhase->SpawnList.empty())
				{
					SAFE_DELETE(m_CurPhase);
					m_PhaseTimer = 0.f;
					m_SpawnEventRunning = false;
					return;
				}
				// 새로운 소환 정보를 얻어옴
				else
				{
					m_CurSpawnInfo = m_CurPhase->SpawnList.front();
					m_CurPhase->SpawnList.pop_front();
					m_SpawnTimer = 0.f;
					return;
				}
			}
		}
		else
		{
			// 현재 페이즈를 페이즈 정보 리스트에서 가져옴
			if (!m_SpawnPhaseList.empty())
			{
				m_CurPhase = m_SpawnPhaseList.front();
				m_SpawnPhaseList.pop_front();

				m_CurPhaseMonsterCount = (int)m_CurPhase->SpawnList.size();
			}

			// 더 이상 남아있는 페이즈가 없을 경우, 이벤트 종료
			if (!m_CurPhase)
			{
				m_SpawnEventRunning = false;
				return;
			}

			m_PhaseTimer = 0.f;
		}
	}
}

bool CDDInstanceSceneMode::Save(FILE* File)
{
	CDDSceneMode::Save(File);

	int Length = (int)m_EnterTriggerObjectName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_EnterTriggerObjectName.c_str(), sizeof(char), Length, File);

	Length = (int)m_BlockerObjectName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_BlockerObjectName.c_str(), sizeof(char), Length, File);

	int EndEventObjNameCount = (int)m_vecEndEventObjName.size();
	fwrite(&EndEventObjNameCount, sizeof(int), 1, File);
	for (int i = 0; i < EndEventObjNameCount; ++i)
	{
		Length = (int)m_vecEndEventObjName[i].length();
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(m_vecEndEventObjName[i].c_str(), sizeof(char), Length, File);
	}

	int PhaseCount = (int)m_SpawnPhaseList.size();
	PhaseCount = (int)m_SpawnPhaseList.size();
	fwrite(&PhaseCount, sizeof(int), 1, File);

	auto iterPhase = m_SpawnPhaseList.begin();
	auto iterPhaseEnd = m_SpawnPhaseList.end();

	int SpawnCount = 0;
	for (; iterPhase != iterPhaseEnd; ++iterPhase)
	{
		fwrite(&(*iterPhase)->Interval, sizeof(float), 1, File);

		SpawnCount = (int)(*iterPhase)->SpawnList.size();
		fwrite(&SpawnCount, sizeof(int), 1, File);

		auto iterSpawnInfo = (*iterPhase)->SpawnList.begin();
		auto iterSpawnInfoEnd = (*iterPhase)->SpawnList.end();

		for (; iterSpawnInfo != iterSpawnInfoEnd; ++iterSpawnInfo)
		{
			Length = (int)(*iterSpawnInfo)->MonsterName.length();
			fwrite(&Length, sizeof(int), 1, File);
			fwrite((*iterSpawnInfo)->MonsterName.c_str(), sizeof(char), Length, File);
			fwrite(&(*iterSpawnInfo)->SpawnPosition, sizeof(Vector3), 1, File);
			fwrite(&(*iterSpawnInfo)->SpawnRotation, sizeof(Vector3), 1, File);
			fwrite(&(*iterSpawnInfo)->Interval, sizeof(float), 1, File);
		}
	}

	return true;
}

bool CDDInstanceSceneMode::Load(FILE* File)
{
	CDDSceneMode::Load(File);

	int Length = 0;
	char Buf[128] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_EnterTriggerObjectName = Buf;

	ZeroMemory(Buf, Length);
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_BlockerObjectName = Buf;

	int EndEventObjNameCount = 0;
	fread(&EndEventObjNameCount, sizeof(int), 1, File);
	m_vecEndEventObjName.resize(EndEventObjNameCount);

	for (int i = 0; i < EndEventObjNameCount; ++i)
	{
		ZeroMemory(Buf, Length);
		fread(&Length, sizeof(int), 1, File);
		fread(Buf, sizeof(char), Length, File);
		m_vecEndEventObjName[i] = Buf;
	}

	int PhaseCount = 0;
	fread(&PhaseCount, sizeof(int), 1, File);

	char NameBuf[128] = {};
	for (int i = 0; i < PhaseCount; ++i)
	{
		DDInstanceSpawnPhaseInfo* Phase = new DDInstanceSpawnPhaseInfo;
		m_SpawnPhaseList.push_back(Phase);

		fread(&Phase->Interval, sizeof(float), 1, File);

		int SpawnCount = 0;
		fread(&SpawnCount, sizeof(int), 1, File);

		for (int j = 0; j < SpawnCount; ++j)
		{
			DDInstanceSpawnInfo* SpawnInfo = new DDInstanceSpawnInfo;
			ZeroMemory(NameBuf, Length);
			fread(&Length, sizeof(int), 1, File);
			fread(NameBuf, sizeof(char), Length, File);
			SpawnInfo->MonsterName = NameBuf;
			fread(&SpawnInfo->SpawnPosition, sizeof(Vector3), 1, File);
			fread(&SpawnInfo->SpawnRotation, sizeof(Vector3), 1, File);
			fread(&SpawnInfo->Interval, sizeof(float), 1, File);
			Phase->SpawnList.push_back(SpawnInfo);
		}
	}

	return true;
}

void CDDInstanceSceneMode::OnDieMonster()
{
	CDDSceneMode::OnDieMonster();

	--m_CurPhaseMonsterCount;

	// 현재 페이즈의 몬스터가 전부 처리된 경우 몬스터 스폰 이벤트 다시 시작함
	if (m_CurPhaseMonsterCount == 0 && m_MonsterCount != 0)
	{
		m_SpawnEventRunning = true;
	}
}

void CDDInstanceSceneMode::OnClearDungeon()
{
	CDDSceneMode::OnClearDungeon();

	if (m_ExitPaperBurn)
	{
		m_ExitPaperBurn->SetInverse(true);
		m_ExitPaperBurn->SetEndEvent(PaperBurnEndEvent::Reset);
		m_ExitPaperBurn->StartPaperBurn();
	}

	m_BlockerDownMoving = true;
	m_BlockerUpMoving = false;

	size_t Size = m_vecEndEventObj.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_vecEndEventObj[i].Obj->Enable(true);
		m_vecEndEventObj[i].PaperBurnComp->SetInverse(true);
		m_vecEndEventObj[i].PaperBurnComp->SetEndEvent(PaperBurnEndEvent::Reset);
		m_vecEndEventObj[i].PaperBurnComp->StartPaperBurn();
	}

	// 다시 BGM 재생
	CResourceManager::GetInst()->SoundStop("InstanceDungeonBGM");
	CResourceManager::GetInst()->SoundPlay(m_ReturnMusic);
}

void CDDInstanceSceneMode::AddSpawnPhase()
{
	DDInstanceSpawnPhaseInfo* PhaseInfo = new DDInstanceSpawnPhaseInfo;

	m_SpawnPhaseList.push_back(PhaseInfo);
}

bool CDDInstanceSceneMode::AddSpawnInfo(int PhaseIndex)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* Info = new DDInstanceSpawnInfo;
	Phase->SpawnList.push_back(Info);

	++m_MonsterCount;

	return true;
}

bool CDDInstanceSceneMode::AddSpawnInfo(int PhaseIndex, const std::string& MonsterName,
	const Vector3& SpawnPoint, float Interval)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* Info = new DDInstanceSpawnInfo;

	Info->MonsterName = MonsterName;
	Info->SpawnPosition = SpawnPoint;
	Info->Interval = Interval;

	Phase->SpawnList.push_back(Info);

	++m_MonsterCount;

	return true;
}

bool CDDInstanceSceneMode::DeleteSpawnPhaseInfo(int PhaseIndex)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	auto iter = m_SpawnPhaseList.begin();
	auto iterEnd = m_SpawnPhaseList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == Phase)
		{
			m_MonsterCount -= (int)(*iter)->SpawnList.size();
			m_SpawnPhaseList.erase(iter);
			return true;
		}
	}

	return false;
}

bool CDDInstanceSceneMode::SetSpawnPhaseInterval(int PhaseIndex, float Interval)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	Phase->Interval = Interval;

	return true;
}

bool CDDInstanceSceneMode::SetSpawnInfo(int PhaseIndex, int SpawnIndex, const std::string& MonsterName, const Vector3& SpawnPoint, float Interval)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	SpawnInfo->MonsterName = MonsterName;
	SpawnInfo->SpawnPosition = SpawnPoint;
	SpawnInfo->Interval = Interval;

	return true;
}

bool CDDInstanceSceneMode::DeleteSpawnInfo(int PhaseIndex, int SpawnIndex)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	if (!SpawnInfo)
	{
		return false;
	}

	auto iter = Phase->SpawnList.begin();
	auto iterEnd = Phase->SpawnList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == SpawnInfo)
		{
			Phase->SpawnList.erase(iter);
			--m_MonsterCount;
			return true;
		}
	}

	return false;
}

bool CDDInstanceSceneMode::SetSpawnMonsterName(int PhaseIndex, int SpawnIndex, const std::string& MonsterName)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	if (!SpawnInfo)
	{
		return false;
	}

	SpawnInfo->MonsterName = MonsterName;
}

bool CDDInstanceSceneMode::SetSpawnPoint(int PhaseIndex, int SpawnIndex, const Vector3& SpawnPoint)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	if (!SpawnInfo)
	{
		return false;
	}

	SpawnInfo->SpawnPosition = SpawnPoint;
}

bool CDDInstanceSceneMode::SetSpawnRotation(int PhaseIndex, int SpawnIndex, const Vector3& SpawnRotation)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	if (!SpawnInfo)
	{
		return false;
	}

	SpawnInfo->SpawnRotation = SpawnRotation;
}

bool CDDInstanceSceneMode::SetSpawnInterval(int PhaseIndex, int SpawnIndex, float Interval)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return false;
	}

	DDInstanceSpawnInfo* SpawnInfo = GetSpawnInfo(Phase, SpawnIndex);

	if (!SpawnInfo)
	{
		return false;
	}

	SpawnInfo->Interval = Interval;
}

void CDDInstanceSceneMode::SetEnterCollider(CColliderBox3D* Collider)
{
	m_EnterTrigger = Collider;
}

size_t CDDInstanceSceneMode::GetSpawnListSize(int PhaseIdx)
{
	DDInstanceSpawnPhaseInfo* PhaseInfo = GetPhaseInfo(PhaseIdx);

	if (!PhaseInfo)
	{
		return -1;
	}

	return PhaseInfo->SpawnList.size();
}

bool CDDInstanceSceneMode::AddEndEventObjName(const std::string& Name)
{
	CGameObject* ExistObj = m_Scene->FindObject(Name);

	if (!ExistObj)
	{
		return false;
	}

	CPaperBurnComponent* PaperBurn = ExistObj->FindComponentFromType<CPaperBurnComponent>();

	if (!PaperBurn)
	{
		return false;
	}

	m_vecEndEventObjName.push_back(Name);

	return true;
}

void CDDInstanceSceneMode::ClearEndEventObj()
{
	m_vecEndEventObj.clear();
}

bool CDDInstanceSceneMode::IsValidPhaseIndex(int Index)
{
	if (Index <= -1 || m_SpawnPhaseList.size() - 1 < Index)
	{
		return false;
	}

	return true;
}

bool CDDInstanceSceneMode::IsValidSpawnIndex(int PhaseIndex, int SpawnIndex)
{
	if (!IsValidPhaseIndex(PhaseIndex))
	{
		return false;
	}

	DDInstanceSpawnPhaseInfo* Info = GetPhaseInfo(PhaseIndex);

	if (SpawnIndex <= -1 || Info->SpawnList.size() <= SpawnIndex)
	{
		return false;
	}

	return true;
}

bool CDDInstanceSceneMode::IsValidSpawnIndex(DDInstanceSpawnPhaseInfo* Phase, int SpawnIndex)
{
	if (SpawnIndex <= -1 || Phase->SpawnList.size() <= SpawnIndex)
	{
		return false;
	}

	return true;
}

DDInstanceSpawnPhaseInfo* CDDInstanceSceneMode::GetPhaseInfo(int Index)
{
	if (!IsValidPhaseIndex(Index))
	{
		return nullptr;
	}

	auto iter = m_SpawnPhaseList.begin();
	auto iterEnd = m_SpawnPhaseList.end();

	int Count = 0;
	for (; iter != iterEnd; ++iter)
	{
		if (Count == Index)
		{
			return (*iter);
		}
		++Count;
	}

	return nullptr;
}

DDInstanceSpawnInfo* CDDInstanceSceneMode::GetSpawnInfo(int PhaseIndex, int SpawnIndex)
{
	DDInstanceSpawnPhaseInfo* Phase = GetPhaseInfo(PhaseIndex);

	if (!Phase)
	{
		return nullptr;
	}

	return GetSpawnInfo(Phase, SpawnIndex);
}

DDInstanceSpawnInfo* CDDInstanceSceneMode::GetSpawnInfo(DDInstanceSpawnPhaseInfo* Phase, int SpawnIndex)
{
	if (!IsValidSpawnIndex(Phase, SpawnIndex))
	{
		return nullptr;
	}

	auto iter = Phase->SpawnList.begin();
	auto iterEnd = Phase->SpawnList.end();

	int Count = 0;
	for (; iter != iterEnd; ++iter)
	{
		if (Count == SpawnIndex)
		{
			return (*iter);
		}
		++Count;
	}

	return nullptr;
}

void CDDInstanceSceneMode::OnCollideEnterTrigger(const CollisionResult& Result)
{
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Player && Result.Dest->GetGameObject() == Player)
	{
		m_BlockerUpMoving = true;
		m_SpawnEventRunning = true;

		if (m_BlockerObj)
		{
			CResourceManager::GetInst()->SoundPlay("BlockerUp");
			m_BlockerObj->Enable(true);
		}

		m_EnterTrigger->GetGameObject()->Enable(false);

		// 현재 BGM 잠깐 멈추고 인스턴스 던전 BGM으로 변경
		m_ReturnMusic= m_Scene->GetSceneSaveGlobalData().BackGroundData.MusicKeyName;

		if (m_ReturnMusic.empty())
		{
			m_ReturnMusic = m_Scene->GetSceneSaveGlobalData().BackGroundData.PrevMusicKeyName;
		}

		CResourceManager::GetInst()->SoundStop(m_ReturnMusic);
		CResourceManager::GetInst()->SoundPlay("InstanceDungeonBGM");
	}
}

void CDDInstanceSceneMode::OnSpawnDoorPaperBurnEnd()
{
	DDSpawnObjectSet Set = m_PaperBurnEndSpawnQueue.front();
	m_PaperBurnEndSpawnQueue.pop();

	Vector3 SpawnPos = Set.Info->SpawnPosition;

	// 몬스터를 소환한다.
 	CGameObject* Monster = CObjectPool::GetInst()->GetMonster(Set.Info->MonsterName, m_Scene);

	Monster->SetWorldPos(SpawnPos);
	Monster->SetWorldRotation(Set.Info->SpawnRotation);
	CResourceManager::GetInst()->SoundPlay("EnemySpawn");

	// Spawn Particle
	CGameObject* Particle = CObjectPool::GetInst()->GetParticle("SpawnEffect", m_Scene);
	SpawnPos.y += 2.f;
	Particle->StartParticle(SpawnPos);

	CGameStateComponent* State = Monster->FindObjectComponentFromType<CGameStateComponent>();
	State->SetTreeUpdate(true);

	// 페이퍼번 정재생 한 뒤 파괴하기 위해 큐에 넣는다.
	CResourceManager::GetInst()->SoundPlay("SpawnDoorDisappear");
	Set.DoorPaperBurn->SetInverse(false);
	Set.DoorPaperBurn->SetFinishCallback(this, &CDDInstanceSceneMode::OnSpawnDoorDestroy);
	Set.DoorPaperBurn->StartPaperBurn();
	m_DoorPaperburnQueue.push(Set.DoorPaperBurn);

	// 메모리 해제
	SAFE_DELETE(Set.Info);
}

void CDDInstanceSceneMode::OnSpawnDoorDestroy()
{
	CPaperBurnComponent* PaperBurn = m_DoorPaperburnQueue.front();
	m_DoorPaperburnQueue.pop();

	PaperBurn->GetGameObject()->Destroy();
}

