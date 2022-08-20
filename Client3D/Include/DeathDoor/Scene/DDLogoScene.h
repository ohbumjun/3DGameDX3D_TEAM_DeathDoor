#pragma once

#include "Scene\SceneMode.h"

class CDDLogoScene :
    public CSceneMode
{
	friend class CScene;

protected:
	CDDLogoScene();
	virtual ~CDDLogoScene();

public:
	virtual bool Init() override;
	virtual void Start() override;

public:
	void SetStartCallBack(class CButton* Button);
	void OnClickStartButton();
	void OnLoadEnd();
	void OnFadeOutEnd();

private:
	class CButton* m_StartButton;
	class CDDSceneLoadingThread* m_NextSceneLoadingThread;
};

