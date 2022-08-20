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
	// 해당 Dir 경로에, 해당 Name 으로 된 파일이 존재하는지 판단해주는 함수 + 존재할 시 FullPath 경로 리턴
	static std::optional<std::string> CheckAndExtractFullPathOfTargetFile(std::string_view Path, std::string_view FileName);

	// FullPath로부터 매크로로 정의된 PathName을 제외한 FileName을 리턴
	static std::string ExtractFilePathFromFullPath(const std::string& FullPath, const std::string& PathName);

	// 혹시나 "\\"가 경로에 있다면, 맨처음 경로부터 "\\" 까지의 정보를 뽑아내주는 함수
	// C::Program\\OBJ\\Material\\Hello.mtrl ? => C::Program\\OBJ\\Material\\ 까지의 정보를 뽑아와준다.
	static bool GetPathInfoBeforeFileName(const std::string& FilePath, std::string& ExtractedPathInfo);

	// 파일 이름 중에서 Extension만 리턴해주는 함수 (앞에 . 는 포함되지 않는다. 확장자 이름만 ex) .txt 에서 txt 만)
	static bool GetFileExt(const std::string& FileName, std::string& ExtractedExt);

	// 혹시나 "\\"가 경로에 있다면, 그 뒤의 파일 이름만을 뽑아내주는 함수
	// 만약 아예 인자로 들어오는 FilePath 에 "\\" 이 존재하지 않는다면, 그냥 FilePath 원본 그대로를 세팅해준다.
	static bool GetFileNameAfterSlash(const std::string& FilePath, std::string& ExtractedFileName);

	// 파일 이름 중에서, Extension 만 제외하고, 파일 본래 이름을 가져다주는 함수
	static bool GetFileNameOnly(const std::string& FullFileName, std::string& ExtractedFileName);

	// (OBJ : 혹시나 사용하고 있는 사람이 있나하여, 기존 함수 남겨둠) 해당 Dir 경로에, 해당 Name 으로 된 파일이 존재하는지 판단해주는 함수 
	static bool IsFileExistInDir(const std::string& Path, const std::string& FileName);

	// 특정 파일을, 현재 디렉토리에서, 다른 디렉토리로 복사하기
	// FileName 에는 확장자 정보까지 들어가 있어야 한다.
	static bool CopyFileToOtherDirectory(const struct PathInfo* CurrentPathInfo, const struct PathInfo* TargetPathInfo, const std::string& FileName);

	// string 으로 path 정보 넘겨주면 알아서 해당 Directory로 복사해주는 기능
	static bool CopyFileToOtherDirectory(const std::string& OriginFullPath, const std::string& TargetFullPath, 
		std::string& FinalCopyPath, bool Recursive = false);

	// 해당 디렉토리가 존재하는지 확인하고, 존재하지 않는다면 해당 디렉토리를 만들어준다. (파일이 아니라, 폴더를 만들어주는 것)
	static void CheckAndMakeDirectory(const struct PathInfo* Info);

	// 풀경로를 넣어주면 파일 이름만 걸러서 리턴해줌
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

	// 사각형을 정의하는 4개의 정점 -> 해당 사각형 안에, 특정 Vector 가 위치하는지 확인하기 (2차원)
	static bool CheckInsideSquare(const Vector2& V1, const Vector2& V2,
		const Vector2& V3, const Vector2& V4, const Vector2& TargetPos);

	// 밑이 a 인 지수함수를 이용해서, 실시간으로 증가하는 지수 함수 형태의 값을 얻어내기
	static float CalculateRealTimeSpeedUsingExponentialWithBottom(float Bottom, float CurTime, float InitSpeed);

	// 총 이동 시간, Init Speed, EndSpeed 를 이용하여, 실시간 변하는 속도 구하기
	// (확 증가하다가, 서서히 증가)
	// (서서히 감소하다가, 확 감소)
	static float CalculateRealTimeSpeedUsingExponentialWithSpeed(float FullTime, float InitSpeed, float EndSpeed, float CurTime);
};

