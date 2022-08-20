#pragma once

#include "ConstantBufferBase.h"

class CGlobalLightCBuffer :
    public CConstantBufferBase
{
public:
	CGlobalLightCBuffer();
	CGlobalLightCBuffer(const CGlobalLightCBuffer& Buffer);
	virtual ~CGlobalLightCBuffer();

protected:
	GlobalLightCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CGlobalLightCBuffer* Clone();

public:
	void SetAmbientIntensity(float AmbInt)
	{
		m_BufferData.AmbientIntensity = AmbInt;
	}

	float GetAmbiendIntensity() const
	{
		return m_BufferData.AmbientIntensity;
	}
};

