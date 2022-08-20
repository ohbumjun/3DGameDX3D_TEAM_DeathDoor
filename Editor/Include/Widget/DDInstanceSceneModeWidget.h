#pragma once

#include "DDSceneModeWidget.h"
#include "../DeathDoor/Scene/DDInstanceSceneMode.h"

class CDDInstanceSceneModeWidget :
    public CDDSceneModeWidget
{
public:
    CDDInstanceSceneModeWidget();
    virtual ~CDDInstanceSceneModeWidget();

public:
    virtual bool Init() override;

public:
    virtual void RefreshWidgets() override;

private:
    void OnClickFindEnterColliderObj();
    void OnClickFindBlokcerObj();
    void OnSelectPhase(int Idx, const char* Label);
    void OnClickAddPhase();
    void OnClickDeletePhase();
    void OnChangePhaseInterval(float Val);
    void OnSelectSpawnList(int Idx, const char* Label);
    void OnClickAddSpawnInfo();
    void OnClickDeleteSpawnInfo();
    void OnChangeSetSpawnPoint(const Vector3& Val);
    void OnChangeSetSpawnRot(const Vector3& Val);
    void OnChangeSpawnInterval(float Val);
    void OnClickSetMonster();
    void OnClickAddEndEventObj();
    void OnClickClearEndEventObj();

    void OnChangeSpawnMonsterPos(const Vector3& WorldPos, const Vector3& RelativePos);
    void OnChangeSpawnMonsterRot(const Vector3& WorldRot, const Vector3& RelativeRot);

    class CGameObject* CreatePhaseMonsterSample(int PhaseIdx, DDInstanceSpawnInfo* Info, class CScene* Scene);
    void EnablePhaseMonsterSamples(int PhaseIdx, bool Enable);
    void DestroyPhaseMonsterSample(int PhaseIdx);
    void SetGizmo(int PhaseIdx, int SpawnIdx);
    void SetGizmoNull();

private:
    std::vector<std::vector<class CGameObject*>> m_vecSpawnMonster;

    DDInstanceSpawnInfo* m_CurSpawnInfo;

    int m_CurSelectPhaseIdx;

    class CIMGUITextInput* m_EnterColliderObjName;
    class CIMGUITextInput* m_EnterColliderObjNameInput;
    class CIMGUIButton* m_FindEnterColliderObj;

    class CIMGUITextInput* m_BlockerObjName;
    class CIMGUITextInput* m_BlockerObjNameInput;
    class CIMGUIButton* m_FindBlockerObj;

    class CIMGUIListBox* m_EndEventObjList;
    class CIMGUITextInput* m_EndEventObjNameInput;
    class CIMGUIButton* m_AddEndEventObj;
    class CIMGUIButton* m_ClearEndEventObj;

    class CIMGUIListBox* m_PhaseList;
    class CIMGUIButton* m_AddPhase;
    class CIMGUIButton* m_DeletePhase;

    class CIMGUIInputFloat* m_PhaseInterval;
    class CIMGUIListBox* m_SpawnList;
    class CIMGUIButton* m_AddSpawnInfo;
    class CIMGUIButton* m_DeleteSpawnInfo;

    class CIMGUIComboBox* m_PoolMonsterNames;
    class CIMGUIButton* m_SetPoolMonster;
    class CIMGUIInputFloat3* m_SpawnRotation;
    class CIMGUIInputFloat3* m_SpawnPoint;
    class CIMGUIInputFloat* m_SpawnInterval;
};

