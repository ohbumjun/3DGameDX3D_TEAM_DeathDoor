#pragma once

#include "Thread.h"

class CDDSceneLoadingThread :
    public CThread
{
    friend class CDDSceneMode;
    friend class CDDLogoScene;

protected:
    CDDSceneLoadingThread();
    virtual ~CDDSceneLoadingThread();

protected:
    virtual void Run() override;

public:
    void Load(const std::string& SceneFileName, class CDDSceneMode* Owner);

public:
    template<typename T>
    void Load(T* Obj, const std::string& SceneFileName, void(T::* Func)())
    {
		m_NextSceneFileName = SceneFileName;
        m_EndFunction = std::bind(Func, Obj);

        Run();
    } 

private:
    class CDDSceneMode* m_Owner;
    std::string m_NextSceneFileName;
    std::function<void()> m_EndFunction;
};

