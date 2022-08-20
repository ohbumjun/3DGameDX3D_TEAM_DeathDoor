#pragma once

#include "ConstantBufferBase.h"

class CDownScaleCBuffer :
    public CConstantBufferBase
{
public:
	CDownScaleCBuffer();
	CDownScaleCBuffer(const CDownScaleCBuffer& Buffer);
	virtual ~CDownScaleCBuffer();

protected:
	DownScaleCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CDownScaleCBuffer* Clone();

public:
	void SetResolution(const Resolution& RS)
	{
		m_BufferData.RS = RS;
	}

	void SetDomain(const unsigned int Domain)
	{
		m_BufferData.Domain = Domain;
	}

	void SetGroupSize(const unsigned int GroupSize)
	{
		m_BufferData.GroupSize = GroupSize;
	}

	void SetAdaptation(float Adaptation)
	{
		m_BufferData.Adaptation = Adaptation;
	}

	void SetBloomThreshold(float Threshold)
	{
		m_BufferData.BloomThreshold = Threshold;
	}

	unsigned int GetGroupSize() const
	{
		return m_BufferData.GroupSize;
	}

	float GetAdaptation() const
	{
		return m_BufferData.Adaptation;
	}

	float GetBloomThreshold() const
	{
		return m_BufferData.BloomThreshold;
	}
};

