#pragma once

#include "../Component/SceneComponent.h"
#include "../Component/ObjectComponent.h"
#include "../Component/NavAgent.h"
#include "../Component/LandScape.h"
//#include "../Scene/Navigation3DManager.h"

class CGameObject :
	public CRef
{
	friend class CScene;
	friend class CSceneManager;
	friend class CObjectPoolManager;
	friend class CObjectPool;

protected:
	CGameObject();
	CGameObject(const CGameObject& obj);
	virtual ~CGameObject();

protected:
	class CScene* m_Scene;
	Object_Type m_ObjectType;
	bool		m_IsEnemy;
	bool		m_ExcludeSceneSave;
	bool		m_NoDestroyFromSceneChange;
	Vector3		m_MoveDir;
	bool		m_InPool;
	bool		m_StartCalled;

public:
	void SetMoveDir(const Vector3& Dir)
	{
		m_MoveDir = Dir;
	}

	const Vector3& GetMoveDir()	const
	{
		return m_MoveDir;
	}

	bool IsEnemy()	const
	{
		return m_IsEnemy;
	}

	virtual void Enable(bool Enable) override;

	void SetEnemy(bool Enemy)
	{
		m_IsEnemy = Enemy;
	}

	Object_Type GetObjectType()	const
	{
		return m_ObjectType;
	}

	void SetObjectType(Object_Type Type)
	{
		m_ObjectType = Type;
	}

	class CScene* GetScene()    const
	{
		return m_Scene;
	}

	void ExcludeFromSceneSave()
	{
		m_ExcludeSceneSave = true;
	}

	bool IsExcludeFromSceneSave() const
	{
		return m_ExcludeSceneSave;
	}

	void SetNoDestroyOnSceneChange(bool NoDestroy)
	{
		m_NoDestroyFromSceneChange = NoDestroy;
	}

	bool IsNoDestroyFromSceneChange() const
	{
		return m_NoDestroyFromSceneChange;
	}

	void SetInPool(bool InPool)
	{
		m_InPool = InPool;
	}

	bool IsInPool()	const
	{
		return m_InPool;
	}

	bool IsStartCalled() const
	{
		return m_StartCalled;
	}

public:
	void SetState(Transform_State State)
	{
		m_RootComponent->SetState(State);
	}

	const Transform_State& GetState()	const
	{
		return m_RootComponent->GetState();
	}

public:
	void SetScene(class CScene* Scene);
	virtual void Destroy();

protected:
	CSharedPtr<CSceneComponent> m_RootComponent;
	std::list<CSceneComponent*>	m_SceneComponentList;
	std::vector<CSharedPtr<CObjectComponent>>   m_vecObjectComponent;

	CGameObject* m_Parent;
	std::vector<CSharedPtr<CGameObject>>   m_vecChildObject;
	float		m_LifeSpan;
	class CNavAgent* m_NavAgent;
	Vector3 m_PrevFramePos;

	std::list<CGameObject*> m_AttackTargetList;	// 광역스킬처럼 여러 오브젝트가 맞는 공격을 했을때 맞은 오브젝트들

public:
	float GetLifeSpan() const
	{
		return m_LifeSpan;
	}
	const Vector3& GetPrevFramePos()	const
	{
		return m_PrevFramePos;
	}

	void AddAttackTarget(CGameObject* Target)
	{
		m_AttackTargetList.push_back(Target);
	}

	const std::list<CGameObject*>& GetAttackTargets()	const
	{
		return m_AttackTargetList;
	}

	void ClearAttackTarget()
	{
		m_AttackTargetList.clear();
	}

	void EraseAttackTarget(CGameObject* Target)
	{

		auto iter = m_AttackTargetList.begin();
		auto iterEnd = m_AttackTargetList.end();

		for ( ; iter != iterEnd; ++iter)
		{
			if (*iter == Target)
			{
				iter = m_AttackTargetList.erase(iter);
				return;
			}
		}
	}

	size_t GetAttackTargetCount()	const
	{
		return m_AttackTargetList.size();
	}

public:
	void AddChildObject(CGameObject* Obj, const std::string& SocketName = "");
	void DeleteObj();
	bool DeleteChildObj(const std::string& Name);
	bool DeleteChildObj(class CGameObject* Child);
	// 실제로 지우진 않고, 나는 부모를 nullptr로 만들고, 부모에게 내 자신을 자식 목록에서 지우게 함
	void ClearParent();
	CGameObject* GetParentObject()	const
	{
		return m_Parent;
	}

	CGameObject* FindChildObject(const std::string& Name)
	{
		size_t Count = m_vecChildObject.size();

		for (size_t i = 0; i < Count; ++i)
		{
			if (m_vecChildObject[i]->GetName() == Name)
				return m_vecChildObject[i];

		}

		return nullptr;
	}

	template <typename T>
	CGameObject* FindChildObjectByType()
	{
		size_t Count = m_vecChildObject.size();

		for (size_t i = 0; i < Count; ++i)
		{
			if (m_vecChildObject[i]->GetTypeID() == typeid(T).hash_code())
				return m_vecChildObject[i];

		}

		return nullptr;
	}

public:
	bool CheckSceneComponent(CSceneComponent* Com)
	{
		auto iter = m_SceneComponentList.begin();
		auto iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter) == Com)
				return true;
		}

		return false;
	}

	const std::list<CSceneComponent*>& GetSceneComponents()	const
	{
		return m_SceneComponentList;
	}

	void SetRootComponent(CSceneComponent* Component)
	{
		m_RootComponent = Component;
	}

	CSceneComponent* GetRootComponent()	const
	{
		return m_RootComponent;
	}

	void AddSceneComponent(CSceneComponent* Component)
	{
		m_SceneComponentList.push_back(Component);
	}

	void DeleteSceneComponent(CSceneComponent* Component)
	{
		auto iter = m_SceneComponentList.begin();
		auto iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter) == Component)
			{
				m_SceneComponentList.erase(iter);
				return;
			}
		}
	}

	void DeleteSceneComponent(const std::string& Name)
	{
		auto iter = m_SceneComponentList.begin();
		auto iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == Name)
			{
				m_SceneComponentList.erase(iter);
				return;
			}
		}
	}

	bool DeleteObjectComponent(const std::string& Name);

	class CComponent* FindComponent(const std::string& Name);
	template <typename T>
	T* FindComponentFromType()
	{
		auto	iter = m_SceneComponentList.begin();
		auto	iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->CheckType<T>())
				return (T*)*iter;
		}

		auto	iter1 = m_vecObjectComponent.begin();
		auto	iter1End = m_vecObjectComponent.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->CheckType<T>())
				return (T*)(*iter1).Get();
		}

		return nullptr;
	}

	CComponent* FindObjectComponent(const std::string& Name)
	{
		auto	iter = m_vecObjectComponent.begin();
		auto	iterEnd = m_vecObjectComponent.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == Name)
				return (*iter);
		}

		return nullptr;
	}

	template <typename T>
	T* FindObjectComponentFromType()
	{
		auto	iter1 = m_vecObjectComponent.begin();
		auto	iter1End = m_vecObjectComponent.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->CheckType<T>())
				return (T*)(*iter1).Get();
		}

		return nullptr;
	}


	template <typename T>
	void FindAllSceneComponentFromType(std::vector<T*>& vecComp)
	{
		auto	iter1 = m_SceneComponentList.begin();
		auto	iter1End = m_SceneComponentList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->CheckType<T>())
				vecComp.push_back((T*)(*iter1));
		}

	}

	template <typename T>
	void FindAllSceneComponentFromTypeName(const std::string& Name, std::vector<T*>& vecComp)
	{
		auto	iter1 = m_SceneComponentList.begin();
		auto	iter1End = m_SceneComponentList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->GetName().find(Name) != std::string::npos && (*iter1)->CheckType<T>())
				vecComp.push_back((T*)(*iter1));
		}

	}

	template <typename T>
	void FindAllObjectComponentFromType(std::vector<T*>& vecComp)
	{
		auto	iter1 = m_vecObjectComponent.begin();
		auto	iter1End = m_vecObjectComponent.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->CheckType<T>())
				vecComp.push_back((T*)(*iter1).Get());
		}

	}

	void GetAllSceneComponentsName(std::vector<FindComponentName>& vecNames);
	void GetAllSceneComponentsPointer(std::vector<CSceneComponent*>& OutVecSceneComp);
	void GetAllObjectComponentsPointer(std::vector<CObjectComponent*>& OutVecObjComp);
	size_t GetSceneComponentCount()	const
	{
		return m_SceneComponentList.size();
	}

	void SetLifeSpan(float LifeSpan)
	{
		m_LifeSpan = LifeSpan;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	void AddCollision();
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CGameObject* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool Save(const char* FullPath);
	virtual bool Load(const char* FullPath);
	virtual bool Save(const char* FileName, const std::string& PathName);
	virtual bool Load(const char* FileName, const std::string& PathName);
	virtual bool SaveHierarchy(FILE* File); // 기존 GameObject의 Save는 Child Object를 Save하지 않아서 추가한 함수
	virtual bool LoadHierarchy(FILE* File, CScene* NextScene = nullptr); // 기존 GameObject의 Load는 Child Object를 Load하지 않아서 추가한 함수
	virtual bool SaveHierarchy(const char* FullPath); // 기존 GameObject의 Save는 Child Object를 Save하지 않아서 추가한 함수
	virtual bool LoadHierarchy(const char* FullPath); // Load Scene을 할때는 새로운 Scene을 만들어서 그 Scene에 Load하므로 Scene이 인자로 필요

	virtual bool SaveOnly(CComponent* Component, const char* FullPath);
	virtual bool SaveOnly(const std::string& ComponentName, const char* FullPath);
	virtual bool LoadOnly(const char* FullPath, CComponent*& OutCom);
	void Reset();

public:
	const std::vector<CSharedPtr<CGameObject>>& GetVecChildObjects() const
	{
		return m_vecChildObject;
	}

	size_t GetChildObjectCount()	const
	{
		return m_vecChildObject.size();
	}

	CGameObject* GetChildObject(int Index)	const
	{
		return m_vecChildObject[Index];
	}
public:
	// Particle Object 의 경우 호출하는 함수
	void StartParticle(const Vector3& WorldPos);
public:
	// NavAgent가 있을 경우에 동작한다.
	void Move(const Vector3& EndPos);
	// m_NavAgent의 m_PathList에 TargetPos를 추가해준다
	void AddPath(const Vector3& TargetPos);
	void ClearPath();
public:
	void SetNavAgent(CNavAgent* Agent)
	{
		m_NavAgent = Agent;
	}

	CNavAgent* GetNavAgent()	const
	{
		return m_NavAgent;
	}

	void SetNavManagerLandScape(class CLandScape* LandScape);
	bool IsNavAgentPathListEmpty()	const;

public:
	template <typename T>
	T* CreateComponent(const std::string& Name)
	{
		T* Component = new T;

		Component->SetName(Name);
		Component->SetScene(m_Scene);
		Component->SetGameObject(this);

		if (!Component->Init())
		{
			SAFE_RELEASE(Component);
			return nullptr;
		}

		if (Component->GetComponentType() == Component_Type::ObjectComponent)
		{
			m_vecObjectComponent.push_back((class CObjectComponent*)Component);

			if (Component->GetTypeID() == typeid(CNavAgent).hash_code())
			{
				m_NavAgent = (CNavAgent*)Component;
				m_NavAgent->SetUpdateComponent(m_RootComponent);
			}
		}

		else
		{
			if (Component->GetTypeID() == typeid(CLandScape).hash_code())
			{
				SetNavManagerLandScape((CLandScape*)Component);
			}

			m_SceneComponentList.push_back((class CSceneComponent*)Component);

			if (!m_RootComponent)
				m_RootComponent = (class CSceneComponent*)Component;
		}

		return Component;
	}

	// Root가 이미 있으면 Root의 자식 Component로 자동으로 들어가게 하는 함수
	template <typename T>
	T* CreateComponentAddChild(const std::string& Name)
	{
		T* Component = new T;

		Component->SetName(Name);
		Component->SetScene(m_Scene);
		Component->SetGameObject(this);

		if (!Component->Init())
		{
			SAFE_RELEASE(Component);
			return nullptr;
		}

		if (Component->GetComponentType() == Component_Type::ObjectComponent)
		{
			m_vecObjectComponent.push_back((class CObjectComponent*)Component);

			if (Component->GetTypeID() == typeid(CNavAgent).hash_code())
			{
				m_NavAgent = (CNavAgent*)Component;
				m_NavAgent->SetUpdateComponent(m_RootComponent);
			}
		}

		else
		{
			m_SceneComponentList.push_back((class CSceneComponent*)Component);

			if (Component->GetTypeID() == typeid(CLandScape).hash_code())
			{
				SetNavManagerLandScape((CLandScape*)Component);
			}

			if (!m_RootComponent)
				m_RootComponent = (class CSceneComponent*)Component;

			else
				m_RootComponent->AddChild((class CSceneComponent*)Component);
		}

		Component->Start();

		return Component;

	}

	template <typename T>
	T* LoadComponent()
	{
		T* Component = new T;

		Component->SetScene(m_Scene);
		Component->SetGameObject(this);


		if (Component->GetComponentType() == Component_Type::ObjectComponent)
			m_vecObjectComponent.push_back((class CObjectComponent*)Component);

		else
		{
			m_SceneComponentList.push_back((class CSceneComponent*)Component);

			if (!m_RootComponent)
				m_RootComponent = Component;
		}

		//Component->Start();

		return Component;
	}

	template <typename T>
	T* LoadObjectComponent()
	{
		T* Component = new T;

		Component->SetScene(m_Scene);
		Component->SetGameObject(this);


		m_vecObjectComponent.push_back((class CObjectComponent*)Component);

		return Component;
	}

public:	// =============== 저주받은 Transform 영역 ===============
	void SetUpdateByMat(bool UpdateByMat);
	void DecomposeWorld();
	void SetTransformByWorldMatrix(const Matrix& matTRS);

	// 자신의 축 기준으로 이동
	void AddWorldPosByLocalAxis(AXIS Axis, float Amount);
	void AddWorldPosByLocalAxis(const Vector3& Pos);

	void SetInheritScale(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritScale(Inherit);
	}

	void SetInheritRotX(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritRotX(Inherit);
	}

	void SetInheritRotY(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritRotY(Inherit);
	}

	void SetInheritRotZ(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritRotZ(Inherit);
	}

	void SetInheritParentRotationPosX(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritParentRotationPosX(Inherit);
	}

	void SetInheritParentRotationPosY(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritParentRotationPosY(Inherit);
	}

	void SetInheritParentRotationPosZ(bool Inherit)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetInheritParentRotationPosZ(Inherit);
	}

	void InheritScale(bool Current)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->InheritScale(Current);
	}

	void InheritRotation(bool Current)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->InheritRotation(Current);
	}

	void InheritParentRotationPos(bool Current)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->InheritParentRotationPos(Current);
	}

	void InheritWorldScale(bool Current)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->InheritWorldScale(Current);
	}

	void InheritWorldRotation(bool Current)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->InheritWorldRotation(Current);
	}

public:
	Vector3 GetRelativeScale()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetRelativeScale();
	}

	Vector3 GetRelativeRot()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetRelativeRot();
	}

	Vector3 GetRelativePos()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetRelativePos();
	}

	Vector3 GetRelativeAxis(AXIS Axis)
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetRelativeAxis(Axis);
	}

public:
	void SetRelativeScale(const Vector3& Scale)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeScale(Scale);
	}

	void SetRelativeScale(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeScale(x, y, z);
	}

	void SetRelativeRotation(const Vector3& Rot)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeRotation(Rot);
	}

	void SetRelativeRotation(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeRotation(x, y, z);
	}

	void SetRelativeRotationX(float x)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeRotationX(x);
	}

	void SetRelativeRotationY(float y)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeRotationY(y);
	}

	void SetRelativeRotationZ(float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativeRotationZ(z);
	}

	void SetRelativePos(const Vector3& Pos)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativePos(Pos);
	}

	void SetRelativePos(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetRelativePos(x, y, z);
	}

	void AddRelativeScale(const Vector3& Scale)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeScale(Scale);
	}

	void AddRelativeScale(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeScale(x, y, z);
	}

	void AddRelativeRotation(const Vector3& Rot)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeRotation(Rot);
	}

	void AddRelativeRotation(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeRotation(x, y, z);
	}

	void AddRelativeRotationX(float x)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeRotationX(x);
	}

	void AddRelativeRotationY(float y)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeRotationY(y);
	}

	void AddRelativeRotationZ(float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativeRotationZ(z);
	}

	void AddRelativePos(const Vector3& Pos)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativePos(Pos);
	}

	void AddRelativePos(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddRelativePos(x, y, z);
	}

public:
	Vector3 GetWorldScale()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetWorldScale();
	}

	Vector3 GetWorldRot()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetWorldRot();
	}

	Vector3 GetWorldPos()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetWorldPos();
	}

	Vector3 GetPivot()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetPivot();
	}

	Vector3 GetMeshSize()	const
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetMeshSize();
	}

	const Matrix& GetWorldMatrix()	const
	{
		if (!m_RootComponent)
			return Matrix();

		return m_RootComponent->GetWorldMatrix();
	}

	Vector3 GetWorldAxis(AXIS Axis)
	{
		if (!m_RootComponent)
			return Vector3();

		return m_RootComponent->GetWorldAxis(Axis);
	}

public:
	void SetPivot(const Vector3& Pivot)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetPivot(Pivot);
	}

	void SetPivot(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetPivot(x, y, z);
	}

	void SetMeshSize(const Vector3& Size)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetMeshSize(Size);
	}

	void SetMeshSize(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetMeshSize(x, y, z);
	}

public:
	void SetWorldScale(const Vector3& Scale)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldScale(Scale);
	}

	void SetWorldScale(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldScale(x, y, z);
	}

	void SetWorldRotation(const Vector3& Rot)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldRotation(Rot);
	}

	void SetWorldRotation(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldRotation(x, y, z);
	}

	void SetWorldRotationX(float x)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldRotationX(x);
	}

	void SetWorldRotationY(float y)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldRotationY(y);
	}

	void SetWorldRotationZ(float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldRotationZ(z);
	}

	void SetWorldPos(const Vector3& Pos)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldPos(Pos);
	}

	void SetWorldPos(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->SetWorldPos(x, y, z);
	}

	void AddWorldScale(const Vector3& Scale)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldScale(Scale);
	}

	void AddWorldScale(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldScale(x, y, z);
	}

	void AddWorldRotation(const Vector3& Rot)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldRotation(Rot);
	}

	void AddWorldRotation(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldRotation(x, y, z);
	}

	void AddWorldRotationX(float x)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldRotationX(x);
	}

	void AddWorldRotationY(float y)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldRotationY(y);
	}

	void AddWorldRotationZ(float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldRotationZ(z);
	}

	void AddWorldPos(const Vector3& Pos)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldPos(Pos);
	}

	void AddWorldPos(float x, float y, float z)
	{
		if (!m_RootComponent)
			return;

		m_RootComponent->AddWorldPos(x, y, z);
	}
};



