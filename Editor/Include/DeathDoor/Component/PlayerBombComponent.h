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
	float m_CollisionLifeTime;// �浹�ϰ� ũ�Ⱑ Ŀ���ٰ� �� �Ŀ� ����������
	float m_AccCollisionLifeTime;
	bool m_Collision;
	bool m_ShootFirstTime;	// ���ʷ� ������ ������ --> ReadyToShoot���� �̹� ��ź ����Ʈ�� ����ǰ� �ִ� �߿� �� ��ź�� �������� �Ҷ� ����ó�� ���ֱ� ���� ����
	bool m_CancleAction;	// ��ź ����Ʈ�� ������̰ų� �� �� �� ��ź �������� �ϸ� true
	bool m_IsClearBomb; // Lift�ϴ� ���߿� MouseRButton���� true
	bool m_BeforeLift;	// Lift�ϱ� ������ ������

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

