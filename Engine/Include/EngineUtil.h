#pragma once

#include "GameInfo.h"

class CEngineUtil
{
public:
	// Enum To String
	static std::string LightTypeToString(Light_Type Type);
	static Light_Type StringToLightType(const std::string& TypeString);

	static std::string FogTypeToString(Fog_Type Type);
	static Fog_Type StringToFogType(const std::string& TypeString);

	static std::string ToonShaderTypeToString(ToonShaderType Type);
	static ToonShaderType StringToToonShaderType(const std::string& TypeString);

	static std::string PaperBurnEndEventTypeToString(PaperBurnEndEvent Type);
	static PaperBurnEndEvent StringToPaperBurnEndEvent(const std::string& TypeString);

	// String Related
	// �ش� Dir ��ο�, �ش� Name ���� �� ������ �����ϴ��� �Ǵ����ִ� �Լ� + ������ �� FullPath ��� ����
	static std::optional<std::string> CheckAndExtractFullPathOfTargetFile(std::string_view Path, std::string_view FileName);

	// FullPath�κ��� ��ũ�η� ���ǵ� PathName�� ������ FileName�� ����
	static std::string ExtractFilePathFromFullPath(const std::string& FullPath, const std::string& PathName);

	// Ȥ�ó� "\\"�� ��ο� �ִٸ�, ��ó�� ��κ��� "\\" ������ ������ �̾Ƴ��ִ� �Լ�
	// C::Program\\OBJ\\Material\\Hello.mtrl ? => C::Program\\OBJ\\Material\\ ������ ������ �̾ƿ��ش�.
	static bool GetPathInfoBeforeFileName(const std::string& FilePath, std::string& ExtractedPathInfo);

	// ���� �̸� �߿��� Extension�� �������ִ� �Լ� (�տ� . �� ���Ե��� �ʴ´�. Ȯ���� �̸��� ex) .txt ���� txt ��)
	static bool GetFileExt(const std::string& FileName, std::string& ExtractedExt);

	// Ȥ�ó� "\\"�� ��ο� �ִٸ�, �� ���� ���� �̸����� �̾Ƴ��ִ� �Լ�
	// ���� �ƿ� ���ڷ� ������ FilePath �� "\\" �� �������� �ʴ´ٸ�, �׳� FilePath ���� �״�θ� �������ش�.
	static bool GetFileNameAfterSlash(const std::string& FilePath, std::string& ExtractedFileName);

	// ���� �̸� �߿���, Extension �� �����ϰ�, ���� ���� �̸��� �������ִ� �Լ�
	static bool GetFileNameOnly(const std::string& FullFileName, std::string& ExtractedFileName);

	// (OBJ : Ȥ�ó� ����ϰ� �ִ� ����� �ֳ��Ͽ�, ���� �Լ� ���ܵ�) �ش� Dir ��ο�, �ش� Name ���� �� ������ �����ϴ��� �Ǵ����ִ� �Լ� 
	static bool IsFileExistInDir(const std::string& Path, const std::string& FileName);

	// Ư�� ������, ���� ���丮����, �ٸ� ���丮�� �����ϱ�
	// FileName ���� Ȯ���� �������� �� �־�� �Ѵ�.
	static bool CopyFileToOtherDirectory(const struct PathInfo* CurrentPathInfo, const struct PathInfo* TargetPathInfo, const std::string& FileName);

	// string ���� path ���� �Ѱ��ָ� �˾Ƽ� �ش� Directory�� �������ִ� ���
	static bool CopyFileToOtherDirectory(const std::string& OriginFullPath, const std::string& TargetFullPath, 
		std::string& FinalCopyPath, bool Recursive = false);

	// �ش� ���丮�� �����ϴ��� Ȯ���ϰ�, �������� �ʴ´ٸ� �ش� ���丮�� ������ش�. (������ �ƴ϶�, ������ ������ִ� ��)
	static void CheckAndMakeDirectory(const struct PathInfo* Info);

	// Ǯ��θ� �־��ָ� ���� �̸��� �ɷ��� ��������
	static std::string FilterFileName(const std::string& FullPath);

	// Collision Profile
	static std::string CollisionChannelToString(Collision_Channel eChannnel);
	static Collision_Channel StringToCollisionChannel(const std::string& String);
	static Collision_Interaction StringToCollisionInteraction(const std::string& String);
	static std::string CollisionInteractionToString(Collision_Interaction eInteraction);

	// String
	static std::string BoolToString(bool Bool);
	static bool StringToBool(const std::string& Str);

	// Bazier 
	static void CalculateBazierTargetPoses(const Vector3& D1, const Vector3& D2,const Vector3& D3, const Vector3& D4,
		std::queue<Vector3>& queuePoses, int DetailNum);
	static void CalculateBazierTargetPoses(const Vector3& D1, const Vector3& D2, const Vector3& D3, const Vector3& D4,
		std::vector<Vector3>& vecPoses, int DetailNum);

	// Quaternion To Euler Angle
	static Vector3 QuarternionToEulerAngles(const XMVECTOR& Qut);

	// �簢���� �����ϴ� 4���� ���� -> �ش� �簢�� �ȿ�, Ư�� Vector �� ��ġ�ϴ��� Ȯ���ϱ� (2����)
	static bool CheckInsideSquare(const Vector2& V1, const Vector2& V2,
		const Vector2& V3, const Vector2& V4, const Vector2& TargetPos);

	// ���� a �� �����Լ��� �̿��ؼ�, �ǽð����� �����ϴ� ���� �Լ� ������ ���� ����
	static float CalculateRealTimeSpeedUsingExponentialWithBottom(float Bottom, float CurTime, float InitSpeed);

	// �� �̵� �ð�, Init Speed, EndSpeed �� �̿��Ͽ�, �ǽð� ���ϴ� �ӵ� ���ϱ�
	// (Ȯ �����ϴٰ�, ������ ����)
	// (������ �����ϴٰ�, Ȯ ����)
	static float CalculateRealTimeSpeedUsingExponentialWithSpeed(float FullTime, float InitSpeed, float EndSpeed, float CurTime);
};

