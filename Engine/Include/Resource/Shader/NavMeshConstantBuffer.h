#pragma once
#include "ConstantBufferBase.h"
class CNavMeshConstantBuffer :
    public CConstantBufferBase
{
public:
	CNavMeshConstantBuffer();
	CNavMeshConstantBuffer(const CNavMeshConstantBuffer& Buffer);
	virtual ~CNavMeshConstantBuffer();

protected:
	NavMeshCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CNavMeshConstantBuffer* Clone();

public:
	void SetNavMeshColor(const Vector4& Color)
	{
		m_BufferData.NavMeshColor = Color;
	}

	void SetWVP(const Matrix& matWVP)
	{
		m_BufferData.NavMeshWVP = matWVP;
	}
};

