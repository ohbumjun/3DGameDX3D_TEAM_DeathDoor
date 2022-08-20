#pragma once

#include "../../GameInfo.h"

class CShaderManager
{
	friend class CResourceManager;

private:
	CShaderManager();
	~CShaderManager();

private:
	std::unordered_map<std::string, CSharedPtr<class CShader>>	m_mapShader;
	std::unordered_map<std::string, CSharedPtr<class CConstantBuffer>>	m_mapConstantBuffer;
	std::unordered_map<class CGraphicShader*, class CGraphicShader*> m_mapInstancingShader;
	std::function<void()> m_ChangeCallBack;

public:
	bool Init();
	class CShader* FindShader(const std::string& Name);
	class CGraphicShader* FindInstancingShader(class CGraphicShader* Shader);
	void ReleaseShader(const std::string& Name);

	bool CreateConstantBuffer(const std::string& Name, int Size, int Register,
		int ConstantBufferShaderType = (int)Buffer_Shader_Type::All);
	class CConstantBuffer* FindConstantBuffer(const std::string& Name);

public :
	const std::unordered_map<std::string, CSharedPtr<class CShader>>& GetMapShader() const
	{
		return m_mapShader;
	}

public:
	template <typename T>
	bool CreateShader(const std::string& Name)
	{
		T* Shader = (T*)FindShader(Name);

		if (Shader)
			return false;

		Shader = new T;

		Shader->SetName(Name);

		if (!Shader->Init())
		{
			SAFE_RELEASE(Shader);
			return false;
		}

		m_mapShader.insert(std::make_pair(Name, Shader));

		if (m_ChangeCallBack)
		{
			m_ChangeCallBack();
		}

		return true;
	}

	template <typename Origin, typename Instancing>
	bool CreateInstancingShader(Origin* OriginShader, const std::string& InstancingShaderName)
	{
		bool Success = CreateShader<Instancing>(InstancingShaderName);

		if (!Success)
		{
			return false;
		}

		CGraphicShader* InstancingShader = (CGraphicShader*)FindShader(InstancingShaderName);
		m_mapInstancingShader.insert(std::make_pair(OriginShader, InstancingShader));

		return true;
	}

public:
	template <typename T>
	void AddResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ChangeCallBack = std::bind(Func, Obj);
	}
};
