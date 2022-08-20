#pragma once

#include "Widget/WidgetWindow.h"

class CDDMouseWidgetWindow :
    public CWidgetWindow
{
	friend class CViewport;
	friend class CWidgetComponent;
	friend class CEngine;

protected:
    CDDMouseWidgetWindow();
    virtual ~CDDMouseWidgetWindow();

public:
    virtual bool Init() override;
	virtual void Render() override;

private:
    class CImage* m_Image;
};

