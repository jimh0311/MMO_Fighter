#include "stdafx.h"
#include "CStreamRQ.h"
#include "NetworkProc.h"
#include "PacketDefine.h"
extern myLinkedList<CBaseObject*> g_List;
extern CPlayerObject*	g_Player;
extern SOCKET			hServSock;
extern HWND				g_hWnd;


CStreamRQ		g_SendRQ(20000);
CStreamRQ		g_RecvRQ(20000);
/////////////////////////////////////////////////////
//	
/////////////////////////////////////////////////////
BOOL			SwitchPacketProc(st_NETWORK_PACKET_HEADER pHeader , char* pDataLoad);
CBaseObject*	SearchObjectById(DWORD id);

/////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////
void	SendProc(st_NETWORK_PACKET_HEADER pHeader, int iHeaderSize, char* pDataLoad, int iDataSize)
{
	char END = 0x79;
	int iRetSend;
	g_SendRQ.Enqueue((char*)&pHeader, iHeaderSize);
	g_SendRQ.Enqueue(pDataLoad, iDataSize);
	g_SendRQ.Enqueue(&END, 1);
	iRetSend = send(hServSock, g_SendRQ.GetReadBufferPtr(), g_SendRQ.GetCurrentUsingSize(), 0);
	g_SendRQ.RemoveData(iRetSend);
}
void	RecvProc(SOCKET sock )
{
	if (sock == NULL)
	{
		return;
	}
	while (1)
	{
		int iRetRecv;
		iRetRecv = recv(sock, g_RecvRQ.GetWriteBufferPtr(), g_RecvRQ.GetNotBrokenPutSize(), 0);
		if (iRetRecv == 0)
		{
			closesocket(sock);
			MessageBox(g_hWnd, L"서버 왈... 나가주세요", L"SYSTEM", MB_OK);
			PostQuitMessage(0);
			return;
		}
		if (iRetRecv == SOCKET_ERROR)
		{
			int iError;
			iError = WSAGetLastError();
			if (iError != WSAEWOULDBLOCK)
			{
				WCHAR szText[64];
				swprintf_s(szText, 64, L"Recv SOCKET_ERROR[%d]", iError);
				MessageBox(g_hWnd, szText, L"SYSTEM", MB_OK);
			}
			break;
		}
		else
		{
			g_RecvRQ.MoveWritePos(iRetRecv);
		}
		PackitProc();
	}
		
}


//////////////////////////////////////////////////////////
//	RECV RQ에 저장된 패킷을 시발 분석해라
//////////////////////////////////////////////////////////
void	PackitProc()
{
	//
	char	szDataLoad[1200];
	//
	st_NETWORK_PACKET_HEADER	stHeader;
	//
	BYTE	byEnd;
	///////////////////////////////////////////////////////
	//	데이터 유무 판단	
	///////////////////////////////////////////////////////
	BOOL bWhile = true;
	while (1)
	{
		if (g_RecvRQ.GetCurrentUsingSize() > sizeof(st_NETWORK_PACKET_HEADER))
		{
			///////////////////////////////////////////////////////
			//	헤더 뽑아내고 데이터 확인
			///////////////////////////////////////////////////////
			g_RecvRQ.Peek((char*)&stHeader, sizeof(st_NETWORK_PACKET_HEADER));
			if (stHeader.byCode == dfNETWORK_PACKET_CODE)
			{
				///////////////////////////////////////////////////////
				//	헤더에서 원하는 사이즈 만큼 데이터가 왔느냐?
				///////////////////////////////////////////////////////
				//	17.01.23
				//	Peek로 확인했기 때문에 헤더사이즈 + 헤더에서 명시한 사이즈보다 큰 값이 들어있는 지 확인해야한다.
				//	그렇다.
				if (g_RecvRQ.GetCurrentUsingSize() >= stHeader.bySize + sizeof(st_NETWORK_PACKET_HEADER)+1)
				{
					g_RecvRQ.RemoveData(sizeof(st_NETWORK_PACKET_HEADER));
					g_RecvRQ.Peek(szDataLoad, stHeader.bySize);
					/////////////////////////////////////////////////////
					//	뽑은 패킷의 처리 부분
					//	SwitchPacketProc();
					/////////////////////////////////////////////////////
					
					g_RecvRQ.RemoveData(stHeader.bySize);
					g_RecvRQ.Dequeue((char*)&byEnd, sizeof(char));
					if (byEnd != dfNETWORK_PACKET_END)
					{
						MessageBox(g_hWnd, L"EndCode Error", L"SYSTEM", MB_OK);
						PostQuitMessage(0);
						return;
					}
					else
					{
						SwitchPacketProc(stHeader, szDataLoad);
						continue;
					}
				}
			
				
			}
		
		}
		break;
	}		
}



BOOL SwitchPacketProc(st_NETWORK_PACKET_HEADER pHeader, char* pDataLoad)
{
	switch (pHeader.byType)
	{
	//////////////////////////////////////////////////////////
	//	내 캐릭터의 접속
	//////////////////////////////////////////////////////////
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		//stPACKET_SC_CREATE_MY_CHARACTER *pPackit = (stPACKET_SC_CREATE_MY_CHARACTER*)&pDataLoad;
		stPACKET_SC_CREATE_MY_CHARACTER stPackit = *((stPACKET_SC_CREATE_MY_CHARACTER*)pDataLoad);
		g_Player = new CPlayerObject(stPackit.ID, enPLAYER, stPackit.X, stPackit.Y, true, stPackit.HP);
		g_List.InsertHead(g_Player);
		return TRUE;
	}
	//////////////////////////////////////////////////////////
	//	다른 캐릭터의 접속
	//////////////////////////////////////////////////////////
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		CPlayerObject *pTemp;
		stPACKET_SC_CREATE_OTHER_CHARACTER stPackit = *((stPACKET_SC_CREATE_OTHER_CHARACTER*)pDataLoad);
		pTemp = new CPlayerObject(stPackit.ID, enPLAYER, stPackit.X, stPackit.Y, false, stPackit.HP);
		g_List.InsertHead(pTemp);
		return TRUE;
	}

	case dfPACKET_SC_DELETE_CHARACTER:
	{
		CBaseObject* data;
		DWORD ID = *((DWORD*)pDataLoad);
		myIterator<CBaseObject*> Iter(&g_List);
		while (Iter.GotoNext())
		{
			if (Iter.GetValue(&data))
			{
				if (data->GetID() == ID)
					Iter.DeleteThisNode();
			}
		}
		return TRUE;
	}
		break;
	case dfPACKET_SC_MOVE_START:
	{
		stPACKET_SC_MOVE_START *stDataLoad = (stPACKET_SC_MOVE_START*)pDataLoad;
		CBaseObject* pB = SearchObjectById(stDataLoad->ID);
		//
		if (pB == NULL)
			return FALSE;
		//
		pB->SetPosX(stDataLoad->X);
		pB->SetPosY(stDataLoad->Y);
		pB->SendActionMsg(stDataLoad->Dir);
		return TRUE;
	}
		break;
	case dfPACKET_SC_MOVE_STOP:
	{
		stPACKET_SC_MOVE_START *stDataLoad = (stPACKET_SC_MOVE_START*)pDataLoad;
		CBaseObject* pB = SearchObjectById(stDataLoad->ID);
		//
		if (pB == NULL)
			return FALSE;
		//
		pB->SetPosX(stDataLoad->X);
		pB->SetPosY(stDataLoad->Y);
		pB->SendActionMsg(dfACTION_STAND);
		return TRUE;
	}
		break;
	case dfPACKET_SC_ATTACK1:
	{
		stPACKET_SC_ATTACK *stDataLoad = (stPACKET_SC_ATTACK*)pDataLoad;
		CBaseObject* pB = SearchObjectById(stDataLoad->ID);
		//
		if (pB == NULL)
			return FALSE;
		//
		pB->SendActionMsg(dfACTION_ATTACK1);
		return TRUE;
	}

		break;
	case dfPACKET_SC_ATTACK2:
	{
		stPACKET_SC_ATTACK *stDataLoad = (stPACKET_SC_ATTACK*)pDataLoad;
		CBaseObject* pB = SearchObjectById(stDataLoad->ID);
		//
		if (pB == NULL)
			return FALSE;
		//
		pB->SendActionMsg(dfACTION_ATTACK2);
		return TRUE;
	}
		break;
	case dfPACKET_SC_ATTACK3:
	{
		stPACKET_SC_ATTACK *stDataLoad = (stPACKET_SC_ATTACK*)pDataLoad;
		CBaseObject* pB = SearchObjectById(stDataLoad->ID);
		//
		if (pB == NULL)
			return FALSE;
		//
		pB->SendActionMsg(dfACTION_ATTACK3);
		return TRUE;
	}
		break;
	case dfPACKET_SC_DAMAGE:
	{
		stPACKET_SC_DAMAGE *stDataLoad = (stPACKET_SC_DAMAGE*)pDataLoad;
		CBaseObject* pB = (SearchObjectById(stDataLoad->DamageID));
		//
		if (pB == NULL)
			return FALSE;
		//
		((CPlayerObject*)pB)->Demaged(stDataLoad->DamageHP);
		//static DWORD dwSparkID = 0x9882;
		CSpark *pSpark = new CSpark(pB->GetPosX(), pB->GetPosY()+1);
		g_List.InsertHead(pSpark);
		return TRUE;
	}
		break;

	case dfPACKET_SC_SYNC:
	{
		stPACKET_SC_SYNC *stDataLoad = (stPACKET_SC_SYNC*)pDataLoad;
		CBaseObject *pB = SearchObjectById(stDataLoad->ID);
		if (pB == NULL)
			return FALSE;

		pB->SetPosX(stDataLoad->X);
		pB->SetPosY(stDataLoad->Y);
		return TRUE;
	}
	}
}


CBaseObject*	SearchObjectById(DWORD id)
{
	CBaseObject *RetObj;
	myIterator<CBaseObject*> Iter(&g_List);
	while (Iter.GotoNext())
	{
		if (Iter.GetValue(&RetObj))
			if (RetObj->GetID() == id)
				return RetObj;
	}
	return NULL;
}