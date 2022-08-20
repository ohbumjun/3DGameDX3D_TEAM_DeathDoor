#pragma once

#include "ConstantBufferBase.h"

class CFogCBuffer :
    public CConstantBufferBase
{
public:
	CFogCBuffer();
	CFogCBuffer(const CFogCBuffer& Buffer);
	virtual ~CFogCBuffer();

protected:
	FogCBuffer m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CFogCBuffer* Clone();

public:
	void SetFogColor(const Vector3& Color)
	{
		m_BufferData.Color = Color;
	}

	void SetFogType(Fog_Type Type)
	{
		m_BufferData.Type = (int)Type;
	}

	void SetFogStart(float Start)
	{
		m_BufferData.Start = Start;
	}

	void SetFogEnd(float End)
	{
		m_BufferData.End = End;
	}

	void SetFogDensity(float Density)
	{
		m_BufferData.Density = Density;
	}

public:
	const Vector3& GetFogColor() const
	{
		return m_BufferData.Color;
	}

	Fog_Type GetFogType() const
	{
		return (Fog_Type)m_BufferData.Type;
	}

	float GetFogStart() const
	{
		return m_BufferData.Start;
	}

	float GetFogEnd() const
	{
		return m_BufferData.End;
	}

	float GetFogDensity() const
	{
		return m_BufferData.Density;
	}
};

