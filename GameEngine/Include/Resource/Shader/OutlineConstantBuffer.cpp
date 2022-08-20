 //#include "OutlineConstantBuffer.h"
 //#include "ConstantBuffer.h"
 //
 //COutlineConstantBuffer::COutlineConstantBuffer()	:
 //	m_BufferData{}
 //{
 //	m_BufferData.DepthMultiplier = 1.f;
 //	m_BufferData.DepthBias = 1.f;
 //	m_BufferData.NormalMultiplier = 1.f;
 //	m_BufferData.NormalBias = 10.f;
 //}
 //
 //COutlineConstantBuffer::COutlineConstantBuffer(const COutlineConstantBuffer& Buffer)	:
 //	CConstantBufferBase(Buffer)
 //{
 //	m_BufferData = Buffer.m_BufferData;
 //}
 //
 //COutlineConstantBuffer::~COutlineConstantBuffer()
 //{
 //}
 //
 //bool COutlineConstantBuffer::Init()
 //{
 //	SetConstantBuffer("OutlineConstantBuffer");
 //
 //	return true;
 //}
 //
 //void COutlineConstantBuffer::UpdateCBuffer()
 //{
 //	m_Buffer->UpdateBuffer(&m_BufferData);
 //}
 //
 //COutlineConstantBuffer* COutlineConstantBuffer::Clone()
 //{
 //	return new COutlineConstantBuffer(*this);
 //}
