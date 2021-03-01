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
//WSAAsyncSelect 에러 무시...
#pragma warning	(disable:4996)

////////////////////////////////////////////////////////////////////////
//	정의....
////////////////////////////////////////////////////////////////////////

//WSAAsync 용 메세지
#define UM_NETWORK	(WM_USER+1)
#define dfIMAGE_TILE	69


////////////////////////////////////////////////////////////////////////
// 전역 변수:
////////////////////////////////////////////////////////////////////////
HINSTANCE	hInst;			// 현재 인스턴스입니다.
HWND		g_hWnd;			// 현재 윈도우 핸들.							
BOOL		g_bActiveApp;	// 현재 윈도우 상태. T = 활성화, F = 비활성화
HIMC		hIMC;
extern BOOL g_KeyProc;

CMap	g_Map(64,100,100);
///////////////////////////////////////////////////////////////////////
//	클라이언트 구동용 변수들...
///////////////////////////////////////////////////////////////////////
//FPS 출력을 위한 변수
DWORD		g_dwTime = timeGetTime();
DWORD		g_dwTempTime = 0;
int			g_iFrameRate = 0;
int			g_iDrawRate = 0;
WCHAR		g_ShowFrameRate[10];
WCHAR		g_ShowDrawRate[10];

//링크드리스트
myLinkedList<CBaseObject*> g_List;

//화면그리는 애들
CScreenDIB*	g_pScreen = CScreenDIB::GetInstance();
CSpriteDIB*	g_pSprite = CSpriteDIB::GetInstance();
//프레임조절
CFrameControl g_Frame(20);

		/////////////////////////////////////////////////////////////
		//캐릭터
		/////////////////////////////////////////////////////////////
		CPlayerObject*	g_Player;
	
///////////////////////////////////////////////////////////////////////
//	네트워크용 변수들...
///////////////////////////////////////////////////////////////////////
BOOL		g_bSendAble = FALSE;
WSADATA		wsaData;
SOCKET		hServSock;
SOCKADDR_IN	servAddr;
//DIALOG 문자열을 받아올 변수	 = 소켓의 주소로 변환 시킬 것
WCHAR		g_wDialogString[32];


/////////////////////////////////////////////////////////////////////
//	함수 프로시저
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
//	WINMAIN 시작
////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


	//
	//  목적: 창 클래스를 등록합니다.
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
	//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
	//
	//   설명:
	//
	//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
	//        주 프로그램 창을 만든 다음 표시합니다.
	//

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.


	///////////////////////////////////////////////////////////////
	//	IP주소를 입력받을 DIALOG 출력
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
	//	네트워크 연결 셋팅...
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
	//	AsyncSelect 설정...
	WSAAsyncSelect(hServSock, g_hWnd, UM_NETWORK, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);

	

	/////////////////////////////////////////////////////////////////////////
	//	Windows 사이즈 및 기타 설정 부분...
	/////////////////////////////////////////////////////////////////////////
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	SetFocus(g_hWnd);
	g_bActiveApp = TRUE;
	//	클라이언트 사이즈 조정

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	//	계산해서 뱉어줌

	AdjustWindowRectEx(
		&WindowRect,
		GetWindowStyle(g_hWnd),
		GetMenu(g_hWnd) != NULL,
		GetWindowExStyle(g_hWnd)
		);
	
	int iX = (GetSystemMetrics(SM_CXSCREEN) - 640) / 2;
	int iY = (GetSystemMetrics(SM_CYSCREEN) - 480) / 2;

	//	가운데로 이동 및 화면 크기 리셋팅

	MoveWindow(
		g_hWnd,
		iX,
		iY,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		TRUE
	);

	///////////////////////////////////////////////////////////////////////////
	//	게임 로직을 위한 셋팅
	///////////////////////////////////////////////////////////////////////////

	GameInit();

	///////////////////////////////////////////////////////////////////////////
	//	목적 : 없음 죽어랏 ㅋ
	///////////////////////////////////////////////////////////////////////////

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PUNISHERACTIONGAME));

    MSG msg;

	//////////////////////////////////////////////////////////////
	//	GAME 구동을 위한 WinMain 메세지 루프
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
			// GAME LOGIC 이 선언되면 된당;

			Update();
		}
	}
	closesocket(hServSock);
	WSACleanup();
    return (int) msg.wParam;
}

///////////////////////////////////////////////////////////////////////////
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//	
///////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
    switch (message)
    {
	/////////////////////////////////////////////////////////////////////////
	//	네트워크 메세지 처리부분
	/////////////////////////////////////////////////////////////////////////
	case UM_NETWORK:
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
			case FD_CONNECT:
			{
				//	최초에 커넥트되어 받아오는 캐릭터가 내 캐릭터이지 않은가?
				//	아니다 패킷에 들어오는 애가 최초이다...
				g_bSendAble = TRUE;
				//////////////////////////////////////////////////////////////
				//	테스트용 더미 캐릭터
				//////////////////////////////////////////////////////////////
			//	for (int i = 1; i < 5; i++)
			//	{
			//	CPlayerObject *temp = new CPlayerObject(i, enPLAYER, rand()%600, rand()%400 , false);
			//	g_List.InsertHead(temp);
			//	}
				break;
			}
			//패킷 수신부...
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
	//	영문자 키 뜨는거 방지하는 부분
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
	//	딱히 중요할 거 없는 부분...
	/////////////////////////////////////////////////////////////////////////
	case WM_ACTIVATEAPP:
		{
			g_bActiveApp = (BOOL)wParam;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

// 정보 대화 상자의 메시지 처리기입니다.
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
//						건전한 나만의 함수들
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
	
	// Tile Image 추가...
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
	//액션부분
	OpenProfiler("ACTION");
	{
		myIterator<CBaseObject*> iter(&g_List);
		CBaseObject*	data;
		while (iter.GotoNext())
		{
			if (iter.GetValue(&data))
			{
				// return false 이면
				if (!data->Action())
				{
					if (data->GetID() == g_Player->GetID())
					{
						MessageBox(g_hWnd, L"너 죽었음...", L"SYSTEM", MB_OK);
					}
					iter.DeleteThisNode();
				}
			}
		}
	}
	//링크드리스트 정렬;
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
//	함수: KeyProc()
//	목적: 키보드입력을 받아 해당 메세지를 객체에게 전달한다.
//
void KeyProc()
{
	DWORD msg = dfACTION_STAND;
	//상하좌우
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
	//대각선
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
	//공격키
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
	//마지막 객체에게 전송한다.
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
// FPS를 띄워줄 함수
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