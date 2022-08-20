#pragma once
#include "IMGUIWidget.h"
class CIMGUILabel :
    public CIMGUIWidget
{
public:
	CIMGUILabel();
	virtual ~CIMGUILabel();

protected:
	ImVec2	m_Align;

public:
	void SetAlign(float x, float y)
	{
		m_Align = ImVec2(x, y);
	}

public:
	virtual bool Init();
	virtual void Render();
};

