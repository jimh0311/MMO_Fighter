#include "stdafx.h"
#include "PunisherActionGame.h"
#include "Profiler.h"
#include "Resource.h"
#include "CStreamRQ.h"
#include "PacketDefine.h"
#include "NetworkProc.h"
#include <timeapi.h>
#include "CMap.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib,"imm32.lib")
#pragma comment (lib,"Winmm.lib")
//WSAAsyncSelect ���� ����...
#pragma warning	(disable:4996)

////////////////////////////////////////////////////////////////////////
//	����....
////////////////////////////////////////////////////////////////////////

//WSAAsync �� �޼���
#define UM_NETWORK	(WM_USER+1)
#define dfIMAGE_TILE	69


////////////////////////////////////////////////////////////////////////
// ���� ����:
////////////////////////////////////////////////////////////////////////
HINSTANCE	hInst;			// ���� �ν��Ͻ��Դϴ�.
HWND		g_hWnd;			// ���� ������ �ڵ�.							
BOOL		g_bActiveApp;	// ���� ������ ����. T = Ȱ��ȭ, F = ��Ȱ��ȭ
HIMC		hIMC;
extern BOOL g_KeyProc;

CMap	g_Map(64,100,100);
///////////////////////////////////////////////////////////////////////
//	Ŭ���̾�Ʈ ������ ������...
///////////////////////////////////////////////////////////////////////
//FPS ����� ���� ����
DWORD		g_dwTime = timeGetTime();
DWORD		g_dwTempTime = 0;
int			g_iFrameRate = 0;
int			g_iDrawRate = 0;
WCHAR		g_ShowFrameRate[10];
WCHAR		g_ShowDrawRate[10];

//��ũ�帮��Ʈ
myLinkedList<CBaseObject*> g_List;

//ȭ��׸��� �ֵ�
CScreenDIB*	g_pScreen = CScreenDIB::GetInstance();
CSpriteDIB*	g_pSprite = CSpriteDIB::GetInstance();
//����������
CFrameControl g_Frame(20);

		/////////////////////////////////////////////////////////////
		//ĳ����
		/////////////////////////////////////////////////////////////
		CPlayerObject*	g_Player;
	
///////////////////////////////////////////////////////////////////////
//	��Ʈ��ũ�� ������...
///////////////////////////////////////////////////////////////////////
BOOL		g_bSendAble = FALSE;
WSADATA		wsaData;
SOCKET		hServSock;
SOCKADDR_IN	servAddr;
//DIALOG ���ڿ��� �޾ƿ� ����	 = ������ �ּҷ� ��ȯ ��ų ��
WCHAR		g_wDialogString[32];


/////////////////////////////////////////////////////////////////////
//	�Լ� ���ν���
/////////////////////////////////////////////////////////////////////
INT_PTR	CALLBACK	DialogProc(HWND , UINT , WPARAM , LPARAM );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				Update();
void				GameInit();
void				KeyProc();
void				Sort();
void				ShowFrame();
////////////////////////////////////////////////////////////////////////
//	WINMAIN ����
////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


	//
	//  ����: â Ŭ������ ����մϴ�.
	//

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PUNISHERACTIONGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PUNISHERACTIONGAME);
	wcex.lpszClassName = L"CLASS";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

   
	
	
	
	//
	//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
	//
	//   ����:
	//
	//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
	//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
	//

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.


	///////////////////////////////////////////////////////////////
	//	IP�ּҸ� �Է¹��� DIALOG ���
	///////////////////////////////////////////////////////////////



	//g_hWnd = CreateWindowW(L"PUNISHER!", L"PUNISHER!", WS_OVERLAPPEDWINDOW,
	//	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	g_hWnd = CreateWindowEx(
		0,
		L"CLASS",
		L"PUNISHER!",
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	int iDlgReturn = DialogBox(hInstance, MAKEINTRESOURCE(IDD_NAME), NULL, DialogProc);
	//////////////////////////////////////////////////////////////
	//	��Ʈ��ũ ���� ����...
	//////////////////////////////////////////////////////////////
	WSAStartup(0x0202, &wsaData);
	hServSock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(5000);
	InetPton(AF_INET, g_wDialogString, &servAddr.sin_addr);

	//bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr));

	if (connect(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			PostQuitMessage(0);
		}
	}
	//	AsyncSelect ����...
	WSAAsyncSelect(hServSock, g_hWnd, UM_NETWORK, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);

	

	/////////////////////////////////////////////////////////////////////////
	//	Windows ������ �� ��Ÿ ���� �κ�...
	/////////////////////////////////////////////////////////////////////////
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	SetFocus(g_hWnd);
	g_bActiveApp = TRUE;
	//	Ŭ���̾�Ʈ ������ ����

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	//	����ؼ� �����

	AdjustWindowRectEx(
		&WindowRect,
		GetWindowStyle(g_hWnd),
		GetMenu(g_hWnd) != NULL,
		GetWindowExStyle(g_hWnd)
		);
	
	int iX = (GetSystemMetrics(SM_CXSCREEN) - 640) / 2;
	int iY = (GetSystemMetrics(SM_CYSCREEN) - 480) / 2;

	//	����� �̵� �� ȭ�� ũ�� ������

	MoveWindow(
		g_hWnd,
		iX,
		iY,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		TRUE
	);

	///////////////////////////////////////////////////////////////////////////
	//	���� ������ ���� ����
	///////////////////////////////////////////////////////////////////////////

	GameInit();

	///////////////////////////////////////////////////////////////////////////
	//	���� : ���� �׾�� ��
	///////////////////////////////////////////////////////////////////////////

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PUNISHERACTIONGAME));

    MSG msg;

	//////////////////////////////////////////////////////////////
	//	GAME ������ ���� WinMain �޼��� ����
	//////////////////////////////////////////////////////////////
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			Update();
		}
		else
		{
			// GAME LOGIC �� ����Ǹ� �ȴ�;

			Update();
		}
	}
	closesocket(hServSock);
	WSACleanup();
    return (int) msg.wParam;
}

///////////////////////////////////////////////////////////////////////////
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//	
///////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
    switch (message)
    {
	/////////////////////////////////////////////////////////////////////////
	//	��Ʈ��ũ �޼��� ó���κ�
	/////////////////////////////////////////////////////////////////////////
	case UM_NETWORK:
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
			case FD_CONNECT:
			{
				//	���ʿ� Ŀ��Ʈ�Ǿ� �޾ƿ��� ĳ���Ͱ� �� ĳ�������� ������?
				//	�ƴϴ� ��Ŷ�� ������ �ְ� �����̴�...
				g_bSendAble = TRUE;
				//////////////////////////////////////////////////////////////
				//	�׽�Ʈ�� ���� ĳ����
				//////////////////////////////////////////////////////////////
			//	for (int i = 1; i < 5; i++)
			//	{
			//	CPlayerObject *temp = new CPlayerObject(i, enPLAYER, rand()%600, rand()%400 , false);
			//	g_List.InsertHead(temp);
			//	}
				break;
			}
			//��Ŷ ���ź�...
			case FD_READ: 
			case FD_CLOSE:
			{
				RecvProc(hServSock);
				break;
			}
		}
		
		break;
	}
	/////////////////////////////////////////////////////////////////////////
	//	������ Ű �ߴ°� �����ϴ� �κ�
	/////////////////////////////////////////////////////////////////////////
	case WM_IME_KEYDOWN:
	case WM_IME_NOTIFY:
		{
			if (hIMC != NULL)
				return 0;

			hIMC = ImmGetContext(g_hWnd);

			ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
			ImmReleaseContext(g_hWnd, hIMC);
			hIMC = NULL;
			return 0;
		}
	/////////////////////////////////////////////////////////////////////////
	//	���� �߿��� �� ���� �κ�...
	/////////////////////////////////////////////////////////////////////////
	case WM_ACTIVATEAPP:
		{
			g_bActiveApp = (BOOL)wParam;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
		return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}






//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//						������ ������ �Լ���
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void GameInit()
{


	InitProfiler();
	g_pSprite->LoadDIBSprite(0, L"_SpriteData\\_Map.bmp", 0, 0); 

	g_pSprite->LoadDIBSprite(1, L"_SpriteData\\Stand_L_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(2, L"_SpriteData\\Stand_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(3, L"_SpriteData\\Stand_L_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(4, L"_SpriteData\\Stand_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(5, L"_SpriteData\\Stand_L_01.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(6, L"_SpriteData\\Stand_R_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(7, L"_SpriteData\\Stand_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(8, L"_SpriteData\\Stand_R_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(9, L"_SpriteData\\Stand_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(10, L"_SpriteData\\Stand_R_01.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(11, L"_SpriteData\\Move_L_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(12, L"_SpriteData\\Move_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(13, L"_SpriteData\\Move_L_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(14, L"_SpriteData\\Move_L_04.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(15, L"_SpriteData\\Move_L_05.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(16, L"_SpriteData\\Move_L_06.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(17, L"_SpriteData\\Move_L_07.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(18, L"_SpriteData\\Move_L_08.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(19, L"_SpriteData\\Move_L_09.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(20, L"_SpriteData\\Move_L_10.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(21, L"_SpriteData\\Move_L_11.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(22, L"_SpriteData\\Move_L_12.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(23, L"_SpriteData\\Move_R_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(24, L"_SpriteData\\Move_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(25, L"_SpriteData\\Move_R_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(26, L"_SpriteData\\Move_R_04.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(27, L"_SpriteData\\Move_R_05.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(28, L"_SpriteData\\Move_R_06.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(29, L"_SpriteData\\Move_R_07.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(30, L"_SpriteData\\Move_R_08.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(31, L"_SpriteData\\Move_R_09.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(32, L"_SpriteData\\Move_R_10.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(33, L"_SpriteData\\Move_R_11.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(34, L"_SpriteData\\Move_R_12.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(35, L"_SpriteData\\Attack1_L_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(36, L"_SpriteData\\Attack1_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(37, L"_SpriteData\\Attack1_L_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(38, L"_SpriteData\\Attack1_L_04.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(39, L"_SpriteData\\Attack1_R_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(40, L"_SpriteData\\Attack1_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(41, L"_SpriteData\\Attack1_R_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(42, L"_SpriteData\\Attack1_R_04.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(43, L"_SpriteData\\Attack2_L_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(44, L"_SpriteData\\Attack2_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(45, L"_SpriteData\\Attack2_L_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(46, L"_SpriteData\\Attack2_L_04.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(47, L"_SpriteData\\Attack2_R_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(48, L"_SpriteData\\Attack2_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(49, L"_SpriteData\\Attack2_R_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(50, L"_SpriteData\\Attack2_R_04.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(51, L"_SpriteData\\Attack3_L_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(52, L"_SpriteData\\Attack3_L_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(53, L"_SpriteData\\Attack3_L_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(54, L"_SpriteData\\Attack3_L_04.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(55, L"_SpriteData\\Attack3_L_05.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(56, L"_SpriteData\\Attack3_L_06.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(57, L"_SpriteData\\Attack3_R_01.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(58, L"_SpriteData\\Attack3_R_02.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(59, L"_SpriteData\\Attack3_R_03.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(60, L"_SpriteData\\Attack3_R_04.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(61, L"_SpriteData\\Attack3_R_05.bmp", 71, 90); 
	g_pSprite->LoadDIBSprite(62, L"_SpriteData\\Attack3_R_06.bmp", 71, 90); 

	g_pSprite->LoadDIBSprite(63, L"_SpriteData\\xSpark_1.bmp", 70, 70); 
	g_pSprite->LoadDIBSprite(64, L"_SpriteData\\xSpark_2.bmp", 70, 70); 
	g_pSprite->LoadDIBSprite(65, L"_SpriteData\\xSpark_3.bmp", 70, 70); 
	g_pSprite->LoadDIBSprite(66, L"_SpriteData\\xSpark_4.bmp", 70, 70); 
	g_pSprite->LoadDIBSprite(67, L"_SpriteData\\HPGuage.bmp", 0, 0); 
	g_pSprite->LoadDIBSprite(68, L"_SpriteData\\Shadow.bmp", 32, 4); 
	
	// Tile Image �߰�...
	g_pSprite->LoadDIBSprite(dfIMAGE_TILE, L"_SpriteData\\Tile_01.bmp", 0, 0);
	
}
void Update()
{
	BYTE* bypBuffer = g_pScreen->GetDIBBuffer();

	int iDestHeight = g_pScreen->GetHeigt();
	int iDestPitch = g_pScreen->GetPitch();
	int iDestWidth = g_pScreen->GetWidth();
	

	OpenProfiler("MAIN");

	if (g_bActiveApp && g_KeyProc)
	{
		KeyProc();
	}
	//�׼Ǻκ�
	OpenProfiler("ACTION");
	{
		myIterator<CBaseObject*> iter(&g_List);
		CBaseObject*	data;
		while (iter.GotoNext())
		{
			if (iter.GetValue(&data))
			{
				// return false �̸�
				if (!data->Action())
				{
					if (data->GetID() == g_Player->GetID())
					{
						MessageBox(g_hWnd, L"�� �׾���...", L"SYSTEM", MB_OK);
					}
					iter.DeleteThisNode();
				}
			}
		}
	}
	//��ũ�帮��Ʈ ����;
	//OpenProfiler("SORT");
	Sort();
	//EndProfiler("SORT");
	EndProfiler("ACTION");

	OpenProfiler("DRAW");
	
	if (g_Frame.FrameCheck())
	{
		{
			g_iDrawRate++;

			
			int DrawX = 0, DrawY = 0;
			g_Map.GetViewPoint(&DrawY, &DrawX);

			if (DrawX <= 0)
				DrawX = 0;
			else if (DrawX >= (6400 - 640))
				DrawX = 0;
			else
				DrawX = -g_Player->GetPosX()%64;

			if (DrawY <= 0)
				DrawY = 0;
			else if (DrawY >= (6400 - 512))
				DrawY = 0;
			else
				DrawY = -g_Player->GetPosY()%64;
			

			for (int H = 0; H < 9; H++)
			{
				for (int W = 0; W < 11; W++)
				{
					g_pSprite->DrawSprite(dfIMAGE_TILE, DrawX +(W*64), DrawY + (H*64), bypBuffer, iDestWidth, iDestHeight, iDestPitch);
				}
			}
			
			myIterator<CBaseObject*> iter(&g_List);
			CBaseObject*	data;
			while (iter.GotoNext())
			{
				if (iter.GetValue(&data))
				{
					data->Draw();
				}
			}
			
		}
		//Filp()
		{
			g_pScreen->DrawBuffer(g_hWnd);
			
		}
	}

	EndProfiler("DRAW");
	EndProfiler("MAIN");
	SaveProfiler();

	g_iFrameRate++;
	ShowFrame();
}
//
//	�Լ�: KeyProc()
//	����: Ű�����Է��� �޾� �ش� �޼����� ��ü���� �����Ѵ�.
//
void KeyProc()
{
	DWORD msg = dfACTION_STAND;
	//�����¿�
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		msg = dfACTION_MOVE_LL;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		msg = dfACTION_MOVE_RR;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		msg = dfACTION_MOVE_DD;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		msg = dfACTION_MOVE_UU;
	}
	//�밢��
	if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_UP) & 0x8000)
	{
		msg = dfACTION_MOVE_LU;
	}
	if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		msg = dfACTION_MOVE_LD;
	}
	if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_UP) & 0x8000)
	{
		msg = dfACTION_MOVE_RU;
	}
	if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		msg = dfACTION_MOVE_RD;
	}
	//����Ű
	if (GetAsyncKeyState(0x5A) & 0x8000)//z
	{
		msg = dfACTION_ATTACK1;
	}
	if (GetAsyncKeyState(0x58) & 0x8000)	// x
	{
		msg = dfACTION_ATTACK2;
	}
	if (GetAsyncKeyState(0x43) & 0x8000)	//c
	{
		msg = dfACTION_ATTACK3;
	}
	//������ ��ü���� �����Ѵ�.
	if (g_Player != nullptr)
	{
		g_Player->SendActionMsg(msg);
	}
	

}

void Sort()
{
	myIterator<CBaseObject*> Loop(&g_List);
	myIterator<CBaseObject*> Iter(&g_List);

	while (Loop.GotoNext())
	{
		Iter.GotoHead();
		while (Iter.GotoNext())
		{
			CBaseObject *temp1, *temp2;
			Iter.GetValue(&temp1);
			if (Iter.GotoNext())
			{
				if (Iter.GetValue(&temp2))
				{
					if (temp1->GetPosY() > temp2->GetPosY())
					{
						Iter.Swap();
					}
					Iter.GotoPrev();
				}
				else
				{
					return;
				}
			}
			else
			{
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////
// FPS�� ����� �Լ�
//////////////////////////////////////////////////////////
void ShowFrame()
{
	
	HDC hdc = GetDC(g_hWnd);
	//SetBkMode(hdc,TRANSPARENT);
	
	g_dwTempTime = timeGetTime();
	if ( g_dwTempTime- g_dwTime > 1000)
	{
		g_dwTime = g_dwTempTime;
		_itow_s(g_iFrameRate, g_ShowFrameRate, 10, 10);
		_itow_s(g_iDrawRate, g_ShowDrawRate, 10, 10);
		
		g_iFrameRate = 0;
		g_iDrawRate = 0;
	}
	//TextOut(hdc, 0, 460, g_ShowFrameRate, wcslen(g_ShowFrameRate));
	//TextOut(hdc, 30, 460, g_ShowDrawRate, wcslen(g_ShowDrawRate));

	//if (g_Player != NULL)
	//{
	//	WCHAR Text[32];
	//	wsprintf(Text, L"H:%d, W:%d", g_Player->GetPosY(), g_Player->GetPosX());
	//	TextOut(hdc, 0, 0, Text, wcslen(Text));
	//}
	//if (g_wDialogString != NULL)
	//{
	//	int x = g_Player->GetPosX();
	//	int y = g_Player->GetPosY();
	//	SetTextAlign(hdc, TA_CENTER);
	//	TextOut(hdc, x, y +20, g_wDialogString, wcslen(g_wDialogString));
	//	SetTextAlign(hdc, TA_LEFT);
	//}
	//SetBkMode(hdc, OPAQUE);
	ReleaseDC(g_hWnd, hdc);
}



INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit;
	switch (message)
	{
		
	case WM_INITDIALOG:
		hEdit = GetDlgItem(hWnd, IDC_EDIT1);
		SetWindowText(hEdit, L"127.0.0.1");
		return TRUE;
	case WM_COMMAND:
		{
		switch (wParam)
			{
		case IDOK:
			GetDlgItemText(hWnd, IDC_EDIT1, g_wDialogString, 32);
			EndDialog(hWnd, 9939);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, 9938);
			return TRUE;
		default:
			return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}