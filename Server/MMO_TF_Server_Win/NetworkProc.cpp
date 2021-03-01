#include "NetworkProc.h"
#include "PacketDefine.h"
#include "PacketProc.h"



extern SOCKET	g_hLisnSock;
extern DWORD g_ClientID;	// AccountNo �ο���, ��л� 7��

extern list<stClient*> l_Client;	//	Ŭ���̾�Ʈ ���� ���� ����Ʈ
extern stSector g_Sector[dfSECTOR_HEIGHT][dfSECTOR_WIDTH];
typedef list<stClient*>::iterator ITER_CLIENT;

#define dfRECV_TICK_MAX	30000

//-------------------------------------------------------
//  17.01.18
//	��¿� �������̶���
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
//	��Ʈ��ũ ���Ͻ���, ���ú긦 �ް�, ���带 �� ���̴�.
//-------------------------------------------------------
void NetworkProc()
{
	fd_set fdR, fdW;	//	64���� ����Ʈ�� �뵵.
	timeval selTime;
	selTime.tv_sec = 0;
	selTime.tv_usec = 0;

		// ��ü�� ��Ʈ���� Iterator ����
		list<stClient*>::iterator Iter_Client = l_Client.begin();
		while (1)
		{
			// �ʱ�ȭ
			FD_ZERO(&fdR);
			FD_ZERO(&fdW);

			FD_SET(g_hLisnSock, &fdR);
			int iReadSet = 1;
			int iWriteSet = 1;

			list<stClient*>::iterator Iter_Recv = Iter_Client;

			// 64 ���� ó���� ���ؼ�
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
			//	Select ó���� ���� ���̴�.
			//-------------------------------------------------------
			int iSelect = 0;
			iSelect = select(0, &fdR, &fdW, NULL, &selTime);
			//-------------------------------------------------------
			//  17.01.18
			//	Select �� ó���ؾ��� ���ϵ��� �ִ�.
			//-------------------------------------------------------
			if (iSelect > 0)
			{
				//-------------------------------------------------------
				//  17.01.18
				//	Accept ó���� �ʿ��� ��
				//-------------------------------------------------------
				if ( FD_ISSET(g_hLisnSock, &fdR) )
				{
					//AcceptProc()�� ���ڰ� �ʿ����.
					AcceptProc();
				}
				//-------------------------------------------------------
				//  17.01.18
				//	Recv ó��
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
//	LisnSocket �� Accept �� ��������, ó���Ѵ�.
//-------------------------------------------------------
void AcceptProc()
{
	g_AcceptCount++;
	SHORT X = rand() % 6300;
	SHORT Y = rand() % 6300;
	//-------------------------------------------------------
	//  17.01.18
	//	�ϴ� Aceept ó��, Session�� ����, List�� �߰�
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
	// Accept ����ó��
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
	//	X,Y ��ǥ�� Ȯ���ؼ� Sector�� �����Ѵ�.
	//
	//	17.01.23
	//	2���� �迭 ���ͷ� ����
	//-------------------------------------------------------
	int iH, iW;				// , iCenterIdx;
	iH = pNew->Y / dfSECTOR_SIZE;
	iW = pNew->X / dfSECTOR_SIZE;

	// ���� �ε��� * PITCH
	pNew->pNow_Sector = &g_Sector[iH][iW];
	// Sector�� ����� ���� �ִ´�.
	pNew->pNow_Sector->l_Sector_Client.push_back(pNew);

	//-------------------------------------------------------
	//  17.01.18
	//	Send_UniCast = �� ĳ������ ���� ��Ŷ �ް�, 
	//-------------------------------------------------------
	CProtoBuffer SendData = MakePacket(pNew, dfPACKET_SC_CREATE_MY_CHARACTER);
	Send_Uni(pNew, &SendData);
	//-------------------------------------------------------
	//  17.01.18
	//	Send_UniCast = �ֺ� ĳ������ ���縦 ��Ŷ���� �ް�,
	//-------------------------------------------------------
	int FixedH, FixedW;
	list<stClient*>::iterator ITER;
	
	int Limit = 0;
	// �̰� �����ε�....
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			FixedH = iH + h;
			FixedW = iW + w;
			if (FixedH < 0 || FixedW < 0 || dfSECTOR_HEIGHT <= FixedH || dfSECTOR_WIDTH <= FixedW)
				continue;

			// Fixed �Ǿ� ã�Ƴ� Sector�� Member Iterater�� �����´�.
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
	//	Send_SectorAround = �ٸ� ĳ������ ���� ��Ŷ ������,
	//-------------------------------------------------------
	SendData.Clear();
	SendData = MakePacket(pNew, dfPACKET_SC_CREATE_OTHER_CHARACTER);
	Send_SectorAround(pNew, pNew->pNow_Sector, &SendData);

	pNew->dwRecvTick = timeGetTime();
}
//-------------------------------------------------------
//  17.01.18
//	������ �����ϴ� �༮.	// �ϴ�, �� �Լ��� ������
//-------------------------------------------------------
void DisConnectProc()
{
	CProtoBuffer SendData;
	DWORD		 nowTime = timeGetTime();
	DWORD		 Tick = 0;
	//DisConnect �ϴ� �༮�̸�...
	ITER_CLIENT Iter_Del = l_Client.begin();
	for (Iter_Del; Iter_Del != l_Client.end();)
	{
		Tick = nowTime - (*Iter_Del)->dwRecvTick;
		//���� ���� ����Ʈ���� �����ϰ�
		if ((*Iter_Del)->bDisconn == TRUE || Tick > dfRECV_TICK_MAX)
		{
			stClient* pDel = *Iter_Del;
			l_Client.erase(Iter_Del++);
			
			//�� ���� �ڽ��� ���ִ� Sector���� ������.
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
			//�׸��� Ż���Ѵ�.
			closesocket(pDel->hSocket);
			//�������� �α׾ƿ� �α�
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
//	�ش� Ŭ���̾�Ʈ ���Ͽ� Recv �ϴ� �Լ�.
//-------------------------------------------------------
void RecvProc(stClient *in_pClient)
{
	//-------------------------------------------------------
	//  17.01.19
	//	�̹� ������ ���Ḧ ������ Socket�̸�, Ż���Ѵ�.
	//-------------------------------------------------------
	if (in_pClient->bDisconn)
		return;

	//-------------------------------------------------------
	//  17.01.19
	//	RecvProc ���� �����մϴ�
	//-------------------------------------------------------
	// RECV ť�� �� ã�ٸ�, ����
	if (0 == in_pClient->rqR.GetNotBrokenPutSize())
	{

		__Log(dfLOG_WARNING, L"Recv Queue Full_USERID[%d]", in_pClient->ID);

		in_pClient->bDisconn = TRUE;
		return;
	}
	
	// RECV ť�� ��ٷ� ���۹ް�,
	int iRecvSize = 0;
	iRecvSize = recv(in_pClient->hSocket, in_pClient->rqR.GetWriteBufferPtr(), in_pClient->rqR.GetNotBrokenPutSize(), 0);		g_RecvCount++;

	// iRecvSize = 0 �� ���� ����޼���...
	if (iRecvSize == 0 || iRecvSize == SOCKET_ERROR)
	{
		__Log(dfLOG_SYSTEM, L"Client Send 0_USERID[%d]", in_pClient->ID);
		in_pClient->bDisconn = TRUE;
		return;
	}
	

	// RECV �����͸�ŭ, WritePos�� �̵���Ų��.	
	in_pClient->rqR.MoveWritePos(iRecvSize);

	//-------------------------------------------------------
	//  17.01.23
	//	dwRecvTick�� �����Ѵ�.
	//-------------------------------------------------------
	in_pClient->dwRecvTick = timeGetTime();

	// ���� PACKET ó����ƾ�� ź��.
	DisAssembleRecvQueue(in_pClient);
}

//-------------------------------------------------------
//  17.01.18
//	�ش� Ŭ���̾�Ʈ�� RecvQueue�� �ִ� Packet�� ó���ϴ� �Լ�.
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
		// ���ú� ť�� �����Ͱ� ���ٸ�, �극��ũ
		if (in_pClient->rqR.GetCurrentUsingSize() < sizeof(stPACKET_HEADER))
			break;
		
		iPeek = in_pClient->rqR.Peek((char*)&Header, sizeof(stPACKET_HEADER));
		
		if (in_pClient->rqR.GetCurrentUsingSize() < sizeof(stPACKET_HEADER) + Header.bySize + 1)
			break;
		
		in_pClient->rqR.RemoveData(sizeof(stPACKET_HEADER));
		iPeek = in_pClient->rqR.Peek(Data.GetBufferPtr(), Header.bySize); // �ƴ�
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
	//	�̹� ���ᰡ ����� ����
	//-------------------------------------------------------
	if (in_pClient->bDisconn)
		return;

	//-------------------------------------------------------
	//  17.01.19
	//	�������� SendProc ����
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
//	���, ������, EndCode ���� �ϼ��� ����ȭ ������ �����Ͱ� �� ���̴�.
//-------------------------------------------------------
//-------------------------------------------------------
//  17.01.19
//	in_pDest �ϳ����� in_pData �� ������.
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
//	in_pSector�� in_pFrom�� �����ϰ� �����͸� ������.
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
//	in_pFrom�� ������ ��ü�������� �����͸� ������.
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
//	in_Center�� �������� 3x3 ���Ϳ� in_pSrc�� �����ϰ� �����͸� ������.
//
//	17.01.23
//	2���� ���� ���� 
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







