#pragma once

#include "GameInfo.h"
#include "Flag.h"
#include "EditorInfo.h"
#include <ShlObj_core.h>

class CEditorUtil
{
public:
	static std::string ConcatNumOverlapName(const std::string& OverlapPossibleString);

	// 풀경로를 넣어주면 파일 이름만 걸러서 리턴해줌
	static std::string FilterFileName(const std::string& FullPath);

	// 해당 경로에 존재하는 모든 파일 이름과 디렉토리 이름들을 가져옴
	static void GetAllFilenames(const std::string& PathName, std::vector<std::string>& vecFileNames,
		std::vector<std::string>& vecDirNames);

	static void GetAllFilenamesFullPath(const std::string& FullPath, std::vector<std::string>& vecFileNames,
		std::vector<std::string>& vecDirNames);

	// 해당 디렉토리에 존재하고, 확장자와 일치하는 파일들의 풀 경로를 리턴함
	static void GetAllFileFullPathInDir(const char* TargetDir, std::vector<std::string>& OutVecFullPath,  
		const char* ExtFilter = nullptr);

	// 해당 디렉토리에 존재하고, 확장자와 일치하는 파일들의 파일 이름들을 리턴
	static void GetAllFileNamesInDir(const char* TargetDir, std::vector<std::string>& OutVecFileName,
		const char* ExtFilter = nullptr);

	// 특정 파일 이름을 포함하는 파일들의 목록만을 뽑아내주는 함수 
	// ex) Singed1.anim , Singed2.anim ... --> InclueName 에 "Singed" 를 넣으면, -> Singed1.anim, Singed2.anim 에 해당하는 파일들의 FullPath 경로 Filter
	static void FilterSpecificNameIncludedFilePath(std::vector<std::string>& InputVecFullPath, std::vector<std::string>& ReturnVecFullPath, const char* IncludeName);

	// 디렉토리 이름을 m_Name으로 가지고 있는 Tree를 첫번째 인자로 주면 Root부터 그 디렉토리 경로까지 모든 디렉토리를 뽑아준다
	// (ex.Bin\\Texture\\Monster 경로면 list에 Bin, Texture, Monster를 넣어준다)
	static void GetFullPathDirectory(class CIMGUITree* CurrentDir, std::list<std::string>& Output);

	// 디렉토리 이름들을 list로 넘겨주면 Root Full경로와 더해서 최종 풀경로를 만들어준다
	// (ex. list에 Texture, Monster라는 이름의 TreeNode가 있으면, [Editor 프로젝트까지 로컬 풀경로]\Editor\Bin\Texture\Monster 를 만들어준다)
	static std::string MergeFullPath(const std::list<std::string> DirNames);

	// 해당 디렉토리를 모두 순회하면서, 해당 (파일 이름 + 확장자) 와 일치하는 파일 "하나" 의 Full 경로를 리턴해준다.
	// 또한 3번째 인자로, 확장자를 제외한, "파일 이름"만 받아올 수도 있다.
	static std::optional<std::string> GetFullPathOfTargetFileNameInDir(const std::string& PathName, 
		const std::string& TargetFileFullName, std::string& FileNameOnly);

	// 특정 파일 및 디렉토리의 이름을 바꿔주는 함수
	static bool ChangeFileOrDirectoryName(const std::string& OriginFullPath, const std::string& NewName);

	// TCHAR [] 형태의 Text 을 넣어주면, char [] (Multibyte) 형태 Text 리턴, 
	static char* ChangeTCHARTextToMultibyte(TCHAR* TCHARText);

	// char [](Multibyte) 형태의 Text 을 넣어주면, TCHAR []  형태의 Text 리턴
	static  TCHAR* ChangeMultibyteTextToTCHAR(const std::string& MText);

	// FullPath 넣어주면, FileName, Ext 만 뽑아주는 함수;
	static void ExtractFileNameAndExtFromPath(const std::string& FullPath, std::string& FileName, std::string& Ext);
	static void ExtractFileNameAndExtFromPath(const std::string& FullPath, char* FileName, char* Ext);

	// 해당 Dir 경로에, 해당 Name 으로 된 파일이 존재하는지 판단해주는 함수
	static bool IsFileExistInDir(const std::string& Path, const std::string& FileName);

	// 특정 문자열에 대해,
	// 1. 문자 하나만 소문자인 문자열 set
	// 2. 문자 하나만 대문자인 문자열 set
	// 3. 모두 소문자
	// 4. 모두 대문자
	// 로 변환한 문자열들을 vector 형태로 만들어주는 함수
	static void GetAllKindsOfTransformedStringVersions(std::string OriginString, std::vector<std::string>&vecEachToLower,
		std::vector<std::string>& vecEachToUpper, std::string& strAllUpper, std::string& strAllLower);

	// Imgui Demo Window를 띄워주는 함수
	static void ShowDemo();

	static std::string ObjectToString(Object_Type Object);

	static std::string SceneComponent3DTypeToString(SceneComponent3DType Type);
	static std::string ObjectComponent3DTypeToString(ObjectComponent3DType Type);

	static size_t ObjectTypeIndexToTypeid(int TypeIndex);
	static int ObjectTypeIdToTypeIndex(size_t ObjectTypeID);

	static size_t SceneComponentTypeIndexToTypeid(int TypeIndex);
	static size_t ObjectComponentTypeIndexToTypeid(int TypeIndex);
	
	static std::string ActioNodeTypeToString(ActionNode Type);
	static std::string ConditionNodeTypeToString(ConditionNode Type);

public:
	static bool CompareExt(const char* FullPath, const char ExtFilter[_MAX_EXT]);

};

namespace AnimationClipInfoKeys
{
	const std::string FrameRange = "Frame Range";
	const std::string FrameLength = "Frame Length";
	const std::string FrameMode = "Frame Mode"; // 24, 30, 60
	const std::string PlayTime = "Play Time";
	const std::string FrameTime = "Frame Time";
	const std::string PlayScale = "Play Scale";
	const std::string AnimSeqKey = "Seq Key";
	const std::string AnimSeqFileName = "Seq File Name";
};


namespace MaterialTextureInfoKeys
{
	const std::string Index = "Index";			// MaterialTextureInfo 배열에서 몇번째 Idx 인지
	const std::string Register = "Register";   // Register 
	const std::string Name = "Name";		   // MaterialTextureInfo 로 설정된 이름
	 // MaterialTextureInfo->Texture->TextureResourceInfo 배열 내의 모든 Texture FileName 들 나열하기 
	const std::string Textures[] = { "Texture1", "Texture2",  "Texture3",  "Texture4",  "Texture5",  "Texture6",  "Texture7"};
	const std::string RenderState[] = {"Blend", "Rasterizer", "DepthStencil"};
};


// 출처 : https://pyoungon.tistory.com/25
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:      // 폴더 선택 대화상자를 초기화 할 때, 초기 경로 설정
	{
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
	}
	break;

	// BROWSEINFO 구조체의 ulFlags 값에 BIF_STATUSTEXT 가 설정된 경우 호출
	// 단, BIF_NEWDIALOGSTYLE 가 설정되어 있을 경우 호출되지 않음
	case BFFM_SELCHANGED:       // 사용자가 폴더를 선택할 경우 대화상자에 선택된 경로 표시
	{
		TCHAR szPath[MAX_PATH] = { 0, };

		::SHGetPathFromIDList((LPCITEMIDLIST)lParam, szPath);
		::SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath);
	}
	break;

	// BROWSEINFO 구조체의 ulFlags 값에 BIF_VALIDATE 가 설정된 경우 호출
	// BIF_EDITBOX 와 같이 설정된 경우만 호출됨
	case BFFM_VALIDATEFAILED:   // 에디터 콘트롤에서 폴더 이름을 잘못 입력한 경우 호출
	{
		::MessageBox(hWnd, TEXT("해당 폴더를 찾을 수 없습니다."), TEXT("오류"), MB_ICONERROR | MB_OK);
	}
	break;
	}

	return 0;
}