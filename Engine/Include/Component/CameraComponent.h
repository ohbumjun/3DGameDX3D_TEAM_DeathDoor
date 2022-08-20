#pragma once

#include "SceneComponent.h"

// 카메라 무브 콜백이 언제 불릴 것인지
enum class CamMoveCallBackCallType
{
	START_MOVE,						// 해당 포인트로 이동 시작하는 타이밍
	REACHED_POINT,					// 해당 포인트에 도착했을 타이밍
	Max
};

struct CamMoveData
{
	Vector3 DestPoint;
	float	NextPointReachTime;
	float	StayTime;
	std::function<void()> CallBack[(int)CamMoveCallBackCallType::Max];
};

class CCameraComponent :
	public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CCameraComponent();
	CCameraComponent(const CCameraComponent& com);
	virtual ~CCameraComponent();

protected:
	Camera_Type m_CameraType;
	Matrix      m_matView;
	Matrix      m_matProj;
	Matrix		m_matKeepProj;
	Matrix      m_matShadowView;
	Matrix      m_matShadowProj;
	float		m_ViewAngle;
	float		m_Distance;
	Resolution	m_RS;
	Vector2		m_Ratio;
	class CFrustum* m_Frustum;

	Vector3		m_PrevShakeAmount;
	bool		m_Shake;
	float		m_ShakeTimer;
	float		m_ShakeMaxTime;
	float		m_ShakeAmount;
	float		m_ShakeDecreaseTick;

	bool		m_Init;
	Vector3		m_OriginOffset;
	Vector3		m_Offet;

	bool		m_LinearMove;
	bool		m_MoveFreeze;
	bool		m_ReverseMove;
	float		m_MoveAccTime;
	float		m_MoveTime;
	Vector3		m_MoveStartPos;
	Vector3		m_MoveDestPos;
	std::function<void()> m_MoveEndCallBack;

	// CutScene Data
	bool		m_StartCutSceneMove;
	Vector3		m_CurCutSceneMoveDir;
	float		m_CurCutSceneMoveTick;
	bool		m_CutSceneStayStart;
	float		m_CutSceneStayTimer;
	CamMoveData* m_CurCutSceneMoveData;
	std::list<CamMoveData*>	m_CutSceneMoveDataList;
	std::function<void()>	m_CutSceneMoveEndCallBack;

#ifdef _DEBUG
	bool m_TestMove;
	Vector3 m_TestOriginPos;
	std::list<CamMoveData*>	m_TestMoveDataList;
#endif // _DEBUG

public:
	void Shake(float Time, float Amount);
	void StartMove(const Vector3& StartPos, const Vector3& EndPos, 
		float MoveTime, bool Reverse = false, bool MoveFreeze = false);

public:
	Resolution GetResolution()	const
	{
		return m_RS;
	}

	Camera_Type GetCameraType()	const
	{
		return m_CameraType;
	}

	Matrix GetViewMatrix()	const
	{
		return m_matView;
	}

	Matrix GetProjMatrix()	const
	{
		return m_matProj;
	}

	Matrix GetShadowViewMatrix()	const
	{
		return m_matShadowView;
	}

	Matrix GetShadowProjMatrix()	const
	{
		return m_matShadowProj;
	}

	Vector2 GetLeftBottom()	const
	{
		Vector2	LB;

		LB.x = GetWorldPos().x;// - m_Ratio.x * m_RS.Width;
		LB.y = GetWorldPos().y;// - m_Ratio.y * m_RS.Height;

		return LB;
	}

	const std::list<CamMoveData*>& GetMoveData() const
	{
		return m_CutSceneMoveDataList;
	}

	CamMoveData* FindMoveData(int Index);

	int GetMoveDataCount() const
	{
		return (int)m_CutSceneMoveDataList.size();
	}

	float GetDistance() const
	{
		return m_Distance;
	}

	float GetViewAngle() const
	{
		return m_ViewAngle;
	}

	bool IsMoving() const
	{
		return m_LinearMove;
	}

	const Vector3& GetMoveStartPos() const
	{
		return m_MoveStartPos;
	}
	
	const Vector3& GetMoveDestPos() const
	{
		return m_MoveDestPos;
	}

public:
	void SetStartCutSceneMoveEnable(bool Enable)
	{
		m_StartCutSceneMove = Enable;
	}
	void SetViewAngle(float Angle)
	{
		m_ViewAngle = Angle;

		if (m_CameraType == Camera_Type::Camera3D)
			CreateProjectionMatrix();
	}

	void SetDistance(float Distance)
	{
		m_Distance = Distance;

		CreateProjectionMatrix();
	}

	void SetCameraType(Camera_Type Type)
	{
		m_CameraType = Type;

		CreateProjectionMatrix();
	}

	// 2D 전용
	void OnViewportCenter()
	{
		if (m_CameraType == Camera_Type::Camera2D)
		{
			float	z = GetRelativePos().z;
			SetRelativePos(m_RS.Width / -2.f, m_RS.Height / -2.f, z);

			m_Ratio.x = 0.5f;
			m_Ratio.y = 0.5f;
		}
	}

	// 2D 전용
	void SetViewportRatio(float x, float y)
	{
		if (m_CameraType == Camera_Type::Camera2D)
		{
			float	z = GetRelativePos().z;
			SetRelativePos(m_RS.Width * -x, m_RS.Height * -y, z);

			m_Ratio.x = x;
			m_Ratio.y = y;
		}
	}

	void SetMoveReverse(bool Reverse)
	{
		m_ReverseMove = Reverse;
	}

	void SetMoveFreeze(bool Freeze)
	{
		m_MoveFreeze = Freeze;
	}

	void StopMove()
	{
		m_LinearMove = false;
	}

public:
	bool FrustumInPoint(const Vector3& Point);
	bool FrustumInSphere(const SphereInfo& Sphere);

public :
	// 원하는 Width, Height에 맞춰서 Projection Matrix 를 만들어내는 함수
	void CreateCustomResolutionProjMatrix(float Width, float Height);

public:
	void ComputeShadowView();

public:
	void StartCutSceneMove();
	void CutSceneMove(float DeltaTime);
	void ClearCutSceneMoveData();
	CamMoveData* AddCutSceneMoveData(const Vector3& Point, float NextPointReachTime, float StayTime = 0.f);
	bool DeleteCutSceneMoveData(int Index);
	bool DeleteCutSceneMoveData(CamMoveData* Data);
	void ChangeCutSceneMoveData(int Index, const Vector3& Point, float NextPointReachTime, float StayTime = 0.f);
	void ChangeCutSceneMoveDestPoint(int Index, const Vector3& Point);
	void ChangeCutSceneMoveDestReachTime(int Index, float Time);
	void ChangeCutSceneMoveStayTime(int Index, float Time);

#ifdef _DEBUG
	void StartTestMove();
	void TestMove(float DeltaTime);
	void UpdateCurTestMoveData();
#endif // _DEBUG

private:
	void CreateProjectionMatrix();
	bool IsValidMoveDataIndex(int Index);
	void UpdateCurMoveData();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CCameraComponent* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);

public:
	template<typename T>
	void AddCutSceneMoveEndCallBack(T* Obj, void(T::* Func)())
	{
		m_CutSceneMoveEndCallBack = std::bind(Func, Obj);
	}

	template <typename T>
	void AddCutSceneMoveCallBack(CamMoveData* Data, CamMoveCallBackCallType CallBackType, T* Obj, void(T::* Func)())
	{
		if (!Data)
		{
			return;
		}

		std::function<void()> CallBack = std::bind(Func, Obj);
		Data->CallBack[(int)CallBackType] = CallBack;
	}

	template <typename T>
	void AddCutSceneMoveCallBack(int Index, CamMoveCallBackCallType CallBackType, T* Obj, void(T::* Func)())
	{
		if (!IsValidMoveDataIndex(Index))
		{
			return;
		}

		CamMoveData* FoundData = FindMoveData(Index);

		if (!FoundData)
		{
			return;
		}

		std::function<void()> CallBack = std::bind(Func, Obj);
		FoundData->CallBack[(int)CallBackType] = CallBack;
	}

	template <typename T>
	void SetMoveEndCallBack(T* Obj, void(T::* Func)())
	{
		if (m_MoveEndCallBack)
		{
			m_MoveEndCallBack = nullptr;
		}

		m_MoveEndCallBack = std::bind(Func, Obj);
	}
};

