#pragma once

#include "IMGUIWindow.h"
#include "IMGUIChild.h"

class CFileBrowser :
	public CIMGUIWindow
{
public:
	CFileBrowser();
	virtual ~CFileBrowser();

private:
	std::vector<std::string>	m_vecFileName;
	std::vector<std::string>	m_vecDirName;
	std::vector<CIMGUIChild*>	m_vecChild;

	bool						m_UpdatePath;
	std::string					m_InitialPath;

	bool						m_UpdateFullPath;
	std::string					m_InitialFullPath;

	size_t						m_SingleLineIconNum;

public:
	void SetUpdatePath(bool Update)
	{
		m_UpdatePath = Update;
	}

	const std::string& GetInitialPath()	const
	{
		return m_InitialPath;
	}

	const std::string& GetInitialFullPath()	const
	{
		return m_InitialFullPath;
	}

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	void SetInitialPath(const std::string& Path);
	void SetInitialFullPath(const std::string& FullPath);
	void ClearWidget();
	// 파일(또는 디렉토리) 이미지를 클릭하면 호출되는 콜백
	void FileClickCallback(class CIMGUIImage* Image);

	void OnDragSrcImageButton(const std::string& FileName);
};
