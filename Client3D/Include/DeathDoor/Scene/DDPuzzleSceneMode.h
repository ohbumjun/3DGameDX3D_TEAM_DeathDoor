#pragma once

#include "DDSceneMode.h"

class CDDPuzzleSceneMode :
    public CDDSceneMode
{
    friend class CScene;

protected:
    CDDPuzzleSceneMode();
    virtual ~CDDPuzzleSceneMode();

public:
    virtual void Start() override;
    virtual void Update(float DeltaTime) override;

public:
    virtual void OnDieMonster() override;
    virtual void OnClearDungeon() override;
    void OnClearCamMoveToClearObjectEnd();
    void OnClearObjectEventEnd();
    void OnClearCamRestoreEnd();

public:
    virtual bool Save(FILE* File) override;
    virtual bool Load(FILE* File) override;

public:
    bool SetBlockerObjectName(const std::string& Name);
    bool SetLadderObjectName(const std::string& Name);
    void SetClearCamMove(bool Move)
    {
        m_ClearCamMove = Move;
    }

    const std::string& GetBlockerObjName()
    {
        return m_BlockerObjectName;
    }

    const std::string& GetLadderObjName()
    {
        return m_LadderObjectName;
    }

    class CGameObject* GetBlockerObject()   const;

    bool IsClearCamMove() const
    {
        return m_ClearCamMove;
    }

protected:
    bool m_ClearCamMove;

    std::string m_BlockerObjectName;
    class CGameObject* m_BlockerObj;
    float m_BlockerOriginY;
    bool m_BlockerDownMoving;

    std::string m_LadderObjectName;
    class CGameObject* m_LadderObj;
    class CPaperBurnComponent* m_LadderPaperBurn;
    std::vector<class CLadderCollider*> m_vecLadderCollider;

    Vector3 m_ClearOriginCamPos;
};

