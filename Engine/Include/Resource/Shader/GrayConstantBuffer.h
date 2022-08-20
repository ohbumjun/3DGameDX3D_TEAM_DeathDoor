#pragma once

#include "ConstantBufferBase.h"

class CGrayConstantBuffer :
    public CConstantBufferBase
{
public:
	CGrayConstantBuffer();
	CGrayConstantBuffer(const CGrayConstantBuffer& Buffer);
	virtual ~CGrayConstantBuffer();

protected:
	GrayCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CGrayConstantBuffer* Clone();

public:
	void SetGrayScale(float Scale)
	{
		if (Scale >= 1.f)
		{
			Scale = 1.f;
		}
		else if (Scale < 0.f)
		{
			Scale = 0.f;
		}
		m_BufferData.GrayScale = Scale;
	}

	float GetGrayScale()
	{
		return m_BufferData.GrayScale;
	}
};

