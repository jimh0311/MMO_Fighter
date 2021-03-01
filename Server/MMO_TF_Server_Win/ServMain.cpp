#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <list>
#include "Global_Struct.h"
#include "__Log.h"
#include "NetworkProc.h"
#include "ClientAction.h"

#pragma comment(lib,"winmm")
#pragma comment(lib,"ws2_32")
using namespace std;

#define dfSCREEN_LEFT 0
#define dfSCREEN_RIGHT 6400
#define dfSCREEN_TOP 0
#define dfSCREEN_BOTTOM 6400

//-------------------------------------------------------
//  17.01.18
//	��¿� �������̶���
//-------------------------------------------------------
DWORD g_Latancy = 0;
DWORD g_SendCount = 0;
DWORD g_RecvCount = 0;
DWORD g_ClientNumber = 0;
DWORD g_AcceptCount = 0;
DWORD g_Frame = 0;
DWORD g_Loop = 0;
DWORD g_Time = 0;
DWORD g_UpdateTick = 0;
DWORD g_FrameMAX = 0;
//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
WSADATA g_wsaData;			//	��Ʈ��ũ Listen Socket ������
SOCKET	g_hLisnSock;
SOCKADDR_IN g_addrLisn;
int g_SizeLisn;

DWORD g_ClientID = 7;	// AccountNo �ο���, ��л� 7��

list<stClient*> l_Client;	//	Ŭ���̾�Ʈ ���� ���� ����Ʈ


//	2���� �迭�� ����� ����ϴµ� �ʹ� ���ŷӴ�.
//stSector g_Sector[dfSECTOR_MAX][dfSECTOR_MAX];

// 200x200 �������� ����,
// 32.32 ��
stSector g_Sector[dfSECTOR_HEIGHT][dfSECTOR_WIDTH];

#define dfNETWORK_PORT 5000
//-------------------------------------------------------
//  17.01.18
//	void PrintConsole;
//-------------------------------------------------------
void PrintConsole();
//-------------------------------------------------------
//  17.01.18
//	��������
//-------------------------------------------------------
void main()
{
	timeBeginPeriod(1);
	//-------------------------------------------------------
	//  17.01.19
	//	���Ϳ� Top, Left ������ �༭ ������ ���� ������ �����غ���.
	//
	//	17.01.23
	//	���� 2�����迭�� �ϴ� ���� Around ���͸� ã�Ƴ��� �� ������ ���Ƽ� ������.
	//-------------------------------------------------------
	int Left = 0;
	int Top = 0;
	
	for (int i = 0; i < dfSECTOR_HEIGHT; i++)
	{
		for (int j = 0; j < dfSECTOR_WIDTH; j++)
		{
			g_Sector[i][j]._iLeft = Left;
			g_Sector[i][j]._iTop = Top;
			g_Sector[i][j]._iH = i;
			g_Sector[i][j]._iW = j;
			Left += 200;
		}
		Top += 200;
		Left = 0;
	}
	//-------------------------------------------------------
	//  17.01.18
	//	��Ʈ��ũ �����۾�
	//-------------------------------------------------------
	WSAStartup(0x0202, &g_wsaData);
	g_hLisnSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_hLisnSock == INVALID_SOCKET)
		return;

	memset(&g_addrLisn, 0, sizeof(SOCKADDR_IN));
	g_addrLisn.sin_family = AF_INET;

	//	PORT �ع���
	g_addrLisn.sin_port = htons(dfNETWORK_PORT);

	InetPton(AF_INET, L"0.0.0.0", &g_addrLisn.sin_addr);

	if (SOCKET_ERROR == bind(g_hLisnSock, (SOCKADDR*)&g_addrLisn, sizeof(SOCKADDR_IN)))
		return;
	if (SOCKET_ERROR == listen(g_hLisnSock, SOMAXCONN))
		return;

	__Log(dfLOG_SYSTEM, L"Server Start Port : %d ", dfNETWORK_PORT);
	//-------------------------------------------------------
	//  17.01.18
	//	���� ���� ���μ���
	//-------------------------------------------------------
	while (1)
	{
		
		//Recv Send Packet ó�� ���
		NetworkProc();
		//ĳ���͵� �����̱�
		Update();
		//�ʴ� ���� ���� ���
		PrintConsole();

		//Test��
		g_Loop++;

		if (GetAsyncKeyState(VK_NUMPAD9)&0x8000 && GetAsyncKeyState(VK_NUMPAD1)&0x8000)
			break;
	}

	//-------------------------------------------------------
	//  17.01.18
	//	�����ƾ
	//-------------------------------------------------------
	list<stClient*>::iterator ITER = l_Client.begin();
	for (ITER; ITER != l_Client.end(); ++ITER)
	{
		closesocket((*ITER)->hSocket);
	}
	//
	closesocket(g_hLisnSock);
	WSACleanup();
	//
	__Log(dfLOG_SYSTEM, L"Server Off");
	timeEndPeriod(1);
	return;
}
//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void PrintConsole()
{
	DWORD nowTick = GetTickCount();
	static DWORD Count = 0;
	DWORD H = Count / 3600;
	DWORD W = Count / 60;
	DWORD S = Count % 60;
	if (nowTick - g_Time > 1000)
	{
		Count++;
		printf("----------------------------------------------\n");
		printf("  [#.%02d:%02d:%02d] NUM0fCLNT[%zd] AccPS[%d] RvPS[%d] SdPS[%d] FPS[%d/%d] LPS[%d]\n", H,W,S, l_Client.size(), g_AcceptCount, g_RecvCount, g_SendCount, g_Frame, g_FrameMAX, g_Loop);
		printf("----------------------------------------------\n");
	//	list<stClient*>::iterator ITER = l_Client.begin();
	//	for (ITER; ITER != l_Client.end(); ++ITER)
	//	{
	//		stClient* pNow = *ITER;
	//		printf("USER%d Y:%d X:%d SECTOR:%d \n", pNow->ID, pNow->Y, pNow->X, pNow->pNow_Sector->_Index);
	//	}
	//	printf("----------------------------------------------\n");
		
					//-------------------------------------------------------
					//  17.01.23
					//	�α� ��°� �ʹ� ����Ǵϱ�, �������� 24�̸��̰ų�, g_Loop�� 1000 �̸��� ��쿡�� �ش� �α׸� ��´�.
					//-------------------------------------------------------
					if(g_Frame < 24 || g_Loop < 800)
					__Log(dfLOG_NOTPRINT, L"NUM0fCLNT[%d] AccPS[%d] RvPS[%d] SdPS[%d] FPS[%d/%d] LPS[%d]", l_Client.size(), g_AcceptCount, g_RecvCount, g_SendCount, g_Frame, g_FrameMAX, g_Loop);

					//
					//
					//
		g_FrameMAX = 0;
		g_AcceptCount = 0;
		g_RecvCount = 0;
		g_SendCount = 0;
		g_Frame = 0;
		g_Loop = 0;

		g_Time = nowTick;
	}
}