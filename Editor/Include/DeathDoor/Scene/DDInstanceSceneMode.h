#pragma once

#include "DDSceneMode.h"

struct DDInstanceSpawnInfo
{
    Vector3 SpawnPosition;
    Vector3 SpawnRotation;
    std::string MonsterName;
    float Interval;

    DDInstanceSpawnInfo() :
        Interval(0.f)
    {
    }
};

struct DDInstanceSpawnPhaseInfo
{
    float Interval;
    std::list<DDInstanceSpawnInfo*> SpawnList;

    DDInstanceSpawnPhaseInfo() :
        Interval(1.5f)
    {
    }

    ~DDInstanceSpawnPhaseInfo()
    {
        auto iter = SpawnList.begin();
        auto iterEnd = SpawnList.end();

        for (; iter != iterEnd; ++iter)
        {
            SAFE_DELETE(*iter);
        }

        SpawnList.clear();
    }
};

struct DDSpawnObjectSet
{
    class CPaperBurnComponent* DoorPaperBurn;
    DDInstanceSpawnInfo* Info;
};

struct DDInstanceEndEventObj
{
    class CGameObject* Obj;
    class CPaperBurnComponent* PaperBurnComp;
};

class CDDInstanceSceneMode :
    public CDDSceneMode
{
    friend class CScene;

protected:
    CDDInstanceSceneMode();
    virtual ~CDDInstanceSceneMode();

public:
    virtual void Start() override;
    virtual void Update(float DeltaTime) override;

public:
    virtual bool Save(FILE* File) override;
    virtual bool Load(FILE* File) override;

public:
	virtual void OnDieMonster() override;
    virtual void OnClearDungeon() override;

public:
    void AddSpawnPhase();
    bool DeleteSpawnPhaseInfo(int PhaseIndex);
    bool SetSpawnPhaseInterval(int PhaseIndex, float Interval);

    bool AddSpawnInfo(int PhaseIndex);
    bool AddSpawnInfo(int PhaseIndex, const std::string& MonsterName, 
        const Vector3& SpawnPoint, float Interval);
    bool SetSpawnInfo(int PhaseIndex, int SpawnIndex, const std::string& MonsterName,
        const Vector3& SpawnPoint, float Interval);
    bool DeleteSpawnInfo(int PhaseIndex, int SpawnIndex);
    bool SetSpawnMonsterName(int PhaseIndex, int SpawnIndex, const std::string& MonsterName);
    bool SetSpawnPoint(int PhaseIndex, int SpawnIndex, const Vector3& SpawnPoint);
    bool SetSpawnRotation(int PhaseIndex, int SpawnIndex, const Vector3& SpawnRotation);
    bool SetSpawnInterval(int PhaseIndex, int SpawnIndex, float Interval);

public:
    DDInstanceSpawnPhaseInfo* GetPhaseInfo(int Index);
    DDInstanceSpawnInfo* GetSpawnInfo(int PhaseIndex, int SpawnIndex);
    DDInstanceSpawnInfo* GetSpawnInfo(DDInstanceSpawnPhaseInfo* Phase, int SpawnIndex);
    void SetEnterColliderObjectName(const std::string& Name)
    {
        m_EnterTriggerObjectName = Name;
    }
    void SetEnterCollider(class CColliderBox3D* Collider);
    void SetBlockerObjectName(const std::string& Name)
    {
        m_BlockerObjectName = Name;
    }
    size_t GetPhaseListSize() const 
    {
        return m_SpawnPhaseList.size();
    }
    size_t GetSpawnListSize(int PhaseIdx);
    const std::string& GetEnterTriggerObjName()
    {
        return m_EnterTriggerObjectName;
    }
    const std::string& GetBlockerObjName()
    {
        return m_BlockerObjectName;
    }
    bool AddEndEventObjName(const std::string& Name);
    void ClearEndEventObj();

private:
    bool IsValidPhaseIndex(int Index);
    bool IsValidSpawnIndex(int PhaseIndex, int SpawnIndex);
    bool IsValidSpawnIndex(DDInstanceSpawnPhaseInfo* Phase, int SpawnIndex);
    void OnCollideEnterTrigger(const CollisionResult& Result);
    void OnSpawnDoorPaperBurnEnd();
    void OnSpawnDoorDestroy();

protected:
    class CPaperBurnComponent* m_ExitPaperBurn;

    std::string m_EnterTriggerObjectName;
    class CColliderBox3D* m_EnterTrigger;

    std::vector<std::string> m_vecEndEventObjName;
    std::vector<DDInstanceEndEventObj> m_vecEndEventObj;

    std::string m_BlockerObjectName;
    class CGameObject* m_BlockerObj;
    float m_BlockerOriginY;
    bool m_BlockerUpMoving;
    bool m_BlockerDownMoving;

    bool m_SpawnEventRunning;
    float m_PhaseTimer;
    float m_SpawnTimer;

    DDInstanceSpawnPhaseInfo* m_CurPhase;
    DDInstanceSpawnInfo* m_CurSpawnInfo;
    int m_CurPhaseMonsterCount;
    std::list<DDInstanceSpawnPhaseInfo*> m_SpawnPhaseList;
    std::queue<DDSpawnObjectSet> m_PaperBurnEndSpawnQueue;
    std::queue<class CPaperBurnComponent*> m_DoorPaperburnQueue;

    std::string m_ReturnMusic;
};

