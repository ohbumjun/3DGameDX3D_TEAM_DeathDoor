#pragma once

#include "IMGUIWidget.h"

// 빈 공간을 만들어주는 위젯
class CIMGUIDummy :
    public CIMGUIWidget
{
public:
    CIMGUIDummy();
    virtual ~CIMGUIDummy();
    
public:
    virtual bool Init() override;
    virtual void Render() override;
};

