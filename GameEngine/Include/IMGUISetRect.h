#pragma once

#include "IMGUIWidget.h"

// ImGuizmo가 그려지는 바탕을 뿌려주는 위젯
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

