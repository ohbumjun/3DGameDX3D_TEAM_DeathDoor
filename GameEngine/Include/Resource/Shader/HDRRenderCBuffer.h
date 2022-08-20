#pragma once

#include "ConstantBufferBase.h"

class CHDRRenderCBuffer :
    public CConstantBufferBase
{
public:
	CHDRRenderCBuffer();
	CHDRRenderCBuffer(const CHDRRenderCBuffer& Buffer);
	virtual ~CHDRRenderCBuffer();

protected:
	HDRRenderCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CHDRRenderCBuffer* Clone();

public:
	void SetMiddleGray(float MiddleGray)
	{
		m_BufferData.MiddleGray = MiddleGray;
	}

	void SetLumWhite(float White)
	{
		m_BufferData.LumWhite = White;
	}

	void SetBloomScale(float Scale)
	{
		m_BufferData.BloomScale = Scale;
	}

	void SetDOFMin(float Min)
	{
		m_BufferData.DOFMin = Min;
	}

	void SetDOFMax(float Max)
	{
		m_BufferData.DOFMax = Max;
	}

	float GetMiddleGray() const
	{
		return m_BufferData.MiddleGray;
	}

	float GetLumWhite() const
	{
		return m_BufferData.LumWhite;
	}

	float GetBloomScale() const
	{
		return m_BufferData.BloomScale;
	}

	float GetDOFMin() const
	{
		return m_BufferData.DOFMin;
	}

	float GetDOFMax() const
	{
		return m_BufferData.DOFMax;
	}
};

