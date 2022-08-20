#pragma once
#include "SceneComponentWidget.h"

class CNavMeshComponentWidget :
    public CSceneComponentWidget
{
public:
    CNavMeshComponentWidget();
    virtual ~CNavMeshComponentWidget();

public:
    virtual bool Init() override;

public:
    virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
    void OnClickLoadMesh();
    void OnClickDebugRender(int Idx, bool Check);
    void OnChangePlayerSpawnPos();

private:
    void RefreshMeshWidget(class CMesh* Mesh);

private:
    class CIMGUITextInput* m_MeshName;
    class CIMGUIButton* m_LoadMeshButton;
    class CIMGUICheckBox* m_DebugRenderCheckBox;
    class CIMGUIInputFloat3* m_PlayerSpawnPosInput;
    class CIMGUIButton* m_PlayerSpawnPosSetButton;
};

