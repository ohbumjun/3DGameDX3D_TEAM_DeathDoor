#pragma once

#include "ConstantBufferBase.h"

class CFadeCBuffer :
    public CConstantBufferBase
{
public:
	CFadeCBuffer();
	CFadeCBuffer(const CFadeCBuffer& Buffer);
	virtual ~CFadeCBuffer();

protected:
	FadeCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CFadeCBuffer* Clone();

public:
	void SetFadeStart(bool Start)
	{
		m_BufferData.FadeStart = Start ? 1 : 0;
	}

	void SetFadeRatio(float Ratio)
	{
		m_BufferData.Ratio = Ratio;
	}

	void SetFadeStartColor(const Vector3& Color)
	{
		m_BufferData.StartColor = Color;
	}

	void SetFadeEndColor(const Vector3& Color)
	{
		m_BufferData.EndColor = Color;
	}
};

