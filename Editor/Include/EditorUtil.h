#pragma once

#include "GameInfo.h"
#include "Flag.h"
#include "EditorInfo.h"
#include <ShlObj_core.h>

class CEditorUtil
{
public:
	static std::string ConcatNumOverlapName(const std::string& OverlapPossibleString);

	// Ǯ��θ� �־��ָ� ���� �̸��� �ɷ��� ��������
	static std::string FilterFileName(const std::string& FullPath);

	// �ش� ��ο� �����ϴ� ��� ���� �̸��� ���丮 �̸����� ������
	static void GetAllFilenames(const std::string& PathName, std::vector<std::string>& vecFileNames,
		std::vector<std::string>& vecDirNames);

	static void GetAllFilenamesFullPath(const std::string& FullPath, std::vector<std::string>& vecFileNames,
		std::vector<std::string>& vecDirNames);

	// �ش� ���丮�� �����ϰ�, Ȯ���ڿ� ��ġ�ϴ� ���ϵ��� Ǯ ��θ� ������
	static void GetAllFileFullPathInDir(const char* TargetDir, std::vector<std::string>& OutVecFullPath,  
		const char* ExtFilter = nullptr);

	// �ش� ���丮�� �����ϰ�, Ȯ���ڿ� ��ġ�ϴ� ���ϵ��� ���� �̸����� ����
	static void GetAllFileNamesInDir(const char* TargetDir, std::vector<std::string>& OutVecFileName,
		const char* ExtFilter = nullptr);

	// Ư�� ���� �̸��� �����ϴ� ���ϵ��� ��ϸ��� �̾Ƴ��ִ� �Լ� 
	// ex) Singed1.anim , Singed2.anim ... --> InclueName �� "Singed" �� ������, -> Singed1.anim, Singed2.anim �� �ش��ϴ� ���ϵ��� FullPath ��� Filter
	static void FilterSpecificNameIncludedFilePath(std::vector<std::string>& InputVecFullPath, std::vector<std::string>& ReturnVecFullPath, const char* IncludeName);

	// ���丮 �̸��� m_Name���� ������ �ִ� Tree�� ù��° ���ڷ� �ָ� Root���� �� ���丮 ��α��� ��� ���丮�� �̾��ش�
	// (ex.Bin\\Texture\\Monster ��θ� list�� Bin, Texture, Monster�� �־��ش�)
	static void GetFullPathDirectory(class CIMGUITree* CurrentDir, std::list<std::string>& Output);

	// ���丮 �̸����� list�� �Ѱ��ָ� Root Full��ο� ���ؼ� ���� Ǯ��θ� ������ش�
	// (ex. list�� Texture, Monster��� �̸��� TreeNode�� ������, [Editor ������Ʈ���� ���� Ǯ���]\Editor\Bin\Texture\Monster �� ������ش�)
	static std::string MergeFullPath(const std::list<std::string> DirNames);

	// �ش� ���丮�� ��� ��ȸ�ϸ鼭, �ش� (���� �̸� + Ȯ����) �� ��ġ�ϴ� ���� "�ϳ�" �� Full ��θ� �������ش�.
	// ���� 3��° ���ڷ�, Ȯ���ڸ� ������, "���� �̸�"�� �޾ƿ� ���� �ִ�.
	static std::optional<std::string> GetFullPathOfTargetFileNameInDir(const std::string& PathName, 
		const std::string& TargetFileFullName, std::string& FileNameOnly);

	// Ư�� ���� �� ���丮�� �̸��� �ٲ��ִ� �Լ�
	static bool ChangeFileOrDirectoryName(const std::string& OriginFullPath, const std::string& NewName);

	// TCHAR [] ������ Text �� �־��ָ�, char [] (Multibyte) ���� Text ����, 
	static char* ChangeTCHARTextToMultibyte(TCHAR* TCHARText);

	// char [](Multibyte) ������ Text �� �־��ָ�, TCHAR []  ������ Text ����
	static  TCHAR* ChangeMultibyteTextToTCHAR(const std::string& MText);

	// FullPath �־��ָ�, FileName, Ext �� �̾��ִ� �Լ�;
	static void ExtractFileNameAndExtFromPath(const std::string& FullPath, std::string& FileName, std::string& Ext);
	static void ExtractFileNameAndExtFromPath(const std::string& FullPath, char* FileName, char* Ext);

	// �ش� Dir ��ο�, �ش� Name ���� �� ������ �����ϴ��� �Ǵ����ִ� �Լ�
	static bool IsFileExistInDir(const std::string& Path, const std::string& FileName);

	// Ư�� ���ڿ��� ����,
	// 1. ���� �ϳ��� �ҹ����� ���ڿ� set
	// 2. ���� �ϳ��� �빮���� ���ڿ� set
	// 3. ��� �ҹ���
	// 4. ��� �빮��
	// �� ��ȯ�� ���ڿ����� vector ���·� ������ִ� �Լ�
	static void GetAllKindsOfTransformedStringVersions(std::string OriginString, std::vector<std::string>&vecEachToLower,
		std::vector<std::string>& vecEachToUpper, std::string& strAllUpper, std::string& strAllLower);

	// Imgui Demo Window�� ����ִ� �Լ�
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
	const std::string Index = "Index";			// MaterialTextureInfo �迭���� ���° Idx ����
	const std::string Register = "Register";   // Register 
	const std::string Name = "Name";		   // MaterialTextureInfo �� ������ �̸�
	 // MaterialTextureInfo->Texture->TextureResourceInfo �迭 ���� ��� Texture FileName �� �����ϱ� 
	const std::string Textures[] = { "Texture1", "Texture2",  "Texture3",  "Texture4",  "Texture5",  "Texture6",  "Texture7"};
	const std::string RenderState[] = {"Blend", "Rasterizer", "DepthStencil"};
};


// ��ó : https://pyoungon.tistory.com/25
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:      // ���� ���� ��ȭ���ڸ� �ʱ�ȭ �� ��, �ʱ� ��� ����
	{
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
	}
	break;

	// BROWSEINFO ����ü�� ulFlags ���� BIF_STATUSTEXT �� ������ ��� ȣ��
	// ��, BIF_NEWDIALOGSTYLE �� �����Ǿ� ���� ��� ȣ����� ����
	case BFFM_SELCHANGED:       // ����ڰ� ������ ������ ��� ��ȭ���ڿ� ���õ� ��� ǥ��
	{
		TCHAR szPath[MAX_PATH] = { 0, };

		::SHGetPathFromIDList((LPCITEMIDLIST)lParam, szPath);
		::SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath);
	}
	break;

	// BROWSEINFO ����ü�� ulFlags ���� BIF_VALIDATE �� ������ ��� ȣ��
	// BIF_EDITBOX �� ���� ������ ��츸 ȣ���
	case BFFM_VALIDATEFAILED:   // ������ ��Ʈ�ѿ��� ���� �̸��� �߸� �Է��� ��� ȣ��
	{
		::MessageBox(hWnd, TEXT("�ش� ������ ã�� �� �����ϴ�."), TEXT("����"), MB_ICONERROR | MB_OK);
	}
	break;
	}

	return 0;
}