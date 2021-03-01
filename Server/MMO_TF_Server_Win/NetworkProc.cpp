#include "NetworkProc.h"
#include "PacketDefine.h"
#include "PacketProc.h"



extern SOCKET	g_hLisnSock;
extern DWORD g_ClientID;	// AccountNo 부여용, 기분상 7로

extern list<stClient*> l_Client;	//	클라이언트 세션 관리 리스트
extern stSector g_Sector[dfSECTOR_HEIGHT][dfSECTOR_WIDTH];
typedef list<stClient*>::iterator ITER_CLIENT;

#define dfRECV_TICK_MAX	30000

//-------------------------------------------------------
//  17.01.18
//	출력용 변수들이랑게
//-------------------------------------------------------
extern DWORD g_Latancy ;
extern DWORD g_SendCount ;
extern DWORD g_RecvCount ;
extern DWORD g_ClientNumber ;
extern DWORD g_AcceptCount ;
extern DWORD g_Frame ;
extern DWORD g_Loop ;
extern DWORD g_Time ;
//-------------------------------------------------------
//  17.01.18
//	네트워크 프록시져, 리시브를 받고, 샌드를 할 것이다.
//-------------------------------------------------------
void NetworkProc()
{
	fd_set fdR, fdW;	//	64개씩 셀렉트할 용도.
	timeval selTime;
	selTime.tv_sec = 0;
	selTime.tv_usec = 0;

		// 전체를 컨트롤할 Iterator 선택
		list<stClient*>::iterator Iter_Client = l_Client.begin();
		while (1)
		{
			// 초기화
			FD_ZERO(&fdR);
			FD_ZERO(&fdW);

			FD_SET(g_hLisnSock, &fdR);
			int iReadSet = 1;
			int iWriteSet = 1;

			list<stClient*>::iterator Iter_Recv = Iter_Client;

			// 64 개의 처리를 위해서
			for (Iter_Client; Iter_Client != l_Client.end(); ++Iter_Client)
			{
				FD_SET((*Iter_Client)->hSocket, &fdR);
				FD_SET((*Iter_Client)->hSocket, &fdW);

				++iReadSet;
				++iWriteSet;

				if (iReadSet >= dfSELECT_LIMIT)
					break;
			}
			//-------------------------------------------------------
			//  17.01.18
			//	Select 처리를 해줄 것이다.
			//-------------------------------------------------------
			int iSelect = 0;
			iSelect = select(0, &fdR, &fdW, NULL, &selTime);
			//-------------------------------------------------------
			//  17.01.18
			//	Select 후 처리해야할 소켓들이 있다.
			//-------------------------------------------------------
			if (iSelect > 0)
			{
				//-------------------------------------------------------
				//  17.01.18
				//	Accept 처리가 필요한 곳
				//-------------------------------------------------------
				if ( FD_ISSET(g_hLisnSock, &fdR) )
				{
					//AcceptProc()은 인자가 필요없다.
					AcceptProc();
				}
				//-------------------------------------------------------
				//  17.01.18
				//	Recv 처리
				//-------------------------------------------------------
				for (Iter_Recv; Iter_Recv != Iter_Client; ++Iter_Recv)
				{
					if (FD_ISSET((*Iter_Recv)->hSocket, &fdW))
					{
						SendProc((*Iter_Recv));
					}
					if (FD_ISSET((*Iter_Recv)->hSocket, &fdR))
					{
						RecvProc((*Iter_Recv));
					}
				}
			}

			if (Iter_Client == l_Client.end())
				break;
		}
		
		DisConnectProc();
}

//-------------------------------------------------------
//  17.01.18
//	LisnSocket 에 Accept 가 들어왔을때, 처리한다.
//-------------------------------------------------------
void AcceptProc()
{
	g_AcceptCount++;
	SHORT X = rand() % 6300;
	SHORT Y = rand() % 6300;
	//-------------------------------------------------------
	//  17.01.18
	//	일단 Aceept 처리, Session의 생성, List에 추가
	//-------------------------------------------------------
	stClient *pNew = new stClient;
	pNew->hSocket = INVALID_SOCKET;
	pNew->ID = g_ClientID++;
	pNew->Dir = dfPACKET_MOVE_DIR_LL;
	pNew->Msg = dfPACKET_CS_MOVE_STOP;
	pNew->X = X;
	pNew->Y = Y;
	pNew->SendCount = 0;
	pNew->RecvCount = 0;
	pNew->byAttack = 0;
	pNew->bDisconn = FALSE;
	
	pNew->dwActionTick = 0;
	pNew->pOld_Sector = NULL;
	pNew->pNow_Sector = NULL;
	pNew->HP = 100;
	pNew->hSocket = accept(g_hLisnSock, NULL, NULL);
	pNew->ActionX = X;
	pNew->ActionY = Y;
	// Accept 오류처리
	if (pNew->hSocket == INVALID_SOCKET)
	{
		int iError = WSAGetLastError();
		//
		__Log(dfLOG_WARNING, L"Accept Error[%d]", iError);
		//
		delete pNew;
		return;
	}

	l_Client.push_back(pNew);
	int optval = TRUE;
	setsockopt(pNew->hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	//-------------------------------------------------------
	//  17.01.18
	//	X,Y 좌표를 확인해서 Sector를 배정한다.
	//
	//	17.01.23
	//	2차원 배열 섹터로 변경
	//-------------------------------------------------------
	int iH, iW;				// , iCenterIdx;
	iH = pNew->Y / dfSECTOR_SIZE;
	iW = pNew->X / dfSECTOR_SIZE;

	// 높이 인덱스 * PITCH
	pNew->pNow_Sector = &g_Sector[iH][iW];
	// Sector의 멤버로 나를 넣는다.
	pNew->pNow_Sector->l_Sector_Client.push_back(pNew);

	//-------------------------------------------------------
	//  17.01.18
	//	Send_UniCast = 내 캐릭터의 생성 패킷 받고, 
	//-------------------------------------------------------
	CProtoBuffer SendData = MakePacket(pNew, dfPACKET_SC_CREATE_MY_CHARACTER);
	Send_Uni(pNew, &SendData);
	//-------------------------------------------------------
	//  17.01.18
	//	Send_UniCast = 주변 캐릭터의 존재를 패킷으로 받고,
	//-------------------------------------------------------
	int FixedH, FixedW;
	list<stClient*>::iterator ITER;
	
	int Limit = 0;
	// 이게 문제인데....
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			FixedH = iH + h;
			FixedW = iW + w;
			if (FixedH < 0 || FixedW < 0 || dfSECTOR_HEIGHT <= FixedH || dfSECTOR_WIDTH <= FixedW)
				continue;

			// Fixed 되어 찾아낸 Sector의 Member Iterater를 가져온다.
			ITER = g_Sector[FixedH][FixedW].l_Sector_Client.begin();
			for (ITER; ITER != g_Sector[FixedH][FixedW].l_Sector_Client.end(); ++ITER)
			{
				if (*ITER != pNew)
				{
					SendData.Clear();
					SendData = MakePacket(*ITER, dfPACKET_SC_CREATE_OTHER_CHARACTER);
					Send_Uni(pNew, &SendData);
					if (Limit >= 100)
					{
						break;
					}
				}
			}
		}
	}
	//-------------------------------------------------------
	//  17.01.18
	//	Send_SectorAround = 다른 캐릭터의 생성 패킷 보내고,
	//-------------------------------------------------------
	SendData.Clear();
	SendData = MakePacket(pNew, dfPACKET_SC_CREATE_OTHER_CHARACTER);
	Send_SectorAround(pNew, pNew->pNow_Sector, &SendData);

	pNew->dwRecvTick = timeGetTime();
}
//-------------------------------------------------------
//  17.01.18
//	연결을 종료하는 녀석.	// 일단, 이 함수는 사용금지
//-------------------------------------------------------
void DisConnectProc()
{
	CProtoBuffer SendData;
	DWORD		 nowTime = timeGetTime();
	DWORD		 Tick = 0;
	//DisConnect 하는 녀석이면...
	ITER_CLIENT Iter_Del = l_Client.begin();
	for (Iter_Del; Iter_Del != l_Client.end();)
	{
		Tick = nowTime - (*Iter_Del)->dwRecvTick;
		//먼저 세션 리스트에서 삭제하고
		if ((*Iter_Del)->bDisconn == TRUE || Tick > dfRECV_TICK_MAX)
		{
			stClient* pDel = *Iter_Del;
			l_Client.erase(Iter_Del++);
			
			//그 다음 자신이 들어가있는 Sector에서 나간다.
			list<stClient*>::iterator Iter_Sector = pDel->pNow_Sector->l_Sector_Client.begin();
			for (Iter_Sector; Iter_Sector != pDel->pNow_Sector->l_Sector_Client.end(); ++Iter_Sector)
			{
				if (*Iter_Sector == pDel) {
					pDel->pNow_Sector->l_Sector_Client.erase(Iter_Sector);
					break;
				}
			}
			SendData.Clear();
			SendData = MakePacket(pDel, dfPACKET_SC_DELETE_CHARACTER);
			Send_SectorAround(pDel, pDel->pNow_Sector, &SendData);
			//그리고 탈출한다.
			closesocket(pDel->hSocket);
			//정상적인 로그아웃 로그
			__Log(dfLOG_SYSTEM, L"Disconnect_UserID[%d], RecvTick : %d, RqSize : %d, SqSize: %d", pDel->ID, Tick, pDel->rqR.GetCurrentUsingSize(), pDel->rqS.GetCurrentUsingSize());
			//__Log(dfLOG_SYSTEM, L"X : %d, Y : %d, SectorH : %d, SectorW : %d", pDel->X, pDel->Y, pDel->Y / 200, pDel->X / 200);
			delete pDel;
		}
		else
		{
			Iter_Del++;
		}
	}
}
//-------------------------------------------------------
//  17.01.18
//	해당 클라이언트 소켓에 Recv 하는 함수.
//-------------------------------------------------------
void RecvProc(stClient *in_pClient)
{
	//-------------------------------------------------------
	//  17.01.19
	//	이미 사전에 종료를 공지한 Socket이면, 탈출한다.
	//-------------------------------------------------------
	if (in_pClient->bDisconn)
		return;

	//-------------------------------------------------------
	//  17.01.19
	//	RecvProc 돌기 시작합니당
	//-------------------------------------------------------
	// RECV 큐가 꽉 찾다면, 종료
	if (0 == in_pClient->rqR.GetNotBrokenPutSize())
	{

		__Log(dfLOG_WARNING, L"Recv Queue Full_USERID[%d]", in_pClient->ID);

		in_pClient->bDisconn = TRUE;
		return;
	}
	
	// RECV 큐로 곧바로 전송받고,
	int iRecvSize = 0;
	iRecvSize = recv(in_pClient->hSocket, in_pClient->rqR.GetWriteBufferPtr(), in_pClient->rqR.GetNotBrokenPutSize(), 0);		g_RecvCount++;

	// iRecvSize = 0 은 연결 종료메세지...
	if (iRecvSize == 0 || iRecvSize == SOCKET_ERROR)
	{
		__Log(dfLOG_SYSTEM, L"Client Send 0_USERID[%d]", in_pClient->ID);
		in_pClient->bDisconn = TRUE;
		return;
	}
	

	// RECV 데이터만큼, WritePos를 이동시킨다.	
	in_pClient->rqR.MoveWritePos(iRecvSize);

	//-------------------------------------------------------
	//  17.01.23
	//	dwRecvTick을 갱신한다.
	//-------------------------------------------------------
	in_pClient->dwRecvTick = timeGetTime();

	// 받은 PACKET 처리루틴을 탄다.
	DisAssembleRecvQueue(in_pClient);
}

//-------------------------------------------------------
//  17.01.18
//	해당 클라이언트의 RecvQueue에 있는 Packet을 처리하는 함수.
//-------------------------------------------------------
void DisAssembleRecvQueue(stClient *in_pClient)
{
	stPACKET_HEADER Header;
	CProtoBuffer Data;
	BYTE CheckEnd;
	while (1)
	{	
		Data.Clear();
		int iPeek = 0;
		// 리시브 큐에 데이터가 없다면, 브레이크
		if (in_pClient->rqR.GetCurrentUsingSize() < sizeof(stPACKET_HEADER))
			break;
		
		iPeek = in_pClient->rqR.Peek((char*)&Header, sizeof(stPACKET_HEADER));
		
		if (in_pClient->rqR.GetCurrentUsingSize() < sizeof(stPACKET_HEADER) + Header.bySize + 1)
			break;
		
		in_pClient->rqR.RemoveData(sizeof(stPACKET_HEADER));
		iPeek = in_pClient->rqR.Peek(Data.GetBufferPtr(), Header.bySize); // 아님
		Data.MoveWritePos(iPeek);
		in_pClient->rqR.RemoveData(iPeek);

		in_pClient->rqR.Dequeue((char*)&CheckEnd, 1);
		if (CheckEnd != dfNETWORK_PACKET_END)
		{
			__Log(dfLOG_WARNING, L"Incorrect End-Code_UserId[%d]", in_pClient->ID);
			in_pClient->bDisconn = TRUE;
			return;
		}

		SwitchPacket(in_pClient, &Header, &Data);
	}
}
//-------------------------------------------------------
//  17.01.18
//	Switch Packet	
//-------------------------------------------------------
void SwitchPacket(stClient *in_pClient, stPACKET_HEADER *pHead, CProtoBuffer *pData)
{
	switch (pHead->byType)
	{
		case dfPACKET_CS_MOVE_START:
		{
			Proc_PACKET_CS_MOVE_START(in_pClient, pData);
			break;
		}
		case dfPACKET_CS_MOVE_STOP:
		{
			Proc_PACKET_CS_MOVE_STOP(in_pClient, pData);
			break;
		}
		case dfPACKET_CS_ATTACK1:
		{
			Proc_PACKET_CS_ATTACK1(in_pClient, pData);
			break;
		}
		case dfPACKET_CS_ATTACK2:
		{
			Proc_PACKET_CS_ATTACK2(in_pClient, pData);
			break;
		}
		case dfPACKET_CS_ATTACK3:
		{
			Proc_PACKET_CS_ATTACK3(in_pClient, pData);
			break;
		}
		default:
			in_pClient->bDisconn = TRUE;
			__Log(dfLOG_WARNING, L"Undefine Packet Msg_UserId[%d]", in_pClient->ID);
			break;
	}
}
//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void SendProc(stClient *in_pClient)
{
	//-------------------------------------------------------
	//  17.01.19
	//	이미 종료가 선언된 소켓
	//-------------------------------------------------------
	if (in_pClient->bDisconn)
		return;

	//-------------------------------------------------------
	//  17.01.19
	//	정상적인 SendProc 진행
	//-------------------------------------------------------
	int iSend;
	int iDequeue;
	char SendBuffer[2000];
	while (in_pClient->rqS.GetCurrentUsingSize() > 0)
	{
		g_SendCount++;
		
		//iDequeue = in_pClient->rqS.Dequeue(SendBuffer, 2000);
		//if (iDequeue == 0)
		//	return;

		iDequeue = in_pClient->rqS.GetNotBrokenGetSize();
		if (iDequeue == 0)
			return;
		
		iSend = send(in_pClient->hSocket, in_pClient->rqS.GetReadBufferPtr(), iDequeue, 0);
		if (iSend == 0 || iSend == SOCKET_ERROR)
		{
			int iError = WSAGetLastError();
			in_pClient->bDisconn = TRUE;
			//__Log(dfLOG_LEVEL3, L"Send Error");
			__Log(dfLOG_WARNING, L"Send Error[%d]", iError);
			break;
		}
		in_pClient->rqS.RemoveData(iSend);

	}
}
//-------------------------------------------------------
//  17.01.18
//	헤더, 데이터, EndCode 까지 완성된 직렬화 버퍼의 포인터가 올 것이다.
//-------------------------------------------------------
//-------------------------------------------------------
//  17.01.19
//	in_pDest 하나에게 in_pData 를 보낸다.
//-------------------------------------------------------
void Send_Uni(stClient *in_pDest, CProtoBuffer *in_pData)		
{
	int iEnq = 0;
	iEnq = in_pDest->rqS.Enqueue(in_pData->GetBufferPtr(), in_pData->GetDataSize());
	if (iEnq != in_pData->GetDataSize())
	{
		__Log(dfLOG_WARNING, L"Send Queue Full!!!_UserID[%d]", in_pDest->ID);
		in_pDest->bDisconn = TRUE;
	}	
}
//-------------------------------------------------------
//  17.01.19
//	in_pSector에 in_pFrom을 제외하고 데이터를 보낸다.
//-------------------------------------------------------
void Send_Sector(stClient *in_pFrom, stSector *in_pSector, CProtoBuffer *in_pData) 
{
	list<stClient*>::iterator ITER;
	ITER = in_pSector->l_Sector_Client.begin();
	for (ITER; ITER != in_pSector->l_Sector_Client.end(); ++ITER)
	{	
		if (*ITER != in_pFrom)
			Send_Uni(*ITER, in_pData);
	}
}
//-------------------------------------------------------
//  17.01.19
//	in_pFrom을 제외한 전체유저에게 데이터를 보낸다.
//-------------------------------------------------------
void Send_Broad(stClient *in_pFrom, CProtoBuffer *in_pData)
{
	ITER_CLIENT Iter = l_Client.begin();
	for (Iter; Iter != l_Client.end(); ++Iter)
	{
		if(*Iter != in_pFrom)
			Send_Uni((*Iter), in_pData);
	}
}
//-------------------------------------------------------
//  17.01.19
//	in_Center를 기준으로 3x3 섹터에 in_pSrc를 제외하고 데이터를 보낸다.
//
//	17.01.23
//	2차원 섹터 변경 
//-------------------------------------------------------
void Send_SectorAround(stClient *in_pSrc, stSector *in_Center, CProtoBuffer *in_pData)
{
	int CenterH, CenterW;
	int FixedH, FixedW;

	CenterH = in_Center->_iH;
	CenterW = in_Center->_iW;
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			FixedH = CenterH + h;
			FixedW = CenterW + w;
			if (FixedH < 0 || FixedW < 0 || dfSECTOR_HEIGHT <= FixedH || dfSECTOR_WIDTH <= FixedW)
				continue;

			Send_Sector(in_pSrc, &g_Sector[FixedH][FixedW], in_pData);
		}
	}
}







