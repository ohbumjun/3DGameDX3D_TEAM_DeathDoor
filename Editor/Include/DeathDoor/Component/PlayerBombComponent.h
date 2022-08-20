#pragma once

#include "ProjectileComponent.h"

class CPlayerBombComponent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CPlayerBombComponent();
	CPlayerBombComponent(const CPlayerBombComponent& com);
	virtual ~CPlayerBombComponent();

private:
	class CPlayerDataComponent* m_PlayerData;
	class CLightComponent* m_Light;
	std::queue<Vector3> m_LiftBombPathQueue;
	class CGameObject* m_Bomb;
	Vector3 m_LiftDir;
	float m_LiftSpeed;
	float m_ShootSpeed;
	float m_CollisionLifeTime;// 충돌하고 크기가 커지다가 얼마 후에 터질것인지
	float m_AccCollisionLifeTime;
	bool m_Collision;
	bool m_ShootFirstTime;	// 최초로 던지는 것인지 --> ReadyToShoot에서 이미 폭탄 이펙트가 재생되고 있는 중에 또 폭탄을 던지려고 할때 예외처리 해주기 위한 변수
	bool m_CancleAction;	// 폭탄 이펙트가 재생중이거나 할 때 또 폭탄 던지려고 하면 true
	bool m_IsClearBomb; // Lift하는 도중에 MouseRButton떼면 true
	bool m_BeforeLift;	// Lift하기 전인지 후인지

	int Count;

public:
	void ResetCount();

	void SetBomb(class CGameObject* Bomb);

	bool IsBeforeLift()	const
	{
		return m_BeforeLift;
	}

	bool IsClearBomb()	const
	{
		return m_IsClearBomb;
	}

	bool IsCancleAction()	const
	{
		return m_CancleAction;
	}

	bool IsShootFirstTime()	const
	{
		return m_ShootFirstTime;
	}

	bool IsBombCollision()	const
	{
		return m_Collision;
	}

	float GetLiftSpeed()	const
	{
		return m_LiftSpeed;
	}

	void SetBeforeLift(bool Lift)
	{
		m_BeforeLift = Lift;
	}

	void SetClearBomb(bool Clear)
	{
		m_IsClearBomb = Clear;
	}

	void SetCancleAction(bool Cancle)
	{
		m_CancleAction = Cancle;
	}

	void SetLiftSpeed(float Speed)
	{
		m_LiftSpeed = Speed;
	}

	void SetShootSpeed(float Speed)
	{
		m_ShootSpeed = Speed;
	}

	bool IsEmptyLiftPathQueue()	const
	{
		return m_LiftBombPathQueue.empty();
	}

	const Vector3& GetBombPos()		const;

	class CGameObject* GetBomb()	const;

	void OnBombProjectileDestroy(const Vector3& Pos);

	void ClearLiftPathQueue();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CPlayerBombComponent* Clone();

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public:
	void LiftBomb();
	void ShootBomb(const Vector3& ShootDir);
	void HideBomb();
	void ResetInfo();
	
public:
	void OnCollision(const CollisionResult& Result);
};

