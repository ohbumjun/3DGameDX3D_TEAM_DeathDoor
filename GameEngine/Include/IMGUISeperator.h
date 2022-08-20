#pragma once

#include "IMGUIWidget.h"

class CIMGUISeperator :
    public CIMGUIWidget
{
public:
    CIMGUISeperator();
    virtual ~CIMGUISeperator();
    
public:
    virtual bool Init() override;
    virtual void Render() override;
};

