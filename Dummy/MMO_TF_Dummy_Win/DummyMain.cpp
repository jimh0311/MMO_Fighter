#include <winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdio>
#include "stSession.h"
#include "PacketDefine.h"
#include "Net_Lib\CProtoBuffer.h"
#include "Net_Lib\__Log.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm")
#define dfACTION_MOVE_LL 0
#define dfACTION_MOVE_LU 1
#define dfACTION_MOVE_UU 2
#define dfACTION_MOVE_RU 3
#define dfACTION_MOVE_RR 4
#define dfACTION_MOVE_RD 5
#define dfACTION_MOVE_DD 6
#define dfACTION_MOVE_LD 7

#define dfACTION_ATTACK1 8
#define dfACTION_ATTACK2 9
#define dfACTION_ATTACK3 10
#define dfACTION_STAND 11
#define dfACTION_MAX 12

#define dfMAX_LEFT	0
#define dfMAX_RIGHT 6400
#define dfMAX_TOP 0
#define dfMAX_BOTTOM 6400

#define dfNETWORK_PORT 5000
#define dfMAX_UPDATE 20

//-------------------------------------------------------
//  17.01.23
//	2000명을 MAX치로 잡고 달려보자.
//-------------------------------------------------------
stSession	arrSession[2000];
DWORD g_Time = 0;

int dfSESSION_MAX = 0;
int g_UpdateIdx = 0;
int g_Loop = 0;
int g_Attack = 0;
int g_Stand = 0;
int g_Move = 0;
int g_Connect = 0;
int g_Accept = 0;
int g_DisConn = 0;
//-------------------------------------------------------
//  17.01.25
//	
//-------------------------------------------------------
void DeadRecording(int iNow);
void NetworkProc();
void Update();
void RecvProc(int idx);
void SendProc(int idx);
void MakePacket(int iNow, DWORD dfACTION_MSG);
void GetCharic(int iNow, CProtoBuffer *pData);
void GetSync(int iNow, CProtoBuffer *pData);
void ConsolePrint();
//-------------------------------------------------------
//  17.01.25
//	메인 프로시져
//-------------------------------------------------------
int main()
{
	//	IP, 더미 숫자를 Console로 입력받는다.
	WCHAR	wsIP[32];
	wprintf(L"Server IP ? :");
	wscanf_s(L"%s", wsIP, 32);
	wprintf(L"Number of Dummy ? :");
	scanf_s("%d", &dfSESSION_MAX);

	//	화면을 지운다
	system("cls");
	//-------------------------------------------------------
	//  17.01.25	
	//	Listen 소켓을 찾아서
	//-------------------------------------------------------
	int i;
	WSADATA	wsaData;
	WSAStartup(0x0202, &wsaData);
	SOCKADDR_IN	addrLisn;
	memset(&addrLisn, 0, sizeof(addrLisn));
	addrLisn.sin_family = AF_INET;
	addrLisn.sin_port = htons(dfNETWORK_PORT);
	InetPton(AF_INET, wsIP, &addrLisn.sin_addr);

	//-------------------------------------------------------
	//  17.01.23
	//	다수의 Connect...
	//-------------------------------------------------------
	for (i = 0; i < dfSESSION_MAX; i++)
	{
		arrSession[i].Socket = socket(AF_INET, SOCK_STREAM, 0);
		connect(arrSession[i].Socket, (SOCKADDR*)&addrLisn, sizeof(addrLisn));

		arrSession[i].bConnect = FALSE;
	}
	//-------------------------------------------------------
	//  17.01.25
	//	Dummy Main Loop 입니다.
	//-------------------------------------------------------
	while (1)
	{
		NetworkProc();

		Update();

		ConsolePrint();

		g_Loop++;//	루프 카운트 체크
		Sleep(20);
	}

	//-------------------------------------------------------
	//  17.01.25
	//	WSACleanup() 을 돌려주세요.
	//-------------------------------------------------------
	WSACleanup();
	return 0;
}

void ConsolePrint()
{
	DWORD nowTick = timeGetTime();
	if (nowTick - g_Time < 3000)
		return;

	for (int i = 0; i < dfSESSION_MAX; i++)
	{
		if (arrSession[i].msg < dfACTION_ATTACK1)
			g_Move++;
		else if (arrSession[i].msg == dfACTION_ATTACK1 || arrSession[i].msg == dfACTION_ATTACK2 || arrSession[i].msg == dfACTION_ATTACK3)
			g_Attack++;
		else
			g_Stand++;

		if (arrSession[i].bConnect)
			g_Connect++;
		else
			g_DisConn++;
	}

	printf("[ Connect : %d / DisConn : %d / Loop : %d ]Move : %d / Attack : %d / Stand : %d \n",g_Connect, g_DisConn,g_Loop/3, g_Move, g_Attack, g_Stand);
	g_Move = g_Attack = g_Stand = g_Connect = g_DisConn = g_Loop = 0;
	g_Time = nowTick;
}

//-------------------------------------------------------
//  17.01.25
//	네트워크 처리
//-------------------------------------------------------
void NetworkProc()
{
	fd_set fdR, fdW;
	int iSelect;
	timeval selTimer;
	selTimer.tv_sec = 0;
	selTimer.tv_usec = 0;


	
	int iBig = 0;
	int iSmall = 0;
	int iStart = 0;
	int iCount = 0;
	while (iBig < dfSESSION_MAX)
	{
		FD_ZERO(&fdR);
		FD_ZERO(&fdW);
		iStart = iBig;
		iSmall = iBig;
		iCount = 0;
		while (iCount < 64)
		{
			if (iSmall >= dfSESSION_MAX)
				break;
			if (arrSession[iSmall].Socket != INVALID_SOCKET)
			{
				FD_SET(arrSession[iSmall].Socket, &fdR);
				FD_SET(arrSession[iSmall].Socket, &fdW);
			}
			iCount++;
			iSmall++;
		}
		iBig += iCount;

		iSelect = select(0, &fdR, &fdW, 0, &selTimer);

		if (iSelect > 0)
		{
			for (iStart ; iStart < iBig ; iStart++)
			{
				if (FD_ISSET(arrSession[iStart].Socket, &fdR))
				{
					RecvProc(iStart);
				}
				if (FD_ISSET(arrSession[iStart].Socket, &fdW))
				{
					SendProc(iStart);
				}
			}
		}
	}
}

//-------------------------------------------------------
//  17.01.25
//	리시브
//
//
//	다 버릴거지만, 최초접속하는 패킷만 채서 캐릭터를 부여한다.
//-------------------------------------------------------
void RecvProc(int iNow)
{
	int iRemainSize = 0;
	int iRecv = 0;
	iRemainSize = arrSession[iNow].rqR.GetNotBrokenPutSize();

	if (iRemainSize <= 0)
	{
		__Log(dfLOG_WARNING,L"ID: %d , Fulled Recv Queue\n", iNow);
		closesocket(arrSession[iNow].Socket);
		arrSession[iNow].bConnect = FALSE;
		arrSession[iNow].Socket = INVALID_SOCKET;
		return;
	}
	
	iRecv = recv(arrSession[iNow].Socket, arrSession[iNow].rqR.GetWriteBufferPtr(), iRemainSize, 0);
	if (iRecv == 0 || iRecv == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		__Log(dfLOG_WARNING, L"ID: %d, ERROR : %d , Shutdown From Server\n", iNow, iError);
		closesocket(arrSession[iNow].Socket);
		arrSession[iNow].bConnect = FALSE;
		arrSession[iNow].Socket = INVALID_SOCKET;
		return;
	}
	else if (iRecv > 0)
	{
			st_NETWORK_PACKET_HEADER Header;
			CProtoBuffer Data;
			arrSession[iNow].rqR.MoveWritePos(iRecv);
			while (1)
			{
				BYTE CheckEnd;
				Data.Clear();
				int iPeek = 0;

				// 리시브 큐에 데이터가 없다면, 브레이크
				if (arrSession[iNow].rqR.GetCurrentUsingSize() < sizeof(st_NETWORK_PACKET_HEADER))
					break;

				iPeek = arrSession[iNow].rqR.Peek((char*)&Header, sizeof(st_NETWORK_PACKET_HEADER));

				if (arrSession[iNow].rqR.GetCurrentUsingSize() < sizeof(st_NETWORK_PACKET_HEADER) + Header.bySize + 1)
					break;

				arrSession[iNow].rqR.RemoveData(sizeof(st_NETWORK_PACKET_HEADER));
				iPeek = arrSession[iNow].rqR.Peek(Data.GetBufferPtr(), Header.bySize); // 아님
				if (iPeek != Header.bySize)
				{
					__Log(dfLOG_WARNING, L"ID: %d , InCorrect iPeekSize \n", iNow);
					closesocket(arrSession[iNow].Socket);
					arrSession[iNow].bConnect = FALSE;
					arrSession[iNow].Socket = INVALID_SOCKET;
					return;
				}
				Data.MoveWritePos(iPeek);
				arrSession[iNow].rqR.RemoveData(iPeek);
				int iDeq = arrSession[iNow].rqR.Dequeue((char*)&CheckEnd, 1);

				if (CheckEnd != dfNETWORK_PACKET_END)
				{
					__Log(dfLOG_WARNING, L"ID: %d , InCorrect End-Code , iDeq = %d \n", iNow, iDeq);
					closesocket(arrSession[iNow].Socket);
					arrSession[iNow].bConnect = FALSE;
					arrSession[iNow].Socket = INVALID_SOCKET;
					return;
				}
				//-------------------------------------------------------
				//  17.01.25
				//	서버의 캐릭터를 받아오는 것.
				//-------------------------------------------------------
				if (Header.byType == dfPACKET_SC_CREATE_MY_CHARACTER)
				{
					GetCharic(iNow, &Data);
				}
				//-------------------------------------------------------
				//  17.01.25
				//	싱크도 처리해줘야함
				//-------------------------------------------------------
				if (Header.byType == dfPACKET_SC_SYNC)
				{
					GetSync(iNow, &Data);
				}

			}
	}
}
void SendProc(int iNow)
{
	//-------------------------------------------------------
	//  17.01.19
	//	정상적인 SendProc 진행
	//-------------------------------------------------------
	int iSend;
	int iDequeue;
	char SendBuffer[2000];
	while (arrSession[iNow].rqS.GetCurrentUsingSize() > 0)
	{
		iDequeue = arrSession[iNow].rqS.Dequeue(SendBuffer, 2000);
		if (iDequeue == 0)
			return;

		iSend = send(arrSession[iNow].Socket, SendBuffer, iDequeue, 0);
		if (iSend == 0 || iSend == SOCKET_ERROR)
		{
			int iError = WSAGetLastError();
			__Log(dfLOG_WARNING, L"Send Error( %d )", iError);
			closesocket(arrSession[iNow].Socket);
			arrSession[iNow].bConnect = FALSE;
			arrSession[iNow].Socket = INVALID_SOCKET;
			break;
		}
	}
}

//-------------------------------------------------------
//  17.01.25
//
//-------------------------------------------------------
void Update()
{
	// 기준점
	DWORD nowTick = timeGetTime();
	int Count = 0;
	int iRandomMsg = 0;
	//-------------------------------------------------------
	//  17.01.25
	//	한 루프에 20명씩만 Action을 처리해줄 것이다.
	//-------------------------------------------------------
	for (g_UpdateIdx; g_UpdateIdx < dfSESSION_MAX; g_UpdateIdx++)
	{
		if (Count > dfMAX_UPDATE)
			break;
		//-------------------------------------------------------
		//  17.01.25
		//	3000초 이상이 지났으면, 그 때 움직인다.
		//-------------------------------------------------------
		if ((nowTick - arrSession[g_UpdateIdx].dwActionTick > 3000) && arrSession[g_UpdateIdx].bConnect == TRUE)
		{
			iRandomMsg = rand() % dfACTION_MAX;
			MakePacket(g_UpdateIdx, iRandomMsg);
			Count++;
		}
	}

	if (g_UpdateIdx >= dfSESSION_MAX)
		g_UpdateIdx = 0;
}


void MakePacket(int iNow, DWORD dfACTION_MSG)
{
	st_NETWORK_PACKET_HEADER	stHeader;
	stHeader.byCode = 0x89;
	char	end = dfNETWORK_PACKET_END;
	CProtoBuffer Data;
	switch (dfACTION_MSG)
	{
	/////////////////////////////////////////////////////////////////
	//	움직이는 거 관련 패킷보내기...
	/////////////////////////////////////////////////////////////////
	case dfACTION_MOVE_LL:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LL;
		stMoveData.X =	arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅자료들
		arrSession[iNow].dir = dfACTION_MOVE_LL;
		arrSession[iNow].msg = dfACTION_MOVE_LL;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();
		break;
	}
	case dfACTION_MOVE_LU:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LU;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅자료들
		//
		arrSession[iNow].dir = dfACTION_MOVE_LL;
		arrSession[iNow].msg = dfACTION_MOVE_LU;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();

		break;
	}
	case dfACTION_MOVE_UU:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_UU;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;

		//셋팅자료들
		arrSession[iNow].msg = dfACTION_MOVE_UU;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();

		break;
	}
	case dfACTION_MOVE_RU:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RU;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		
		//셋팅
		arrSession[iNow].dir = dfACTION_MOVE_RR;
		arrSession[iNow].msg = dfACTION_MOVE_RU;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	case dfACTION_MOVE_RR:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RR;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅
		arrSession[iNow].dir = dfACTION_MOVE_RR;
		arrSession[iNow].msg = dfACTION_MOVE_RR;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	case dfACTION_MOVE_RD:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RD;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;

		//셋팅
		arrSession[iNow].dir = dfACTION_MOVE_RR;
		arrSession[iNow].msg = dfACTION_MOVE_RD;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	case dfACTION_MOVE_DD:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_DD;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅
		arrSession[iNow].msg = dfACTION_MOVE_DD;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	case dfACTION_MOVE_LD:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LD;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅
		arrSession[iNow].dir = dfACTION_MOVE_LL;
		arrSession[iNow].msg = dfACTION_MOVE_LD;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));
		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	/////////////////////////////////////////////////////////////////
	//	서있는거
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	//	때리는거
	/////////////////////////////////////////////////////////////////
	case dfACTION_ATTACK1:
	{
		DeadRecording(iNow);
		//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y

		stHeader.bySize = 5;
		stHeader.byType = dfPACKET_CS_ATTACK1;
		
		Data.Clear();
		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;

		arrSession[iNow].msg = dfACTION_ATTACK1;

		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());

		
		//	1	-	Direction	( 방향 디파인 값 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y
//		stHeader.bySize = 5;
//		stHeader.byType = dfPACKET_CS_MOVE_STOP;

//		Data.Clear();
//		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;
//
//		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
//		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());

		arrSession[iNow].dwActionTick = timeGetTime();
		break;
	}
	case dfACTION_ATTACK2:
	{
		DeadRecording(iNow);
		//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y

		stHeader.bySize = 5;
		stHeader.byType = dfPACKET_CS_ATTACK2;

		Data.Clear();
		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;

		arrSession[iNow].msg = dfACTION_ATTACK2;

		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());


		//	1	-	Direction	( 방향 디파인 값 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y
//		stHeader.bySize = 5;
//		stHeader.byType = dfPACKET_CS_MOVE_STOP;

//		Data.Clear();
//		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;

//		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
//		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());

		arrSession[iNow].dwActionTick = timeGetTime();
		break;
	}
	case dfACTION_ATTACK3:
	{
		DeadRecording(iNow);
		//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y

		stHeader.bySize = 5;
		stHeader.byType = dfPACKET_CS_ATTACK3;

		Data.Clear();
		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;

		arrSession[iNow].msg = dfACTION_ATTACK3;

		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());


		//	1	-	Direction	( 방향 디파인 값 좌/우만 사용 )
		//	2	-	X
		//	2	-	Y
//		stHeader.bySize = 5;
//		stHeader.byType = dfPACKET_CS_MOVE_STOP;

//		Data.Clear();
//		Data << arrSession[iNow].dir << arrSession[iNow].shX << arrSession[iNow].shY << end;

//		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
//		arrSession[iNow].rqS.Enqueue(Data.GetBufferPtr(), Data.GetDataSize());

		arrSession[iNow].dwActionTick = timeGetTime();
		break;
	}
	case dfACTION_STAND:
	{
		//데드레커닝
		DeadRecording(iNow);
		//패킷제작
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_STOP;
		stPACKET_CS_MOVE_START	stMoveData;
		// LEFT == 0 ; RIGHT == 1;
		stMoveData.Direction = arrSession[iNow].dir;
		stMoveData.X = arrSession[iNow].shX;
		stMoveData.Y = arrSession[iNow].shY;
		//셋팅
		arrSession[iNow].msg = dfACTION_STAND;
		//샌드큐에 넣기
		arrSession[iNow].rqS.Enqueue((char*)&stHeader, sizeof(stHeader));
		arrSession[iNow].rqS.Enqueue((char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		arrSession[iNow].rqS.Enqueue(&end, sizeof(char));

		//ActionTick 리셋팅
		arrSession[iNow].dwActionTick = timeGetTime();


		break;
	}
	

	break;
	}
}



//-------------------------------------------------------
//  17.01.25
//	내 캐릭터를 생성하는 부분
//
//	다른 리시브 패킷은 다 버리지만, 얘는 버리면 안된다.
//-------------------------------------------------------
void GetCharic(int iNow, CProtoBuffer *pData)
{
	//	4	-	ID
	//	1	-	Direction
	//	2	-	X
	//	2	-	Y
	//	1	-	HP
	DWORD SC_ID;
	BYTE SC_DIR, SC_HP;
	SHORT SC_X, SC_Y;

	//	직렬화버퍼에서 데이터를 뽑아온다.
	*pData >> SC_ID >> SC_DIR >> SC_X >> SC_Y >> SC_HP;

	//	셋팅한다.
	arrSession[iNow].dir	= dfACTION_MOVE_LL;
	arrSession[iNow].ID		= SC_ID;
	arrSession[iNow].shX	= SC_X;
	arrSession[iNow].shY	= SC_Y;
	arrSession[iNow].msg	= dfACTION_STAND;
	arrSession[iNow].dwActionTick = 0;
	//	캐릭터 생성완료.

	arrSession[iNow].bConnect = TRUE;
}
//-------------------------------------------------------
//  17.01.25
//	내 ID로 온 Sync만 처리해준다.
//-------------------------------------------------------
void GetSync(int iNow, CProtoBuffer *pData)
{
	//	4	-	ID
	//	2	-	X
	//	2	-	Y
	DWORD SC_ID;
	SHORT SC_X, SC_Y;
	*pData >> SC_ID >> SC_X >> SC_Y;

	//	나한테 온 Sync가 아니므로, 처리하지 않는다.
	if (arrSession[iNow].ID != SC_ID)
		return;

	arrSession[iNow].shX = SC_X;
	arrSession[iNow].shY = SC_Y;
}


//
//
///
///
//
//
void DeadRecording(int iNow)
{

	//-------------------------------------------------------
	//  17.01.25
	//	넘 단순하게 여기서 timeGetTime을 하고 있으니 에러가 생길 수 밖에 없지 않니?
	//-------------------------------------------------------

	DWORD dwIntervalTime = timeGetTime() - arrSession[iNow].dwActionTick;

	//-------------------------------------------------------
	//  17.01.25
	//	서버와 데드레커닝 프레임 차이좀 보고싶어서 넣음
	//-------------------------------------------------------

	int iActionFrame = dwIntervalTime / 20;	// 50 프레임 기준으로 가는 듯?
		
	//
	//
	//
	int iRemoveFrame = 0;

	int iRPosX = arrSession[iNow].shX;
	int iRPosY = arrSession[iNow].shY;

	int iValue = 0;

	int iDistanceX = iActionFrame * 3;
	int iDistanceY = iActionFrame * 2;
	switch (arrSession[iNow].msg)
	{
	case dfPACKET_MOVE_DIR_LL:
	{
		iRPosX -= iDistanceX;
		break;
	}
	case dfPACKET_MOVE_DIR_LD:
	{
		iRPosX -= iDistanceX;
		iRPosY += iDistanceY;
		break;
	}
	case dfPACKET_MOVE_DIR_LU:
	{
		iRPosX -= iDistanceX;
		iRPosY -= iDistanceY;
		break;
	}
	case dfPACKET_MOVE_DIR_RR:
	{
		iRPosX += iDistanceX;
		break;
	}
	case dfPACKET_MOVE_DIR_RD:
	{
		iRPosX += iDistanceX;
		iRPosY += iDistanceY;
		break;
	}
	case dfPACKET_MOVE_DIR_RU:
	{
		iRPosX += iDistanceX;
		iRPosY -= iDistanceY;
		break;
	}
	case dfPACKET_MOVE_DIR_DD:
	{
		iRPosY += iDistanceY;
		break;
	}
	case dfPACKET_MOVE_DIR_UU:
	{
		iRPosY -= iDistanceY;
		break;
	}
	default:
		break;
	}
	//
	//
	//

	if (iRPosX <= dfMAX_LEFT)
	{
		iValue = abs(dfMAX_LEFT - abs(iRPosX)) / 3;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosX >= dfMAX_RIGHT)
	{
		iValue = abs(dfMAX_RIGHT - iRPosX) / 3;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY <= dfMAX_TOP)
	{
		iValue = abs(dfMAX_TOP - abs(iRPosY)) / 2;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY >= dfMAX_BOTTOM)
	{
		iValue = abs(dfMAX_BOTTOM - iRPosY) / 2;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}

	//위에서 계산된 결과 삭제 되어야 할 프레임이 나타났다면 좌표를 다시 재 계산한다.
	if (iRemoveFrame > 0)
	{

		iActionFrame -= iRemoveFrame;
		//-------------------------------------------------------
		//  17.01.25
		//	서버와 데드레커닝 프레임 차이좀 보고싶어서 넣음
		//-------------------------------------------------------

		//보정된좌표 다시 계산
		iDistanceX = iActionFrame * 3;
		iDistanceY = iActionFrame * 2;

		iRPosX = arrSession[iNow].shX;
		iRPosY = arrSession[iNow].shY;

		switch (arrSession[iNow].msg)
		{
		case dfPACKET_MOVE_DIR_LL:
		{
			iRPosX -= iDistanceX;
			break;
		}
		case dfPACKET_MOVE_DIR_LD:
		{
			iRPosX -= iDistanceX;
			iRPosY += iDistanceY;
			break;
		}
		case dfPACKET_MOVE_DIR_LU:
		{
			iRPosX -= iDistanceX;
			iRPosY -= iDistanceY;
			break;
		}
		case dfPACKET_MOVE_DIR_RR:
		{
			iRPosX += iDistanceX;
			break;
		}
		case dfPACKET_MOVE_DIR_RD:
		{
			iRPosX += iDistanceX;
			iRPosY += iDistanceY;
			break;
		}
		case dfPACKET_MOVE_DIR_RU:
		{
			iRPosX += iDistanceX;
			iRPosY -= iDistanceY;
			break;
		}
		case dfPACKET_MOVE_DIR_DD:
		{
			iRPosY += iDistanceY;
			break;
		}
		case dfPACKET_MOVE_DIR_UU:
		{
			iRPosY -= iDistanceY;
			break;
		}
		}
	}

	iRPosX = min(iRPosX, dfMAX_RIGHT);
	iRPosX = max(iRPosX, dfMAX_LEFT);
	iRPosY = min(iRPosY, dfMAX_BOTTOM);
	iRPosY = max(iRPosY, dfMAX_TOP);

	arrSession[iNow].shX = iRPosX;
	arrSession[iNow].shY = iRPosY;

	return;
}