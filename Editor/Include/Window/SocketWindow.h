#pragma once

#include "IMGUIWindow.h"
#include "Resource/Animation/Skeleton.h"

class CSocketWindow :
	public CIMGUIWindow
{
	friend class CEditorManager;

public:
	CSocketWindow();
	~CSocketWindow();

private:
	class CIMGUITextInput* m_SkeletonNameTextInput;
	class CIMGUITextInput* m_SocketNameTextInput;
	class CIMGUIButton* m_LoadBoneButton;
	class CIMGUIButton* m_AddSokcetButton;
	class CIMGUIListBox* m_LoadedSkeletonListBox;	// SceneResource에 로드된 스켈레톤들을 보여주는 ListBox
	class CIMGUIListBox* m_SelectedSkeletonBoneListBox; // 선택된 스켈레톤 내부의 Bone을 보여주는 ListBox
	class CIMGUIButton* m_SaveBoneFileButton;

	class CIMGUIListBox* m_SelectedSkeletonSocketListBox;

	class CSkeleton* m_SelectSkelton;
	Bone* m_SelectBone;
	char m_SrcFileFullPath[MAX_PATH];

public:
	virtual bool Init();

private:
	void OnLoadBoneFile();
	void OnSelectSkeleton(int Index, const char* Label);
	void OnSelectBone(int Index, const char* Label);
	void OnAddSocket();
	void OnSaveBoneFile();
};

