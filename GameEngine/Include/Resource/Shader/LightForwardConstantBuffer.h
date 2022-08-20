#pragma once

#include "ConstantBufferBase.h"

class CLightForwardConstantBuffer :
    public CConstantBufferBase
{
public:
	CLightForwardConstantBuffer();
	CLightForwardConstantBuffer(const CLightForwardConstantBuffer& Buffer);
	virtual ~CLightForwardConstantBuffer();

protected:
	LightForwardCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CLightForwardConstantBuffer* Clone();

public:
	void SetLightCount(int LightCount)
	{
		m_BufferData.LightCount = LightCount;
	}
};

