#pragma once

#include "../../Ref.h"
#include "../Material/Material.h"
#include "../Shader/ParticleUpdateShader.h"
#include "../Shader/ParticleConstantBuffer.h"

struct ParticleSaveLoadStruct
{
	bool MaterialEnable;
	char MaterialName[MAX_PATH];
	bool UpdateShaderEnable;
	char UpdateShaderName[MAX_PATH];
	ParticleInfo Info;
	ParticleInfoShared InfoShared;
	bool Is2D;
	int SpawnCountMax;
	float SpawnTimeMax;
};

class CParticle :
	public CRef
{
	friend class CParticleManager;
	friend class  CEffectEditor;
private:
	CParticle();
	CParticle(const CParticle& particle);
	~CParticle();

private:
	class CScene* m_Scene;
	std::vector<class CStructuredBuffer*>   m_vecStructuredBuffer;
	CSharedPtr<CMaterial>                   m_Material;
	CSharedPtr<CParticleUpdateShader>		m_UpdateShader;
	ParticleInfo							m_Info;
	ParticleInfoShared						m_InfoShare;
	CParticleConstantBuffer*				m_CBuffer;
	float									m_SpawnTimeMax;
	bool									m_2D;
	int										m_SpawnCountMax;
	ParticleSaveLoadStruct       m_SaveLoadStruct;
private :
	// 정규 분포 형태의 Y 값들을 지닌 구조화 버퍼
	// 원소 개수 (X 축 원소 개수) : SpawnCount 만큼
	// Y값 범위 : 0 ~ 1 (정규분포)
	// class CStructuredBuffer* m_NormalDistributionBuffer;
	// std::vector<float>	m_vecNormalDistVal;
public:
	CMaterial* CloneMaterial()	const
	{
		return m_Material->Clone();
	}
	CParticleUpdateShader* GetUpdateShader()	const
	{
		return m_UpdateShader;
	}

	CParticleUpdateShader* CloneUpdateShader()	const
	{
		return m_UpdateShader->Clone();
	}

	CParticleConstantBuffer* CloneConstantBuffer()	const
	{
		return m_CBuffer->Clone();
	}

	float GetSpawnTimeMax()	const
	{
		return m_SpawnTimeMax;
	}

public:
	bool Init();
	virtual bool Save(FILE* File) override;
	virtual bool Load(FILE* File) override;
public:
	void AddStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count,
		int Register, bool Dynamic = false,
		int StructuredBufferShaderType = (int)Buffer_Shader_Type::Compute);

	bool ResizeBuffer(const std::string& Name, unsigned int Size, unsigned int Count,
		int Register, bool Dynamic = false,
		int StructuredBufferShaderType = (int)Buffer_Shader_Type::Compute);
	void CloneStructuredBuffer(std::vector<CStructuredBuffer*>& vecBuffer);
private :
	// m_NormalDistributionBuffer 에 정규 분포 계산 값들을 적용하는 함수
	// void CloneNormalDistStructuredBuffer(CStructuredBuffer*& NormalDistBuffer);
	// void GenerateNormalDistribution();
	/*
	void CreateNormalDistStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count,
	int Register, bool Dynamic = false,
	int StructuredBufferShaderType = (int)Buffer_Shader_Type::Compute);
	bool ResizeNormalDistStructuredBuffer(const std::string& Name, unsigned int Size, unsigned int Count,
	int Register, bool Dynamic = false,
	int StructuredBufferShaderType = (int)Buffer_Shader_Type::Compute);
	*/
public :
	bool SaveFile(const char* FullPath);
	bool LoadFile(const char* FullPath);
public:
	CParticleConstantBuffer* GetCBuffer()	const
	{
		return m_CBuffer;
	}
	CMaterial* GetMaterial() const
	{
		return m_Material;
	}

	// int GetApplyRandom() const
	// {
	// 	return m_CBuffer->GetApplyRandom();
	// }

	int GetSpawnCountMax()	const
	{
		return m_SpawnCountMax;
	}
	const Vector3& GetStartMin()	const
	{
		return m_CBuffer->GetStartMin();
	}

	const Vector3& GetStartMax()	const
	{
		return m_CBuffer->GetStartMax();
	}

	int GetSpawnCount()	const
	{
		return m_CBuffer->GetSpawnCount();
	}
	bool GetSpawnEnable()
	{
		return m_CBuffer->GetSpawnEnable();
	}
	const Vector3& GetStartMin()
	{
		return m_CBuffer->GetStartMin();
	}

	const Vector3& GetStartMax()
	{
		return m_CBuffer->GetStartMax();
	}

	unsigned int GetSpawnCountMax()
	{
		return m_CBuffer->GetSpawnCountMax();
	}

	const Vector3& GetScaleMin()
	{
		return m_CBuffer->GetScaleMin();
	}

	const Vector3& GetScaleMax()
	{
		return m_CBuffer->GetScaleMax();
	}

	float GetLifeTimeMin()
	{
		return m_CBuffer->GetLifeTimeMin();
	}

	float GetLifeTimeMax()
	{
		return m_CBuffer->GetLifeTimeMax();
	}

	const Vector4& GetColorMin()
	{
		return m_CBuffer->GetColorMin();
	}

	const Vector4& GetColorMax()
	{
		return m_CBuffer->GetColorMax();
	}

	int GetSpeedChangeMethod() const
	{
		return m_CBuffer->GetSpeedChangeMethod();
	}

	float GetSpeedStart()
	{
		return m_CBuffer->GetSpeedStart();
	}

	float GetSpeedEnd()
	{
		return m_CBuffer->GetSpeedEnd();
	}

	bool GetMove()
	{
		return m_CBuffer->GetMove();
	}

	bool GetGravity()
	{
		return m_CBuffer->GetGravity();
	}

	const Vector3& GetMoveDir()
	{
		return m_CBuffer->GetMoveDir();
	}

	// bool Is2D()
	// {
	// 	return m_CBuffer->Is2D();
	// }

	int GetSpecialMoveDirType()
	{
		return m_CBuffer->GetSpecialMoveDirType();
	}

	const Vector3& GetMoveAngle()
	{
		return m_CBuffer->GetMoveAngle();
	}

	const Vector3& GetRotationAngle()
	{
		return m_CBuffer->GetRotationAngle();
	}	

	// Min, Max
	const Vector3& GetMinSeperateRotAngle()
	{
		return m_CBuffer->GetMinSeperateRotAngle();
	}
	const Vector3& GetMaxSeperateRotAngle()
	{
		return m_CBuffer->GetMaxSeperateRotAngle();
	}

	// 생성 반지름
	float GetGenerateRadius() const
	{
		return m_CBuffer->GetGenerateRadius();
	}
	
	// 가운데에서 멀어질수록 LifeTime 감소 
	bool IsLifeTimeLinearFromCenter()
	{
		return m_CBuffer->IsLifeTimeLinearFromCenter();
	}
	// Bounce
	int IsBounceEnable() const
	{
		return m_CBuffer->IsBounceEnable();
	}
	float GetBounceResistance() const
	{
		return m_CBuffer->GetParticleBounceResist();
	}

	int GetParticleShapeType() const
	{
		return m_CBuffer->GetParticleShapeType();
	}

	int IsLoopGenerateRing() const
	{
		return m_CBuffer->IsLoopGenerateRing();
	}

	// Alpha
	float GetStartAlpha() const
	{
		return m_CBuffer->GetStartAlpha();
	}
	float GetEndAlpha() const
	{
		return m_CBuffer->GetEndAlpha();
	}
	// Alpha Linear
	int IsAlphaLinearFromCenter() const
	{
		return m_CBuffer->IsAlphaLinearFromCenter();
	}
	// UV Move
	int GetUVMoveEnable() const
	{
		return m_CBuffer->GetUVMoveEnable();
	}
	int GetUVRowN() const
	{
		return m_CBuffer->GetUVRowN();
	}
	int GetUVColN() const
	{
		return m_CBuffer->GetUVColN();
	}
	// Linear Rotate
// Linaer Iterate 할때의 최초 Rot Angle
	int IsSeperateLinearRotate()
	{
		return m_CBuffer->IsSeperateLinearRotate();
	}
	// 되살리는 효과
	int IsDisableNewAlive() const
	{
		return m_CBuffer->IsDisableNewAlive();
	}
	int IsLinearUVClippingEnabled() const
	{
		return m_CBuffer->IsLinearUVClippingEnabled();
	}
	int IsNoiseTextureSamplingApplied()
	{
		return m_CBuffer->IsNoiseTextureSamplingApplied();
	}
	float GetNoiseTextureApplyRatio() const
	{
		return m_CBuffer->GetNoiseTextureApplyRatio();
	}
	// Linear Emissive 
	int IsLinearEmissiveChangeEnable() const
	{
		return m_CBuffer->IsLinearEmissiveChangeEnable();
	}

	const Vector3& GetStartEmissiveColor() const
	{
		return m_CBuffer->GetStartEmissiveColor();
	}

	const Vector3& GetEndEmissiveColor() const
	{
		return  m_CBuffer->GetEndEmissiveColor();
	}
	int IsFollowRealTimeParticleComponentPos() const
	{
		return m_CBuffer->IsFollowRealTimeParticleComponentPos();
	}
	bool IsLinearUVClippingDirReversed() const
	{
		return m_CBuffer->IsLinearUVClippingDirReversed();
	}
public:
	// Follow Particle Component Pos
	void SetFollowRealTimeParticleComponentPos(bool Enable)
	{
		m_CBuffer->SetFollowRealTimeParticleComponentPos(Enable);
	}
	// Noise Texture
	void SetApplyNoiseTextureSamplingEnable(bool Enable)
	{
		m_CBuffer->SetApplyNoiseTextureSamplingEnable(Enable);
	}
	void SetNoiseTextureApplyRatio(float Ratio)
	{
		m_CBuffer->SetNoiseTextureApplyRatio(Ratio);
	}
	// UV Clipping
	void SetLinearUVClippingDirReverseEnable(bool Enable)
	{
		m_CBuffer->SetLinearUVClippingDirReverseEnable(Enable);
	}

	void SetLinearUVClippingEnable(bool Enable)
	{
		m_CBuffer->SetLinearUVClippingEnable(Enable);
	}
	// Restart 기능 (구조화 버퍼 내의 특정 값을 초기화 해주는 것)
	// void SetResetParticleSharedInfoSumSpawnCnt(bool Enable)
	// {
	// 	m_CBuffer->SetResetParticleSharedInfoSumSpawnCnt(Enable);
	// }
	void SetDisableNewAlive(bool Enable)
	{
		m_CBuffer->SetDisableNewAlive(Enable);
	}

	// Linaer Rotate
	void SetRotToDir(bool Enable)
	{
		m_CBuffer->SetRotToDir(Enable);
	}

	void SetLinearEmissiveChangeEnable(bool Enable)
	{
		m_CBuffer->SetLinearEmissiveChangeEnable(Enable);
	}

	// Linear Emissive
	void SetStartEmissiveColor(const Vector3& Color)
	{
		m_CBuffer->SetStartEmissiveColor(Color);
	}

	// End Emissive Color
	void SetEndEmissiveColor(const Vector3& Color)
	{
		m_CBuffer->SetEndEmissiveColor(Color);
	}

	// Alpha Linear
	void SetAlphaLinearFromCenter(bool Enable)
	{
		m_CBuffer->SetAlphaLinearFromCenter(Enable);
	}

	// Component 의 Relative Scale 반영학
	// void SetCommonWorldScale(const Vector3& Scale)
	// {
	// 	m_CBuffer->SetCommonWorldScale(Scale);
	// }
	// Set  Particle Type
	void SetParticleShapeType(ParitcleShapeType Type)
	{
		m_CBuffer->SetParticleShapeType((int)Type);
	}
	void SetParticleShapeType(int Type)
	{
		m_CBuffer->SetParticleShapeType(Type);
	}

	// Seperate Rot Angle
	void SetMinSeperateRotAngle(const Vector3& Angle)
	{
		m_CBuffer->SetMinSeperateRotAngle(Angle);
	}
	void SetMaxSeperateRotAngle(const Vector3& Angle)
	{
		m_CBuffer->SetMaxSeperateRotAngle(Angle);
	}
	// UV Move
	void SetUVMoveEnable(bool Enable)
	{
		m_CBuffer->SetUVMoveEnable(Enable);
	}
	void SetUVRowN(int Row)
	{
		m_CBuffer->SetUVRowN(Row);
	}
	void SetUVColN(int Col)
	{
		m_CBuffer->SetUVColN(Col);
	}
	// MoveUp Corn
	void SetLifeTimeLinearFromCenter(bool Enable)
	{
		m_CBuffer->SetLifeTimeLinearFromCenter(Enable);
	}
	// Alpha
	void SetStartAlpha(float Alpha) 
	{
		m_CBuffer->SetStartAlpha(Alpha);
	}
	void SetEndAlpha(float Alpha)
	{
		m_CBuffer->SetEndAlpha(Alpha);
	}
	// 생성 반지름
	void SetGenerateRadius(float Radius)
	{
		m_CBuffer->SetGenerateRadius(Radius);
	}
	// Ring
	void SetLoopGenerateRing(bool Enable)
	{
		m_CBuffer->SetLoopGenerateRing(Enable);
	}
	// Bounce
	void SetBounceEnable(bool Enable)
	{
		m_CBuffer->SetBounceEnable(Enable);
	}
	void SetBounceResistance(float Resist)
	{
		m_CBuffer->SetBounceResist(Resist);
	}
	void SetMaterial(CMaterial* Material)
	{
		m_Material = Material;
	}
	void SetSpawnTimeMax(float SpawnTime)
	{
		m_SpawnTimeMax = SpawnTime;
	}

	void SetSpawnEnable(unsigned int Count)
	{
		m_CBuffer->SetSpawnEnable(Count);
	}

	void SetStartMin(const Vector3& StartMin)
	{
		m_CBuffer->SetStartMin(StartMin);
	}

	void SetStartMax(const Vector3& StartMax)
	{
		m_CBuffer->SetStartMax(StartMax);
	}

	void SetRotationAngle(const Vector3& Angle)
	{
		m_CBuffer->SetRotationAngle(Angle);
	}

	void SetSpawnCountMax(unsigned int Count);

	void SetScaleMin(const Vector3& ScaleMin)
	{
		m_CBuffer->SetScaleMin(ScaleMin);
	}

	void SetScaleMax(const Vector3& ScaleMax)
	{
		m_CBuffer->SetScaleMax(ScaleMax);
	}

	void SetLifeTimeMin(float Min)
	{
		m_CBuffer->SetLifeTimeMin(Min);
	}

	void SetLifeTimeMax(float Max)
	{
		m_CBuffer->SetLifeTimeMax(Max);
	}

	void SetColorMin(const Vector4& ColorMin)
	{
		m_CBuffer->SetColorMin(ColorMin);
	}

	void SetColorMin(float r, float g, float b, float a)
	{
		m_CBuffer->SetColorMin(r, g, b, a);
	}

	void SetColorMax(const Vector4& ColorMax)
	{
		m_CBuffer->SetColorMax(ColorMax);
	}

	void SetColorMax(float r, float g, float b, float a)
	{
		m_CBuffer->SetColorMax(r, g, b, a);
	}

	void SetSpeedChangeMethod(ParticleSpeedChangeMethod Method)
	{
		m_CBuffer->SetSpeedChangeMethod(Method);
	}

	void SetSpeedChangeMethod(int Method)
	{
		m_CBuffer->SetSpeedChangeMethod(Method);
	}

	void SetSpeedStart(float SpeedMin)
	{
		m_CBuffer->SetSpeedStart(SpeedMin);
	}

	void SetSpeedEnd(float SpeedMax)
	{
		m_CBuffer->SetSpeedEnd(SpeedMax);
	}

	void SetMove(bool Move)
	{
		m_CBuffer->SetMove(Move);
	}

	void SetSpecialMoveDirType(int Type)
	{
		m_CBuffer->SetSpecialMoveDirType(Type);
	}

	void SetSpecialMoveDirType(ParticleSpecialMoveDir Type)
	{
		m_CBuffer->SetSpecialMoveDirType(Type);
	}

	void SetGravity(bool Gravity)
	{
		m_CBuffer->SetGravity(Gravity);
	}

	void SetMoveDir(const Vector3& MoveDir)
	{
		m_CBuffer->SetMoveDir(MoveDir);
	}

	// void SetApplyRandom(bool Enable)
	// {
	// 	m_CBuffer->SetApplyRandom(Enable);
	// }

	// void Set2D(bool Is2D)
	// {
	// 	m_2D = Is2D;
	// 	m_CBuffer->Set2D(Is2D);
	// }

	void SetMoveAngle(const Vector3& MoveAngle)
	{
		m_CBuffer->SetMoveAngle(MoveAngle);
	}
};
