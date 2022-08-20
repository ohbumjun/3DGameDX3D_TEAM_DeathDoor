#pragma once

#include "ConstantBufferBase.h"

class CToonCBuffer :
    public CConstantBufferBase
{
public:
	CToonCBuffer();
	CToonCBuffer(const CToonCBuffer& Buffer);
	virtual ~CToonCBuffer();

protected:
	ToonCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CToonCBuffer* Clone();

public:
	void SetType(ToonShaderType Type)
	{
		m_BufferData.Type = (int)Type;
	}

public:
	ToonShaderType GetType() const
	{
		return (ToonShaderType)m_BufferData.Type;
	}
};

