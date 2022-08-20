#pragma once

#include "IMGUIWindow.h"

class CFileBrowserTree :
    public CIMGUIWindow
{
public:
	CFileBrowserTree();
	virtual ~CFileBrowserTree();

private:
	//std::vector<std::string>	m_vecCurrentPathDir;
	std::string					m_CurrentPath;
	std::string					m_CurrentFullPath;
	class CIMGUITree*			m_Root;
	class CIMGUIText*			m_CurrentPathText;

public:
	std::string GetCurrentFullPath()	const
	{
		return m_CurrentFullPath;
	}

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	void OnOpenBrowserTree(class CIMGUITree* Tree);
	void OnShowFileBrowser(class CIMGUITree* Tree);

private:
	
};

