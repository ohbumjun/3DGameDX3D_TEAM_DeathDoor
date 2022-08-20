#include "Engine.h"
#include "Device.h"
#include "Resource/ResourceManager.h"
#include "PathManager.h"
#include "Timer.h"
#include "Scene/SceneManager.h"
#include "Render/RenderManager.h"
#include "Input.h"
#include "IMGUIManager.h"
#include "Collision/CollisionManager.h"
#include "Resource/Shader/GlobalConstantBuffer.h"
#include "Resource/Shader/StructuredBuffer.h"
#include "ObjectPoolManager.h"
#include <time.h>

DEFINITION_SINGLE(CEngine)

bool CEngine::m_Loop = true;

CEngine::CEngine()	:
	m_ClearColor{},
	m_Timer(nullptr),
	m_Start(false),
	m_Play(true),
	m_Pause(false),
	m_Space(Engine_Space::Space3D),
	m_MouseState(Mouse_State::Normal),
	m_ShowCursorCount(0),
	m_GlobalCBuffer(nullptr),
	m_GlobalAccTime(0.f),
	m_EditMode(false)
{
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(8951);
	m_ClearColor[0] = 0.88f; 
	m_ClearColor[1] = 0.88f; 
	m_ClearColor[2] = 0.88f; 
	 
	srand(unsigned int(GetTickCount()));
	rand();
}

CEngine::~CEngine()
{
	CSceneManager::DestroyInst();

	m_RandomBuffer->ResetShader(90, (int)Buffer_Shader_Type::Compute);

	SAFE_DELETE(m_GlobalCBuffer); 
	SAFE_DELETE(m_RandomBuffer); 

	CInput::DestroyInst();

	CRenderManager::DestroyInst();

	CCollisionManager::DestroyInst();

	CPathManager::DestroyInst();

	CResourceManager::DestroyInst();

	if (m_EditMode)
	{
		CIMGUIManager::DestroyInst();
	}

	CDevice::DestroyInst();

	CObjectPoolManager::DestroyInst();

	SAFE_DELETE(m_Timer);
}

void CEngine::SetMouseState(Mouse_State State)
{
	if (m_MouseWidget[(int)m_MouseState])
	{
	}

	m_MouseState = State;
}

bool CEngine::Init(HINSTANCE hInst, const TCHAR* Name, unsigned int Width,
	unsigned int Height, int IconID, bool WindowMode)
{
	m_hInst = hInst;

	m_RS.Width = Width;
	m_RS.Height = Height;

	Register(Name, IconID);
	Create(Name);

	return Init(hInst, m_hWnd, Width, Height, WindowMode);
}

bool CEngine::Init(HINSTANCE hInst, HWND hWnd,
	unsigned int Width, unsigned int Height, bool WindowMode)
{
	m_hInst = hInst;
	m_hWnd = hWnd;

	m_RS.Width = Width;
	m_RS.Height = Height;

	m_Timer = new CTimer;

	if (!CDevice::GetInst()->Init(m_hWnd, Width, Height, WindowMode))
		return false;

	if (!CObjectPoolManager::GetInst()->Init())
		return false;

	if (!CPathManager::GetInst()->Init())
		return false;

	if (!CResourceManager::GetInst()->Init())
		return false;

	if (!CCollisionManager::GetInst()->Init())
		return false;

	if (!CInput::GetInst()->Init(m_hInst, m_hWnd))
		return false;

	if (m_EditMode)
	{
		if (!CIMGUIManager::GetInst()->Init(m_hWnd))
			return false;
	}


	if (!CRenderManager::GetInst()->Init())
		return false;

	m_GlobalCBuffer = new CGlobalConstantBuffer;

	if (!m_GlobalCBuffer->Init())
		return false;

	m_GlobalCBuffer->SetResolution(m_RS);


	if (!CSceneManager::GetInst()->Init())
		return false;


	m_RandomBuffer = new CStructuredBuffer;

	m_RandomBuffer->Init("RandomBuffer", sizeof(float), 10000, 10, true);

	srand((unsigned int)time(0));
	rand();

	float	RandNumber[10000] = {};

	for (int i = 0; i < 10000; ++i)
	{
		RandNumber[i] = rand() % 10001 / 10000.f;
	}

	m_RandomBuffer->UpdateBuffer(RandNumber, 10000);


	m_RandomBuffer->SetShader(90, (int)Buffer_Shader_Type::Compute);


	// NoiseTexture
	CResourceManager::GetInst()->LoadTexture("GlobalNoiseTexture", TEXT("noise_01.png"));

	m_GlobalNoiseTexture = CResourceManager::GetInst()->FindTexture("GlobalNoiseTexture");

	m_GlobalNoiseTexture->SetShader(100, (int)Buffer_Shader_Type::All, 0);

	m_GlobalCBuffer->SetNoiseResolution((float)m_GlobalNoiseTexture->GetWidth(), (float)m_GlobalNoiseTexture->GetHeight());

	return true;
}

int CEngine::Run()
{
	MSG msg = {};

	while (m_Loop)
	{

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);


			DispatchMessage(&msg);
		}


		else
		{
			Logic();
		}
	}

	return (int)msg.wParam;
}

void CEngine::Logic()
{
	if (!m_Start)
	{
		m_Start = true;
		CSceneManager::GetInst()->Start();
	}

	m_Timer->Update();

	float	DeltaTime = m_Timer->GetDeltaTime();

	if (m_Pause)
		DeltaTime = 0.f;

	m_GlobalAccTime += DeltaTime;

	m_GlobalCBuffer->SetDeltaTime(DeltaTime);
	m_GlobalCBuffer->SetAccTime(m_GlobalAccTime);

	m_GlobalCBuffer->UpdateCBuffer();

	if (m_EditMode)
	{
		CIMGUIManager::GetInst()->Update(DeltaTime);
	}

	CResourceManager::GetInst()->Update();

	CInput::GetInst()->Update(DeltaTime);

	if (Update(DeltaTime))
		return;

	if (PostUpdate(DeltaTime))
		return;

	Render(DeltaTime);

	CInput::GetInst()->Clear();
}

void CEngine::Play()
{
	if (m_Play)
	{
		return;
	}

	m_Play = true;
	m_Pause = false;
}

void CEngine::Pause()
{
	if (m_Pause || !m_Play)
	{
		return;
	}

	m_Play = false;
	m_Pause = true;
}

void CEngine::Resume()
{
	if (!m_Pause || !m_Play)
	{
		return;
	}

	m_Play = true;
	m_Pause = false;
}

void CEngine::Stop()
{
	if (!m_Play)
	{
		return;
	}

	m_Play = false;
	m_Pause = false;
}

bool CEngine::Update(float DeltaTime)
{
	if (CSceneManager::GetInst()->Update(DeltaTime))
	{
		CIMGUIManager::GetInst()->Render();
		return true;
	}

	if (m_MouseWidget[(int)m_MouseState])
	{
		// ���콺 ��ġ�� ���´�.
		Vector2 Pos = CInput::GetInst()->GetMousePos();

		// ���콺�� ������ â�� ����ٸ� ���̰� �Ѵ�.
		if (Pos.x < 0.f || Pos.x > m_RS.Width || Pos.y < 0.f || Pos.y > m_RS.Height)
		{
			if (m_ShowCursorCount < 0)
			{
				ShowCursor(true);
				m_ShowCursorCount = 0;
			}
		}

		else
		{
			if (m_ShowCursorCount == 0)
			{
				ShowCursor(false);
				--m_ShowCursorCount;
			}
		}

		// Pos.y -= m_MouseWidget[(int)m_MouseState]->GetWindowSize().y;
		Pos.y = m_RS.Height - Pos.y;

		m_MouseWidget[(int)m_MouseState]->SetPos(Pos);

		m_MouseWidget[(int)m_MouseState]->Update(DeltaTime);
	}

	return false;
}

bool CEngine::PostUpdate(float DeltaTime)
{
	if (CSceneManager::GetInst()->PostUpdate(DeltaTime))
		return true;

	if (m_MouseWidget[(int)m_MouseState])
	{
		m_MouseWidget[(int)m_MouseState]->PostUpdate(DeltaTime);
	}

	return false;
}

bool CEngine::Render(float DeltaTime)
{
	CDevice::GetInst()->RenderStart();
	CDevice::GetInst()->ClearRenderTarget(m_ClearColor);
	CDevice::GetInst()->ClearDepthStencil(1.f, 0);

	CRenderManager::GetInst()->Render(DeltaTime);

	if (m_EditMode)
	{
		CIMGUIManager::GetInst()->Render();
	}

	CDevice::GetInst()->Flip();

	return true;
}

ATOM CEngine::Register(const TCHAR* Name, int IconID)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);


	wcex.style = CS_HREDRAW | CS_VREDRAW;


	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;


	wcex.hInstance = m_hInst;


	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IconID));


	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;

	wcex.lpszClassName = Name;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IconID));

	return RegisterClassExW(&wcex);
}

BOOL CEngine::Create(const TCHAR* Name)
{
	m_hWnd = CreateWindowW(Name, Name, WS_OVERLAPPEDWINDOW,
		0, 0, m_RS.Width, m_RS.Height, nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
	{
		return FALSE;
	}
	

	RECT    rc = { 0, 0, (LONG)m_RS.Width, (LONG)m_RS.Height };
	

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);


	SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, rc.right - rc.left,
		rc.bottom - rc.top,
		SWP_NOZORDER);


	ShowWindow(m_hWnd, SW_SHOW);


	UpdateWindow(m_hWnd);

	return TRUE;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 1;

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;


		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		short s = HIWORD(wParam);
		CInput::GetInst()->SetWheelDir(s / -120);
	}
		break;
	case WM_DESTROY:
		m_Loop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
