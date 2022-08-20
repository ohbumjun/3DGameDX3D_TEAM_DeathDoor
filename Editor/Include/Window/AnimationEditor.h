#pragma once
#include "IMGUIWindow.h"
#include "../EditorInfo.h"
#include <filesystem>


class CAnimationEditor :
	public CIMGUIWindow
{
	friend class CEditorManager;
	// friend class CAnimationInstanceConvertThread;
public:
	CAnimationEditor();
	~CAnimationEditor();
private:
	// Anim Combo Box
	class CIMGUIComboBox* m_CurAnimComboBox;
	// Anim Clip Table
	class CIMGUITable* m_AnimInfoTable;
	// Frame Display
	class CIMGUITextInput* m_FrameInput;
	class CIMGUISliderInt* m_FrameSlider;
	class CIMGUIButton* m_SetOriginalPlayTimeBtn;
	// Frame ����
	class CIMGUITextInput* m_StartFrameEditInput;
	class CIMGUITextInput* m_EndFrameEditInput;
	class CIMGUIButton* m_StartEndFrameEditBtn;
	// Seq Add
	class CIMGUIButton* m_AnimSequenceAddBtn;
	class CIMGUIButton* m_CreateSample3DBtn;
	// Animation Seq �����
	class CIMGUIButton* m_DeleteAnimSequenceBtn;
	class CIMGUIButton* m_DeleteAnimObject;
	// Animation Play ����
	class CIMGUICheckBox* m_DeltaTimeCheckBtn;
	class CIMGUICheckBox* m_AnimationCheckBtn;
	class CIMGUICheckBox* m_RotationCheckBtn;
	class CIMGUICheckBox* m_ZoomEnableBtn;
	class CIMGUICheckBox* m_LoopEnableBtn;
	// New Anim Names
	class CIMGUITextInput* m_NewAnimSeqName;
	class CIMGUITextInput* m_NewAnimSeqDataKeyName;
	class CIMGUITextInput* m_EditAnimSeqDataKeyName;
	class CIMGUIButton* m_EditAnimKeyBtn;
	// Save Load
	class CIMGUIButton* m_SaveAnimationInstanceBtn;
	class CIMGUIButton* m_LoadAnimationInstanceBtn;
	// Loaded Anim File Name
	class CIMGUITextInput* m_LoadedAnimFileName;
	// Play Scale, Time
	class CIMGUITextInput* m_PlayScaleInput;
	class CIMGUIButton* m_PlayScaleEditBtn;
	class CIMGUITextInput* m_PlayTimeInput;
	class CIMGUIButton* m_PlayTimeEditBtn;
	// Excel
	class CIMGUITextInput* m_LoadedExcelFileName;
	class CIMGUIButton* m_LoadExcelBtn;
	class CIMGUIButton* m_MakeAnimInstByExcelBtn;
	// Render Target
	bool m_RenderTargetSet;
	class CIMGUIImage* m_AnimationRenderTarget;

	// Anim Instance Make
private:
	class CAnimationInstanceConvertThread* m_AnimInstanceConvertThread;
	// ������ Folder
	class CIMGUITextInput* m_AnimSeqcSrcFolderPath;
	// ���� File Name
	class CIMGUITextInput* m_CommonAnimSeqName;
	// Convert Btn
	class CIMGUIButton* m_ConvertAnimInstanceBtn;
	// Folder ����
	class CIMGUIButton* m_SelectAnimInstanceFolderPath;
	// Log
	class CIMGUIChild* m_AnimInstanceConvertLog;
	// ProgressBar
	class CIMGUIProgressBar* m_AnimInstanceProgressBar;
	// ������ Animation File Name
	class CIMGUITextInput* m_SavedAnimFileName;
private:
	// Excel
	class CExcelData* m_LoadedExcelData;
	class CIMGUITextInput* m_ExcelSavedAnimFileName;
	std::string m_ExcelKeyName;
	std::unordered_map<std::string, std::pair<int, int>> m_mapLoadedSqcExcel;
	// Animation
	class CAnim3DObject* m_3DTestObject;
	std::string m_3DTestObjectMeshName;
	class CAnimationSequenceInstance* m_Animation;
private:
	// Anim Sqc ���� Animation Instance ����� 
	// FullPath ��� ��Ƶ� Vector 
	std::vector<std::string> m_vecAnimationSeqFilesFullPath;
	char m_SelectedSeqSrcsDirPath[MAX_PATH];
	class CAnimationSequenceInstance* m_DummyAnimation;

public:
	const std::string& Get3DTestObjectMeshName() const
	{
		return m_3DTestObjectMeshName;
	}
	class CAnim3DObject* GetAnim3DObjectFromAnimationEditor() const
	{
		return m_3DTestObject;
	}
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
private:
	// Refresh 
	void OnRefreshAnimationClipTable(class CAnimationSequence* SequenceData);
	void OnRefreshFrameSliderInfo(class CAnimationSequence* SequenceData);
	void OnRefreshAnimationComboBox();
	void OnRefreshScaleAndTimeInputInfo();
	void OnRefreshCheckBoxInfo();
	// St, Ed Frame Edit
	void OnEditStartEndFrame();
	class CAnimationSequence* NewlyEditSqcFile(const TCHAR* FileSavedFullPath, CAnimationSequence* ExistingSequence,
		int StartFrame, int EndFrame);
	bool SaveEditedSqcFile(const TCHAR* FileSavedFullPath, CAnimationSequence* ExistingSequence,
		int StartFrame, int EndFrame, bool MakeCopy = true);
	// Play
	void OnCreateSample3DObject();
	void OnSetPlayEngineDeltaTime(const char*, bool);
	void OnPlayAnimation(const char*, bool);
	void OnRotateAnimationCamera(const char*, bool);
	void OnZoomAnimationCamera(const char*, bool);
	// Loop
	void OnLoopAnimation(const char*, bool);
	// Excel
	void OnLoadExcel();
	void OnMakeAnimInstByExcel();
	// Save Load
	void OnAddAnimationSequence();
	void OnSaveAnimationInstance();
	void OnLoadAnimationInstance();
	void OnApplyAnimationSlider(CAnimationSequence* Sequence);
	bool LoadElementsForSqcLoading(const char* SqcFileName);
	void SetMeshMaterialReadyForAnimation();
	// Edit
	void OnSetOriginalAnimPlayTime(); // ���� Sequence�� PlayTime �� ����Ͽ� PlayTime �� �ٽ� �����Ѵ�.
	void OnEditAnimPlayTime();
	void OnEditAnimPlayScale();
	void OnEditAnimSequenceKey();
	// Delete
	void OnDeleteAnimationSequenceData(); // Ư�� Animation �����
	void OnDeleteAnimation3DObject(); // ���� Animation Object ����� 
private:
	// Sqc ��Ƽ� Animation ����� 
	void OnAnimInstanceConvertLoading(const LoadingMessage& msg);
	void OnClickSetAnimSeqSrcDirButton();
	void OnConvertSequencesIntoAnimationInstance();
	// Animation Instance �� Add �� Key �̸��� return �Ѵ�.
	void AddSequenceToDummyAnimationInstance(const char* FileFullPath, std::string& AddedKeyName);
	bool CheckSavedFileNameDuplicated();
	bool CheckGatheredSeqFilesIncludeCommonName();
	void GatherFullPathInfoOfSqcFilesInSelectedDir(const std::string& SelectedDirPath, const std::string& CommonSqcName);

// Helper Functions
	void OnDeleteExisting3DObject();
	void OnClearExistingAnimationSeqInfos();
	void OnClickAnimationSequence(int, const char*);
	void OnSetAnimationComboBoxCallback(const std::string& AnimSequenceName);
	void OnAnimationSliderIntCallback(int);
	void OnAnimationFrameInputCallback();

};

