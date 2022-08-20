
#include "EditorUtil.h"
// Library
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
// Engine
#include "PathManager.h"
#include "GameObject/Minion.h"
#include "GameObject/Champion.h"
#include "GameObject/MapObject.h"
#include "Component/LightComponent.h"
#include "Component/Arm.h"
#include "Component/LandScape.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderHalfLine.h"
#include "Component/ColliderRay.h"
#include "Component/NavMeshComponent.h"
#include "Component/WaterComponent.h"
#include "IMGUITree.h"
#include "Flag.h"
#include "EngineUtil.h"
#include "PathManager.h"

namespace fs = std::filesystem;

std::string CEditorUtil::ConcatNumOverlapName(const std::string& OverlapPossibleString)
{
	std::string input = OverlapPossibleString;
	char outputBuf[256] = {};

	if (input[input.length() - 1] == '0' || input[input.length() - 1] == '1' || input[input.length() - 1] == '2'
		|| input[input.length() - 1] == '3' || input[input.length() - 1] == '4' || input[input.length() - 1] == '5'
		|| input[input.length() - 1] == '6' || input[input.length() - 1] == '7' || input[input.length() - 1] == '8'
		|| input[input.length() - 1] == '9')
	{
		size_t Length = input.length();
		std::string NumPart;

		int UnderIdx = -1;

		for (size_t i = 0; i < Length; ++i)
		{
			if (input[i] == '_')
				UnderIdx = (int)i;

			if (input[i] == '0' || input[i] == '1' || input[i] == '2'
				|| input[i] == '3' || input[i] == '4' || input[i] == '5'
				|| input[i] == '6' || input[i] == '7' || input[i] == '8'
				|| input[i] == '9')
			{
				NumPart += input[i];
			}
		}

		std::stringstream ss(NumPart);

		int Num = 0;
		ss >> Num;

		++Num;
		ss.clear();
		ss << Num;

		input.erase(UnderIdx + 1);

		input += ss.str();
		strcpy_s(outputBuf, input.c_str());
	}

	else
	{
		input += "_1";
		strcpy_s(outputBuf, input.c_str());
	}

	return std::string(outputBuf);
}

std::string CEditorUtil::FilterFileName(const std::string& FullPath)
{
	size_t len = FullPath.length();

	std::string FileName;

	for (size_t i = len - 1; i >= 0; --i)
	{
		if (FullPath[i] == '\\' || FullPath[i] == '/')
		{
			FileName = FullPath.substr(i + 1);
			return FileName;
		}
	}

	return FileName;
}

void CEditorUtil::GetAllFilenames(const std::string& PathName, std::vector<std::string>& vecFileNames, std::vector<std::string>& vecDirNames)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (!Info)
		return;

	fs::path p = Info->Path;

	fs::directory_iterator itr(p);

	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path iterpath = entry.path();

		bool isDirectory = fs::is_directory(iterpath);

		std::string fullPath = entry.path().string();

		std::string fileName = FilterFileName(fullPath);

		if (isDirectory)
			vecDirNames.push_back(fileName);

		else
			vecFileNames.push_back(fileName);

		++itr;
	}
}

void CEditorUtil::GetAllFilenamesFullPath(const std::string& FullPath, std::vector<std::string>& vecFileNames, std::vector<std::string>& vecDirNames)
{
	fs::path p = FullPath;

	fs::directory_iterator itr(p);

	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path iterpath = entry.path();

		bool isDirectory = fs::is_directory(iterpath);

		std::string fullPath = entry.path().string();

		std::string fileName = FilterFileName(fullPath);

		if (isDirectory)
			vecDirNames.push_back(fileName);

		else
			vecFileNames.push_back(fileName);

		++itr;
	}
}

void CEditorUtil::GetAllFileFullPathInDir(const char* TargetDir, std::vector<std::string>& OutVecFullPath, const char* ExtFilter)
{
	fs::directory_iterator itr(TargetDir);

	char Ext[_MAX_EXT] = {};

	if (ExtFilter)
	{
		strcpy_s(Ext, ExtFilter);
		_strupr_s(Ext);
	}

	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path iterpath = entry.path();

		bool isDirectory = fs::is_directory(iterpath);

		if (isDirectory)
		{
			++itr;
			continue;
		}

		std::string fullPath = entry.path().string();

		if (ExtFilter)
		{
			if (!CompareExt(fullPath.c_str(), Ext))
			{
				++itr;
				continue;
			}
		}

		OutVecFullPath.push_back(fullPath);

		++itr;
	}
}

void CEditorUtil::GetAllFileNamesInDir(const char* TargetDir, std::vector<std::string>& OutVecFileName, const char* ExtFilter)
{
	fs::directory_iterator itr(TargetDir);

	char Ext[_MAX_EXT] = {};

	if (ExtFilter)
	{
		strcpy_s(Ext, ExtFilter);
		_strupr_s(Ext);
	}

	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;
		fs::path iterpath = entry.path();
		bool isDirectory = fs::is_directory(iterpath);
		if (isDirectory)
		{
			++itr;
			continue;
		}

		std::string fileName = entry.path().filename().string();

		if (ExtFilter)
		{
			if (!CompareExt(fileName.c_str(), Ext))
			{
				++itr;
				continue;
			}
		}

		OutVecFileName.push_back(fileName);

		++itr;
	}
}

void CEditorUtil::FilterSpecificNameIncludedFilePath(std::vector<std::string>& InputVecFullPath, std::vector<std::string>& ReturnVecFullPath, 
	const char* IncludeName)
{
	ReturnVecFullPath.clear();
	ReturnVecFullPath.reserve(InputVecFullPath.size());

	size_t InputSize = InputVecFullPath.size();
	
	for (size_t i = 0; i < InputSize; ++i)
	{
		// Common Name 을 포함하고 있지 않다면 Skips
		// size_t nPos = InputVecFullPath[i].find(IncludeName);
		// FullPath 에서 중간 경로 들이 아니라, 오로지 FileName 이 IncludeName Str 을 포함하고 있는지를 조사해야 한다.
		// 따라서 폴더 경로 제외, FileName 을 중간에 추출해야 한다.
		std::string CurFileName;
		CEngineUtil::GetFileNameAfterSlash(InputVecFullPath[i], CurFileName);
		size_t nPos = CurFileName.find(IncludeName);

		if (nPos == std::string::npos)
			continue;

		ReturnVecFullPath.push_back(InputVecFullPath[i]);
	}
}

void CEditorUtil::GetFullPathDirectory(CIMGUITree* CurrentDir, std::list<std::string>& Output)
{
	while (CurrentDir->GetParent() != nullptr)
	{
		Output.push_front(CurrentDir->GetName());
		CurrentDir = CurrentDir->GetParent();
	}
}

std::string CEditorUtil::MergeFullPath(const std::list<std::string> DirNames)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(ROOT_PATH);

	std::string FullPath = Info->PathMultibyte;

	auto iter = DirNames.begin();
	auto iterEnd = DirNames.end();

	for (; iter != iterEnd; ++iter)
	{
		FullPath += (*iter);
		FullPath += "\\";
	}

	return FullPath;
}

std::optional<std::string> CEditorUtil::GetFullPathOfTargetFileNameInDir(const std::string& PathName,
	const std::string& TargetFileName, std::string& FileNameOnly)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (!Info)
		return  std::nullopt;

	char FolderName[MAX_PATH] = {};

	strcpy_s(FolderName, Info->PathMultibyte);

	fs::path TargetFolder(FolderName);
	
	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(TargetFolder))
	{
		const std::string& FileName = FilterFileName(entry.path().u8string());
	
		if (strcmp(FileName.c_str(), TargetFileName.c_str()) == 0)
		{
			CEngineUtil::GetFileNameOnly(fs::path(entry.path()).filename().u8string(), FileNameOnly);

			return entry.path().u8string();
		}
	}

	return std::nullopt;
}

bool CEditorUtil::ChangeFileOrDirectoryName(const std::string& OriginFullPath, const std::string& NewName)
{
	fs::path TargetPath = OriginFullPath;

	// Folder 경로 세팅
	std::string RenamePathStr;
	CEngineUtil::GetPathInfoBeforeFileName(OriginFullPath, RenamePathStr);

	RenamePathStr.append(NewName);

	fs::path ReNamedPath = RenamePathStr;

	// Folder 의 경우(dir) 단순히 rename 이 안된다.
	// 따라서, 아예 복사를 해준 다음, 해당 디렉토리의 이름을 바꺼주는 방식을 취한다.

	if (fs::is_directory(TargetPath))
	{
		// 빈 디렉토리 생성 
		fs::create_directory(ReNamedPath);

		// 혹시나 하는 예외처리
		// create_directory 의 경우, 해당 경로에 이미 해당 dir 이 존재하면, create 하지 않는다.
		// 이때, 아래에서 copy 를 해주기 위해서는, ReNamedPath 폴더가 비어있어야 한다.
		// 따라서, ReNamePath 를 우선 비워줄 것이다.
		// 기존 폴더 내에 있던 데이터들 모두 삭제
		while (true)
		{
			bool is_modified = false;

			for (const fs::directory_entry& entry :
				fs::recursive_directory_iterator(ReNamedPath))
			{
				fs::remove(entry.path());
				is_modified = true;
				break;
			}

			if (!is_modified)
				break;
		}
		
		// 폴더 내용 복사
		fs::copy(TargetPath, ReNamedPath, fs::copy_options::recursive);

		// 기존 Folder 는 지워준다.
		while (true)
		{
			bool is_modified = false;

			for (const fs::directory_entry& entry :
				fs::recursive_directory_iterator(TargetPath))
			{
				fs::remove(entry.path());
				is_modified = true;
				break;
			}

			if (!is_modified)
				break;
		}

		// 기존 폴더 삭제
		fs::remove(TargetPath);
	}
	else
	{
		fs::rename(TargetPath, ReNamedPath);
	}

	return true;
}



char* CEditorUtil::ChangeTCHARTextToMultibyte(TCHAR* TCHARText)
{
	char FilePathMultibyte[MAX_PATH] = {};

	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, TCHARText, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, TCHARText, -1, FilePathMultibyte, ConvertLength, 0, 0);

	return FilePathMultibyte;
}

// void CEditorUtil::ChangeMultibyteTextToTCHAR(const std::string& MText, std::wstring& TCHARTextResult)
TCHAR* CEditorUtil::ChangeMultibyteTextToTCHAR(const std::string& MText)
{
	TCHAR TCHARTextResult[MAX_PATH] = {};

	int ConvertLength = MultiByteToWideChar(CP_ACP, 0, MText.c_str(), -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, MText.c_str(), -1, TCHARTextResult, ConvertLength);

	return TCHARTextResult;
}

void CEditorUtil::ExtractFileNameAndExtFromPath(const std::string& FullPath, std::string& FileNameResult, std::string& ExtResult)
{
	char FileName[MAX_PATH] = {};
	char FileExt[_MAX_EXT] = {};

	_splitpath_s(FullPath.c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, FileExt, _MAX_EXT);

	FileNameResult = FileName;
	ExtResult = FileExt;
}

void CEditorUtil::ExtractFileNameAndExtFromPath(const std::string& FullPath, char* FileName, char* FileExt)
{
	_splitpath_s(FullPath.c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, FileExt, _MAX_EXT);
}

bool CEditorUtil::IsFileExistInDir(const std::string& PathName, const std::string& TargetFileName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (!Info)
	{
		assert(false);
		return  false;
	}

	char FolderName[MAX_PATH] = {};

	strcpy_s(FolderName, Info->PathMultibyte);

	fs::path TargetFolder(FolderName);

	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(TargetFolder))
	{
		const std::string& FileName = FilterFileName(entry.path().u8string());

		if (strcmp(TargetFileName.c_str(), FileName.c_str()) == 0)
		{
			return true;
		}
	}

	return false;
}

void CEditorUtil::GetAllKindsOfTransformedStringVersions(std::string OriginalString, 
	std::vector<std::string>& vecEachToLower, 
	std::vector<std::string>& vecEachToUpper, 
	std::string& strAllUpper, std::string& strAllLower)
{
	// 모두 대문자
	strAllUpper.resize(OriginalString.size());
	std::transform(OriginalString.begin(), OriginalString.end(), strAllUpper.begin(), ::toupper);

	// 모두 소문자 
	strAllLower.resize(OriginalString.size());
	std::transform(OriginalString.begin(), OriginalString.end(), strAllLower.begin(), ::tolower);

	// 원본 모두 소문자로 바꾸기
	std::transform(OriginalString.begin(), OriginalString.end(), OriginalString.begin(), ::tolower);

	vecEachToUpper.clear();

	// 하나씩 대문자 
	{
		size_t  StrSize = OriginalString.size();

		std::string CpyString = OriginalString;

		for (size_t Index = 0; Index < StrSize; ++Index)
			vecEachToUpper.push_back(CpyString);

		for (size_t Index = 0; Index < StrSize; ++Index)
		{
			// abcd
			// - Abcd
			// - aBcd
			// - abCd
			// - abcD
			vecEachToUpper[Index][Index] = toupper(vecEachToUpper[Index][Index]);
		}
	}

	// 원본 모두 대문자로 바꾸기
	std::transform(OriginalString.begin(), OriginalString.end(), OriginalString.begin(), ::toupper);

	vecEachToLower.clear();

	// 하나씩 소문자
	{
		size_t  StrSize = OriginalString.size();

		std::string CpyString = OriginalString;

		for (size_t Index = 0; Index < StrSize; ++Index)
			vecEachToLower.push_back(CpyString);

		for (size_t Index = 0; Index < StrSize; ++Index)
		{
			// ABCD
			// - aBCD
			// - AbCD
			// - ABcD
			// - ABCd
			vecEachToLower[Index][Index] = tolower(vecEachToLower[Index][Index]);
		}
	}
}

void CEditorUtil::ShowDemo()
{
	static bool Open = false;

	ImGui::ShowDemoWindow(&Open);
}

std::string CEditorUtil::ObjectToString(Object_Type Object)
{
	switch (Object)
	{
	case Object_Type::Player:
		return "Player";
	case Object_Type::Monster:
		return "Monster";
	case Object_Type::MapObject:
		return "MapObject";
	}

	return "";
}

// TODO : SceneComponent Type 추가될 때 마다 추가
std::string CEditorUtil::SceneComponent3DTypeToString(SceneComponent3DType Type)
{
	switch (Type)
	{
	case SceneComponent3DType::AnimationMeshComponent:
		return "AnimationMeshComponent";
	case SceneComponent3DType::StaticMeshComponent:
		return "StaticMeshComponent";
	case SceneComponent3DType::CameraComponent:
		return "CameraComponent";
	case SceneComponent3DType::LandScapeComponent:
		return "LandScapeComponent";
	case SceneComponent3DType::ArmComponent:
		return "ArmComponent";
	case SceneComponent3DType::LightComponent:
		return "LightComponent";
	case SceneComponent3DType::SceneComponent:
		return "SceneComponent";
	case SceneComponent3DType::ParticleComponent:
		return "ParticleComponent";
	case SceneComponent3DType::ColliderBox3D:
		return "ColliderBox3D";
	case SceneComponent3DType::ColliderSphere:
		return "ColliderSphere";
	case SceneComponent3DType::ColliderHalfLine:
		return "ColliderHalfLine";
	case SceneComponent3DType::ColliderRay:
		return "ColliderRay";
	case SceneComponent3DType::NavMeshComponent:
		return "NavMeshComponent";
	case SceneComponent3DType::WaterComponent:
		return "WaterComponent";
	}

	return "";
}

std::string CEditorUtil::ObjectComponent3DTypeToString(ObjectComponent3DType Type)
{
	switch (Type)
	{
	case ObjectComponent3DType::PaperBurnComponent:
		return "PaperBurnComponent";
	case ObjectComponent3DType::NavAgent:
		return "NavAgent";
	}

	return "";
}

size_t CEditorUtil::ObjectTypeIndexToTypeid(int TypeIndex)
{
	switch (TypeIndex)
	{
	case 0:
		return typeid(CGameObject).hash_code();
	case 1:
		return typeid(CMovingObject).hash_code();
	case 2:
		return typeid(CMapObject).hash_code();
	case 3:
		return typeid(CChampion).hash_code();
	case 4:
		return typeid(CMinion).hash_code();
	}

	return -1;
}

size_t CEditorUtil::SceneComponentTypeIndexToTypeid(int TypeIndex)
{
	// TODO : SceneComponent Type 추가될 때 마다 추가
	switch (TypeIndex)
	{
	case (int)SceneComponent3DType::AnimationMeshComponent:
		return typeid(CAnimationMeshComponent).hash_code();

	case (int)SceneComponent3DType::StaticMeshComponent:
		return typeid(CStaticMeshComponent).hash_code();

	case (int)SceneComponent3DType::LandScapeComponent:
		return typeid(CLandScape).hash_code();

	case (int)SceneComponent3DType::ArmComponent:
		return typeid(CArm).hash_code();

	case (int)SceneComponent3DType::LightComponent:
		return typeid(CLightComponent).hash_code();

	case (int)SceneComponent3DType::SceneComponent:
		return typeid(CSceneComponent).hash_code();

	case (int)SceneComponent3DType::ParticleComponent:
		return typeid(CParticleComponent).hash_code();

	case (int)SceneComponent3DType::ColliderBox3D:
		return typeid(CColliderBox3D).hash_code();

	case (int)SceneComponent3DType::ColliderSphere:
		return typeid(CColliderSphere).hash_code();

	case (int)SceneComponent3DType::ColliderHalfLine:
		return typeid(CColliderHalfLine).hash_code();

	case (int)SceneComponent3DType::ColliderRay:
		return typeid(CColliderRay).hash_code();

	case (int)SceneComponent3DType::NavMeshComponent:
		return typeid(CNavMeshComponent).hash_code();

	case (int)SceneComponent3DType::WaterComponent:
		return typeid(CWaterComponent).hash_code();

	case (int)SceneComponent3DType::CameraComponent:
		return typeid(CCameraComponent).hash_code();
	}

	return -1;
}

// TODO : ObjectComponent 종류 추가될 때 마다 추가
size_t CEditorUtil::ObjectComponentTypeIndexToTypeid(int TypeIndex)
{
	switch (TypeIndex)
	{
	case (int)ObjectComponent3DType::PaperBurnComponent:
		return typeid(CPaperBurnComponent).hash_code();

	case (int)ObjectComponent3DType::NavAgent:
		return typeid(CNavAgent).hash_code();

	}

	return -1;
}

std::string CEditorUtil::ActioNodeTypeToString(ActionNode Type)
{
	std::string str = "";

	switch (Type)
	{
	case ActionNodeMax:
		break;
	}

	return str;
}

std::string CEditorUtil::ConditionNodeTypeToString(ConditionNode Type)
{
	std::string str = "";

	switch (Type)
	{
	case ConditionNodeMax:
		break;
	}

	return str;
}

bool CEditorUtil::CompareExt(const char* FullPath, const char ExtFilter[_MAX_EXT])
{
	char FullPathExt[_MAX_EXT] = {};
	char FilterBuf[_MAX_EXT] = {};

	if (ExtFilter[0] == '.')
	{
		strcpy_s(FilterBuf, ExtFilter + 1);
	}
	else
	{
		strcpy_s(FilterBuf, ExtFilter);
	}
	_strupr_s(FilterBuf);

	int ExtLen = (int)strlen(FilterBuf);
	int Len = (int)strlen(FullPath);

	strncpy_s(FullPathExt, FullPath + Len - ExtLen, 4);
	_strupr_s(FullPathExt);

	return (strcmp(FilterBuf, FullPathExt) == 0);
}

