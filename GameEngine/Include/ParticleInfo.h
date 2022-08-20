#pragma once

struct	ParticleCBuffer
{
	unsigned int	SpawnEnable;	// ���� ��ƼŬ ���� ����
	Vector3	StartMin;		// ��ƼŬ�� ������ ������ Min

	Vector3	StartMax;		// ��ƼŬ�� ������ ������ Max
	unsigned int	SpawnCountMax;	// ������ ��ƼŬ�� �ִ�

	Vector3	ScaleMin;		// ������ ��ƼŬ ũ���� Min
	float	LifeTimeMin;	// ������ ��ƼŬ�� ������� �ּҽð�

	Vector3	ScaleMax;		// ������ ��ƼŬ ũ���� Max
	float	LifeTimeMax;	// ������ ��ƼŬ�� ������� �ִ�ð�

	Vector4	ColorMin;		// ������ ��ƼŬ�� ���� Min
	Vector4	ColorMax;		// ������ ��ƼŬ�� ���� Max

	float	SpeedStart;		// ��ƼŬ�� �ּ� �̵��ӵ�
	float	SpeedEnd;		// ��ƼŬ�� �ִ� �̵��ӵ�
	int		Move;			// �̵��� �ϴ��� ���ϴ���
	int		Gravity;		// �߷� ������ �޴��� �ȹ޴���

	Vector3	MoveDir;		// �̵��� �Ѵٸ� ������ �� �̵� ����
	int		DestroyExistingAllLivingParticles;			// 2D�� ��ƼŬ����

	Vector3	MoveAngle;	// �̵��� �Ѵٸ� ������ �� �������κ��� x, y, z �� ����� ������ŭ Ʋ���� ������ ������ ���Ѵ�.
	int ParticleBounce;

	// ��� Particle �鿡 �����ϰ� ������ Rotation Angle
	// ParticleComponent PostUpdate ���� WorldRot ������ �������ְ� �ȴ�.
	Vector3 RotationAngle;
	float	ParticleBounceResistance;

	int ParticleShapeType;
	float GenerateRadius;
	int IsLoopGenerateRing;
	int SpecialMoveDirType;
	// int     ParticleBounce;
	// float	 ParticleBounceResistance;

	float AlphaEnd;
	float AlphaStart;
	int IsLifeTimeLinearFromCenter;
	int SeperateLinerRotate;

	int UVMoveEnable;
	int UVRowN;
	int UVColN;
	int ApplyLinearEmissiveChange;

	// �� Particle ���� �ٸ��� Rotation Angle�� �ִ� ���
	Vector3 MinSeperateRotAngle;
	// �ѹ��� ó�� SpawnCount ������ŭ �����ǰ� �����ϱ� (Ȯ �����ǰ�, Ȯ �������)
	// SpawnTime �� ������ ���� ��������ų� �ϴ� ȿ�� X
	int DisableNewAlive;

	Vector3 MaxSeperateRotAngle;
	int AlphaLinearFromCenter;

	// Particle Component �󿡼� �����ϴ� Scale ����
	// Vector3 CommonWorldScale;
	Vector3 StartEmissiveColor;
	// ���� �������� ���� UV Clipping �Ǵ� ȿ�� �����ϱ� 
	int LinearUVClippingEnable;

	// Vector3 CommonParticleComponentWorldPos;
	Vector3 EndEmissiveColor;
	int SpeedChangeMethod; // Linear, ���� ����

	int ApplyNoiseTexture; // Pixel Shader ���� �ż��� Noise Texture �� ����, Sampling �� �ؼ� Color, Alpha �� ���� �ٲٴ� ��
	float NoiseTextureApplyRatio; // ������� �����ϴ� Ratio ���� 
	int LinearClippingReverseDir; // Liner UV Clipping ���� �ݴ�
	int FollowRealTimeParticleComponentPos; // �ǽð����� Particle Component �� ��ġ�� ���󰡰� �� ���ΰ� 

	Vector3 ParticleEmptyInfo1;
	int RotateXYDistFromCenter; // ���� ����� ������ ���ؼ�, ��� ���� �������κ��� ������ �ִ� ������ ����ؼ� Rotate �����ֱ� 

	Vector3 ParticleEmptyInfo2;
	float ParticleInfo2;
};

struct	ParticleTempValCBuffer
{
	Vector3 CommonWorldScale;
	int ResetParticleSharedInfoSumSpawnCnt;

	Vector3 CommonParticleComponentWorldPos; //
	int ParticleEmpty1;

	Vector3 RotatedAxisX;
	int ParticleEmpty2;

	Vector3 RotatedAxisY;
	int ParticleEmpty3;

	Vector3 RotatedAxisZ;
	int ParticleEmpty4;
};

struct ParticleInfo
{
	Vector3	WorldPos;
	Vector3	Dir;
	float	Speed;
	float	LifeTime;

	float	LifeTimeMax;
	int		Alive;
	float	FallTime;
	float	FallStartY;

	// �� Particle �� ó�� Alpha ���� ��
	float AlphaDistinctStart;

	// 1) Circle, Ring �� Particle Shape
	// 2) Linear Rot �� ���õǴ� �� 
	float CurrentParticleAngle;

	Vector3 SeperateRotAngleOffset;
	Vector3 FinalSeperateRotAngle;

	float  InitLocalPosY;
	Vector3 InitParticleComponentWorldPos;

	float DestroyedDuringAlive;
	float SingleInfoEmpty1;
	float SingleInfoEmpty2;
	Vector3 SingleEmptyInfo3;
};

struct ParticleInfoShared
{
	unsigned int	SpawnEnable;
	unsigned int	SpawnCountMax;

	// ���ݱ��� ������ Particle ����
	unsigned int	CurrentSpawnCountSum;

	// �ѹ��� ó�� SpawnCount ������ŭ �����ǰ� �����ϱ� 
	// SpawnTime �� ������ ���� ��������ų� �ϴ� ȿ�� X
	int DisableNewAlive;

	Vector3	ScaleMin;
	Vector3	ScaleMax;

	Vector4	ColorMin;
	Vector4	ColorMax;

	Vector3	CommonRelativeScale;

	int		GravityEnable;

	Vector3  CommonRotationAngle;
	Vector3  SeperateMinRotAngle;
	Vector3  SeperateMaxRotAngle;

	Vector3  CommonParticleCompWorldPos;

	float MaxDistFromCenter;

	float  PrevRingAngle;

	int    UVMoveEnable;
	int    UVRowN;
	int    UVColN;

	Vector3 ShareEmptyInfo1;
	Vector3 ShareEmptyInfo2;
};


enum class ParticleSpeedChangeMethod
{
	Linear = 1,
	Exponential,  // �ӵ� ��ȭ�� ���� ũ��
	Log, /// �ӵ� ��ȭ�� ���� �۰� 
	Max
};



enum class ParitcleShapeType
{
	YUpDirRing = 0, // ���� �ٶ󺸴� ���·� ��, Y �� �ٶ󺸴� ���·� Ring ����
	Circle,
	ZMinusDirRing, // ����� ������ �ٶ󺸴� ���·� Ring ����
	Sphere,
	Max
};

enum class ParticleSpecialMoveDir
{
	XZSpread = 0, // xz ��� �������� �̵� y�� 0
	XYSpread, // xy ��� �������� �̵� z �� 0
	Max
};
