#pragma once

#include "../GameInfo.h"

struct TransformCallBack
{
	void* Callee;
	std::function<void(const Vector3&, const Vector3&)> Func; // World, Relative 순서로 인자에 넣어줌
};

class CTransform
{
	friend class CSceneComponent;

private:
	CTransform();
	CTransform(const CTransform& transform);
	~CTransform();

private:
	class CScene* m_Scene;
	class CGameObject* m_Object;
	class CSceneComponent* m_Owner;

private:
	CTransform* m_Parent;
	std::vector<CTransform*>	m_vecChild;
	class CTransformConstantBuffer* m_CBuffer;
	Transform_State		m_State;
	class CSkeletonSocket* m_Socket;

	// Detroy시 콜백되는 함수
	std::list<std::function<void()>> m_OnDestroyCallBackList;

	// P, R, S 변경 시 콜백되는 함수 리스트
	std::list<TransformCallBack> m_ScaleChangeCallBackList;
	std::list<TransformCallBack> m_PosChangeCallBackList;
	std::list<TransformCallBack> m_RotChangeCallBackList;

private:
	bool	m_InheritScale;
	bool	m_InheritRotX;
	bool	m_InheritRotY;
	bool	m_InheritRotZ;
	bool	m_InheritParentRotationPosX;
	bool	m_InheritParentRotationPosY;
	bool	m_InheritParentRotationPosZ;
	bool	m_UpdateScale;
	bool	m_UpdateRot;
	bool	m_UpdateRotAxis; // 주어진 축 기준 회전으로 업데이트 여부
	bool	m_UpdatePos;

	bool	m_UpdateByMat;

	bool m_FixTransform;

public:
	void SetFixTransform(bool Fix)
	{
		m_FixTransform = Fix;
	}

	bool GetFixTransform()	const
	{
		return m_FixTransform;
	}

public:
	class CTransformConstantBuffer* GetTransformCBuffer()	const;

	void SetState(Transform_State State)
	{
		m_State = State;
	}

	const Transform_State& GetState()	const
	{
		return m_State;
	}

	void SetInstancingInfo(Instancing3DInfo* Info);
	void SetInstancingShadowInfo(Instancing3DInfo* Info);

	void SetUpdateByMat(bool UpdateByMat)
	{
		m_UpdateByMat = UpdateByMat;
	}

	void SetSocket(class CSkeletonSocket* Socket)
	{
		m_Socket = Socket;
	}

	void SetTransformState(Transform_State State)
	{
		m_State = State;
	}

	void SetInheritScale(bool Inherit)
	{
		m_InheritScale = Inherit;
	}

	void SetInheritRotX(bool Inherit)
	{
		m_InheritRotX = Inherit;
	}

	void SetInheritRotY(bool Inherit)
	{
		m_InheritRotY = Inherit;
	}

	void SetInheritRotZ(bool Inherit)
	{
		m_InheritRotZ = Inherit;
	}

	void SetInheritParentRotationPosX(bool Inherit)
	{
		m_InheritParentRotationPosX = Inherit;
	}

	void SetInheritParentRotationPosY(bool Inherit)
	{
		m_InheritParentRotationPosY = Inherit;
	}

	void SetInheritParentRotationPosZ(bool Inherit)
	{
		m_InheritParentRotationPosZ = Inherit;
	}

	void SetUpdateRotAxis(bool Update)
	{
		m_UpdateRotAxis = Update;
	}

	void InheritScale(bool Current);
	void InheritRotation(bool Current);
	void InheritParentRotationPos(bool Current);

	void InheritWorldScale(bool Current);
	void InheritWorldRotation(bool Current);
	void InheritParentRotationWorldPos(bool Current);

private:	// Relative
	Vector3	m_RelativeScale;
	Vector3	m_RelativeRot;
	Vector3	m_RelativePos;
	Vector3	m_RelativeAxis[AXIS_MAX];

public:
	Vector3 GetRelativeScale()	const
	{
		return m_RelativeScale;
	}

	Vector3 GetRelativeRot()	const
	{
		return m_RelativeRot;
	}

	Vector3 GetRelativePos()	const
	{
		return m_RelativePos;
	}

	Vector3 GetRelativeAxis(AXIS Axis)
	{
		return m_RelativeAxis[Axis];
	}

	bool IsInheritScale() const
	{
		return m_InheritScale;
	}

	bool IsInheritRotX() const
	{
		return m_InheritRotX;
	}

	bool IsInheritRotY() const
	{
		return m_InheritRotY;
	}

	bool IsInheritRotZ() const
	{
		return m_InheritRotZ;
	}

	bool IsInheritPosX() const
	{
		return m_InheritParentRotationPosX;
	}

	bool IsInheritPosY() const
	{
		return m_InheritParentRotationPosY;
	}

	bool IsInheritPosZ() const
	{
		return m_InheritParentRotationPosZ;
	}

public:
	void SetRelativeScale(const Vector3& Scale);
	void SetRelativeScale(float x, float y, float z);
	void SetRelativeRotation(const Vector3& Rot);
	void SetRelativeRotation(float x, float y, float z);
	void SetRelativeRotationX(float x);
	void SetRelativeRotationY(float y);
	void SetRelativeRotationZ(float z);
	void SetRelativePos(const Vector3& Pos);
	void SetRelativePos(float x, float y, float z);
	void AddRelativeScale(const Vector3& Scale);
	void AddRelativeScale(float x, float y, float z);
	void AddRelativeRotation(const Vector3& Rot);
	void AddRelativeRotation(float x, float y, float z);
	void AddRelativeRotationX(float x);
	void AddRelativeRotationY(float y);
	void AddRelativeRotationZ(float z);
	void AddRelativePos(const Vector3& Pos);
	void AddRelativePos(float x, float y, float z);

private:
	Vector3	m_WorldScale;
	Vector3	m_WorldRot;
	Vector3	m_WorldPos;
	Vector3	m_OriginWorldScale;
	Vector3	m_OriginWorldRot;
	Vector3	m_OriginWorldPos;
	Vector3	m_WorldAxis[AXIS_MAX];
	Vector3	m_Pivot;
	Vector3	m_MeshSize;

	Matrix	m_matScale;
	Matrix	m_matRot;
	Matrix	m_matPos;
	Matrix	m_matWorld;

public:
	Vector3 GetWorldScale()	const
	{
		return m_WorldScale;
	}

	Vector3 GetWorldRot()	const
	{
		return m_WorldRot;
	}

	Vector3 GetWorldPos()	const
	{
		return m_WorldPos;
	}

	Vector3 GetPivot()	const
	{
		return m_Pivot;
	}

	Vector3 GetMeshSize()	const
	{
		return m_MeshSize;
	}

	Vector3 GetWorldAxis(AXIS Axis)
	{
		return m_WorldAxis[Axis];
	}

	const Matrix& GetRotationMatrix()	const
	{
		return m_matRot;
	}

	const Matrix& GetWorldMatrix()	const
	{
		return m_matWorld;
	}

	void SetRotationMatrix(const Matrix& Mat)
	{
		m_matRot = Mat;
	}

public:
	void SetPivot(const Vector3& Pivot)
	{
		m_Pivot = Pivot;
	}

	void SetPivot(float x, float y, float z)
	{
		m_Pivot = Vector3(x, y, z);
	}

	void SetMeshSize(const Vector3& Size)
	{
		m_MeshSize = Size;
	}

	void SetMeshSize(float x, float y, float z)
	{
		m_MeshSize = Vector3(x, y, z);
	}

public:
	void SetTransformByWorldMatrix(const Matrix& matTRS);
	void SetWorldScale(const Vector3& Scale);
	void SetWorldScale(float x, float y, float z);
	void SetWorldRotation(const Vector3& Rot);
	void SetWorldRotation(float x, float y, float z);
	void SetWorldRotationX(float x);
	void SetWorldRotationY(float y);
	void SetWorldRotationZ(float z);
	void SetWorldPos(const Vector3& Pos);
	void SetWorldPos(float x, float y, float z);
	void SetRotationAxis(const Vector3& OriginDir, const Vector3& View);
	void AddWorldScale(const Vector3& Scale);
	void AddWorldScale(float x, float y, float z);
	void AddWorldRotation(const Vector3& Rot);
	void AddWorldRotation(float x, float y, float z);
	void AddWorldRotationX(float x);
	void AddWorldRotationY(float y);
	void AddWorldRotationZ(float z);
	void AddWorldPos(const Vector3& Pos);
	void AddWorldPos(float x, float y, float z);
	

	// 자신의 축 기준으로 이동
	void AddWorldPosByLocalAxis(AXIS Axis, float Amount);
	void AddWorldPosByLocalAxis(const Vector3& Pos);

public:
	void Start();
	void Init();
	void Update(float DeltaTime);
	void PostUpdate(float DeltaTime);
	void SetTransform();
	void SetTransformShadow();
	void ComputeWorld();
	void DecomposeWorld();
	CTransform* Clone();
	void Save(FILE* File);
	void Load(FILE* File);

	// Animation Editor Set Transform
public : 
	void SetAnimationTransform();
	void SetParticleEffectEditorTransform();

	// CallBack
private:
	void CallChangePosCallBack();
	void CallChangeRotCallBack();
	void CallChangeScaleCallBack();
	void OnDestroy();

public:
	void DeleteChangePosCallBack(void* Callee);
	void DeleteChangeScaleCallBack(void* Callee);
	void DeleteChangeRotCallBack(void* Callee);

public:
	void ForceUpdateMat();

public:
	template <typename T>
	void AddChangeScaleCallBack(T* Obj, void(T::* Func)(const Vector3&, const Vector3&))
	{
		TransformCallBack CallBack;
		CallBack.Callee = Obj;
		CallBack.Func = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
		m_ScaleChangeCallBackList.push_back(CallBack);
	}

	template <typename T>
	void AddChangeRotCallBack(T* Obj, void(T::* Func)(const Vector3&, const Vector3&))
	{
		TransformCallBack CallBack;
		CallBack.Callee = Obj;
		CallBack.Func = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
		m_RotChangeCallBackList.push_back(CallBack);
	}

	template <typename T>
	void AddChangePosCallBack(T* Obj, void(T::* Func)(const Vector3&, const Vector3&))
	{
		TransformCallBack CallBack;
		CallBack.Callee = Obj;
		CallBack.Func = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
		m_PosChangeCallBackList.push_back(CallBack);
	}

	template <typename T>
	void AddOnDestroyCallBack(T* Obj, void(T::* Func)())
	{
		std::function<void()> CallBack = std::bind(Func, Obj);
		m_OnDestroyCallBackList.push_back(CallBack);
	}
};


