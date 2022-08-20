#pragma once

#include "IMGUIWindow.h"

class CBehaviorTreeMenuBar :
    public CIMGUIWindow
{
public:
	CBehaviorTreeMenuBar();
	virtual ~CBehaviorTreeMenuBar();

private:
	class CBehaviorTreeWindow* m_TreeEditorWindow;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
private :
	void OnSaveGameObject(class CGameObject* Object);
	class CGameObject* OnLoadGameObject();
};

