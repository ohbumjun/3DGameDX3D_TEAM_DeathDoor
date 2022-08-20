
#include "IMGUIImageButton.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture/Texture.h"
#include "PathManager.h"

CIMGUIImageButton::CIMGUIImageButton()  :
	m_BackgroundColor(1.f, 1.f, 1.f, 1.f),
	m_TintColor(1.f, 1.f, 1.f, 1.f)
{

	m_Texture = new CTexture;
}

CIMGUIImageButton::~CIMGUIImageButton()
{
}

bool CIMGUIImageButton::SetTexture(CTexture* Texture)
{
	if (!m_Texture)
		return false;

	m_Texture = Texture;

	return true;
}

bool CIMGUIImageButton::SetTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	m_Texture->LoadTexture(Name, FileName, PathName);

	return true;
}

bool CIMGUIImageButton::SetTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	m_Texture->LoadTextureFullPath(Name, FullPath);

	return true;
}

bool CIMGUIImageButton::Init()
{
	CIMGUIWidget::Init();

    return true;
}

void CIMGUIImageButton::Render()
{
	if (!m_Texture)
		return;

	float Width = (float)m_Texture->GetWidth();
	float Height = (float)m_Texture->GetHeight();

	ImGui::PushID(m_WidgetID);

	ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
	ImVec2 uv1 = ImVec2(m_Size.x / Width, m_Size.y / Height);// UV coordinates for (32,32) in our texture

	if (ImGui::ImageButton(m_Texture->GetResource(), m_Size, uv0, uv1, -1, m_BackgroundColor, m_TintColor))
	{
		m_PressCount += 1;

		if (m_PressCount == 2)
		{
			m_PressCount = 0;
			m_DoubleClickCallback(m_Name);
		}
	}

	
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("DND_DEMO_CELL", &m_WidgetID, sizeof(int));

		// TODO : msh 파일일 경우에만 FullPath를 Inspector의 TextInput으로 Drag & Drop으로 가져갈 수 있게 하기
		// EditorUtil.h에 CompareExt로 확장자 비교하면 됨.
		// FullPath는 SetDragDropPayload의 2번째 인자로 넣어서 넘겨줘보자
		if (m_DragSrcCallback)
			m_DragSrcCallback(m_Name);

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			char* payload_n = (char*)payload->Data;


		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();

}
