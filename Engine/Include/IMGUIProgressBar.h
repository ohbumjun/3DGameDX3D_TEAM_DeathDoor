#pragma once

#include "IMGUIWidget.h"

class CIMGUIProgressBar :
    public CIMGUIWidget
{
public:
	CIMGUIProgressBar();
	virtual ~CIMGUIProgressBar();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetPercent(const float& Percent)
	{
		m_Percent = Percent;
	}

private:
	float m_Percent;
};

