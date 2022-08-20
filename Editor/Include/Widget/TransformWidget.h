#pragma once

#include "IMGUIWidgetList.h"

class CTransformWidget :
    public CIMGUIWidgetList
{
public:
    CTransformWidget();
    virtual ~CTransformWidget();

public:
    virtual bool Init() override;

public:
    // �ݵ�� Init ���� �� �Լ� ȣ���ؾ� ��
    void SetSceneCompoent(class CSceneComponent* Component);

// Transform���� ȣ��
public:
    void OnChangeTransformScale(const Vector3& WorldScale, const Vector3& RelativeScale);
    void OnChangeTransformPos(const Vector3& WorldPos, const Vector3& RelativePos);
    void OnChangeTransformRot(const Vector3& WorldRot, const Vector3& RelativeRot);
    void OnTransformDestroy();

// �������� �� ����� ȣ��
private:
    void OnCheckInherit(int Idx, bool Check);
    void OnChangeWorldPos(const Vector3& Pos);
    void OnChangeWorldRot(const Vector3& Rot);
    void OnChangeWorldScale(const Vector3& Scale);
    void OnChangeRelativePos(const Vector3& Pos);
    void OnChangeRelativeRot(const Vector3& Rot);
    void OnChangeRelativeScale(const Vector3& Scale);

private:
    class CSceneComponent* m_SceneComponent;
    class CTransform* m_Transform;

    class CIMGUICheckBox* m_InheritCheckBox;

    class CIMGUITree* m_WorldTree;
    class CIMGUIInputFloat3* m_WorldPosInput;
    class CIMGUIInputFloat3* m_WorldRotInput;
    class CIMGUIInputFloat3* m_WorldScaleInput;

    class CIMGUITree* m_RelativeTree;
    class CIMGUIInputFloat3* m_RelativePosInput;
    class CIMGUIInputFloat3* m_RelativeRotInput;
    class CIMGUIInputFloat3* m_RelativeScaleInput;
};

