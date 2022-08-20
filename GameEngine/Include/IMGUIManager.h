#pragma once

#include "GameInfo.h"
#include <string>
#include <iostream>

class CIMGUIManager
{
private:
	ImGuiContext* m_Context;
	std::unordered_map<std::string, class CIMGUIWindow*>	m_mapWindow;
	std::unordered_map<std::string, ImFont*>	m_mapFont;
	ImFont* m_CurrentFont;

	// Tree마다 갖고 있는 고유한 ID
	int m_GlobalTreeID; 
	// Tree를 Drag&Drop할때 Src Tree에서는 지워졌고, Dest Tree로 들어가기 전 저장해두는 멤버
	class CIMGUITree* m_StagingTree;

public:
	int GetGlobalTreeID()	const
	{
		return m_GlobalTreeID;
	}

	void AddGloablTreeID(int ID)
	{
		m_GlobalTreeID += ID;
	}

	void SetStagingTree(class CIMGUITree* Tree);

	class CIMGUITree* GetStagingTree()	const;

public:
	bool Init(HWND hWnd);
	void Update(float DeltaTime);
	void Render();
	class CIMGUIWindow* FindIMGUIWindow(const std::string& Name);

public:
	bool AddFont(const std::string& Name, const char* FileName, float Size, bool Korea = false, int OverH = 2, int OverV = 1,
		float Spacing = 1.f, const std::string& PathName = FONT_PATH);
	bool AddFontFullPath(const std::string& Name, const char* FullPath, float Size, bool Korea = false, int OverH = 2, int OverV = 1,
		float Spacing = 1.f);
	void SetCurrentFont(const std::string& Name);
	ImFont* FindFont(const std::string& Name);
	void SetCurrentFont();
	void ResetCurrentFont();

public:
	template <typename T>
	T* AddWindow(const std::string& Name)
	{
		T* Window = (T*)FindIMGUIWindow(Name);

		if (Window)
			return Window;

		Window = new T;

		Window->SetName(Name);
		if (!Window->Init())
		{
			SAFE_DELETE(Window);
			return nullptr;
		}

		m_mapWindow.insert(std::make_pair(Name, Window));

		return Window;
	}

	DECLARE_SINGLE(CIMGUIManager)
};

// namespace DragDropPayLoadName
// {
// 	const char* Default = "DND_DEMO_CELL";
// 	const char* TextDragDrop = "DND_TEXT_CELL";
// };