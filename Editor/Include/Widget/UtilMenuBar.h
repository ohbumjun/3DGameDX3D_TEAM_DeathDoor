#pragma once
#include "IMGUIBeginMenu.h"
class CUtilMenuBar :
    public CIMGUIBeginMenu
{
public:
	CUtilMenuBar();
	virtual ~CUtilMenuBar();
private :
	// Scene
	class CIMGUIMenuItem* m_FBXConvertWindow;
	class CIMGUIMenuItem* m_FileBrowserTree;
public:
	virtual bool Init() override;
	virtual void Render() override;
private:
	void OnToggleFBXConvertWindow();
	void OnToggleFileBrowser();
};

