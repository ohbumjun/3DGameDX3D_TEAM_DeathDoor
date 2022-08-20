#include "IMGUITextInput.h"
#include "IMGUIUtil.h"

CIMGUITextInput::CIMGUITextInput() :
	m_TextType(ImGuiText_Type::String),
	m_Text{},
	m_wText{},
	m_TextUTF8{},
	m_HintText{},
	m_wHintText{},
	m_HintTextUTF8{},
	m_MultiLine(false),
	m_Flag(0),
	m_ValueInt(0),
	m_ValueFloat(0.f)
{
}

CIMGUITextInput::~CIMGUITextInput()
{
}

bool CIMGUITextInput::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUITextInput::Render()
{
	RenderText();

	ApplyDropEffect();

	ImGui::PopID();
}

void CIMGUITextInput::RenderText()
{
	bool	Input = false;

	ImGui::PushID(m_WidgetID);
	ImGui::PushItemWidth(m_Size.x);

	if (m_MultiLine)
	{
		Input = ImGui::InputTextMultiline(m_Name.c_str(), m_Text, 1024, m_Size, m_Flag);
	}

	else
	{
		switch (m_TextType)
		{
		case ImGuiText_Type::String:
			if (strlen(m_HintText) > 0)
			{
				Input = ImGui::InputTextWithHint(m_Name.c_str(), m_HintText, m_Text, 1024, m_Flag);
			}

			else
			{
				Input = ImGui::InputText(m_Name.c_str(), m_Text, 1024, m_Flag);
			}
			break;
		case ImGuiText_Type::Int:
			Input = ImGui::InputInt(m_Name.c_str(), &m_ValueInt, 1, 10, m_Flag);
			break;
		case ImGuiText_Type::Float:
			Input = ImGui::InputFloat(m_Name.c_str(), &m_ValueFloat, 0.f, 0.f, "%.3f", m_Flag);
			break;
		}
	}

	if (Input)
	{
		if (m_TextType == ImGuiText_Type::String)
		{
			memset(m_wText, 0, sizeof(wchar_t) * 1024);
			memset(m_TextUTF8, 0, sizeof(char) * 1024);

			int	Length = MultiByteToWideChar(CP_ACP, 0, m_Text, -1, 0, 0);
			MultiByteToWideChar(CP_ACP, 0, m_Text, -1, m_wText, Length);

			// UTF8로 변환한다.
			Length = WideCharToMultiByte(CP_UTF8, 0, m_wText, -1, 0, 0, 0, 0);
			WideCharToMultiByte(CP_UTF8, 0, m_wText, -1, m_TextUTF8, Length, 0, 0);

		}

		if (m_InputCallback)
			m_InputCallback();
	}
}

void CIMGUITextInput::ApplyDropEffect()
{
	bool	Input = false;

	// Drop 내용을 받는다.
	if (ImGui::BeginDragDropTarget())
	{
		// if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(vIMGUIDragDropPayLoadName.TextDragDrop))
		{
			// char type 배수 
			size_t DroppedTextSize = payload->DataSize;
			size_t DetNoSize = sizeof(TCHAR);
			if (DroppedTextSize % DetNoSize != 0)
				return;

			// 일반적으로 IMGUIText Widget을 Drop 할 것이다.
			// Drop 받은 내용을 TextType 에 맞게 변환할 것이다.
			TCHAR* payload_n = (TCHAR*)payload->Data;

			char DroppedTextMultibyte[MAX_PATH] = {};

			int ConvertLength = WideCharToMultiByte(CP_UTF8, 0, payload_n, -1, 0, 0, 0, 0);
			WideCharToMultiByte(CP_UTF8, 0, payload_n, -1, DroppedTextMultibyte, ConvertLength, 0, 0);

			memset(m_PrevText, 0, sizeof(char) * 1024);
			strcpy_s(m_PrevText, m_TextUTF8);

			switch (m_TextType)
			{
			case ImGuiText_Type::String:
			{
				// 그리고 IMGUIText 는 char[] 이기 때문에, char type 의 배수 크기인지 확인한다.
				char TextDrop[MAX_PATH] = {};
				strcpy_s(TextDrop, DroppedTextMultibyte);
				SetText(TextDrop);
			}
				break;
			case ImGuiText_Type::Int:
			{
				std::string IntDrop = DroppedTextMultibyte;
				int FinalIntInput = std::stoi(IntDrop);
				m_ValueInt = FinalIntInput;
			}
				break;
			case ImGuiText_Type::Float:
			{
				std::string FloatDrop = DroppedTextMultibyte;
				float FinalIntInput = std::stof(FloatDrop);
				m_ValueFloat = FinalIntInput;
			}
				break;
			}

			if (m_DropCallback)
			{
				m_DropCallback(DroppedTextMultibyte);
			}

		}
		ImGui::EndDragDropTarget();
	}
}

