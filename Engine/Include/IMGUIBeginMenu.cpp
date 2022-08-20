#include "IMGUIBeginMenu.h"
#include "IMGUIMenuItem.h"

CIMGUIBeginMenu::CIMGUIBeginMenu()
{
}

CIMGUIBeginMenu::~CIMGUIBeginMenu()
{
	size_t size = m_vecMenuChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_vecMenuChild[i]);
	}
}

bool CIMGUIBeginMenu::Init()
{
	return true;
}

void CIMGUIBeginMenu::Render()
{
	if (ImGui::BeginMenu(m_Name.c_str()))
	{
		mSize = m_vecMenuChild.size();

		for (size_t i = 0; i < mSize; ++i)
		{
			m_vecMenuChild[i]->Render();
		}

		ImGui::EndMenu();
	}
}

CIMGUIMenuItem* CIMGUIBeginMenu::AddMenuItem(const std::string& name, const float width, const float height)
{
	CIMGUIMenuItem* widget = new CIMGUIMenuItem;

	widget->SetName(name);
	widget->SetShortCut(name);
	widget->SetOwner(m_Owner);
	widget->SetSize(width, height);

	if (!widget->Init())
	{
		SAFE_DELETE(widget);
		assert(false);
		return nullptr;
	}
	m_vecMenuChild.push_back(widget);

	return widget;
}
