#pragma once

#include "IMGUIWidget.h"

// ImGuizmo�� �׷����� ������ �ѷ��ִ� ����
class CIMGUISetRect :
    public CIMGUIWidget
{
public:
	CIMGUISetRect();
	~CIMGUISetRect();

public:
	virtual bool Init() override;
	virtual void Render() override;
};

