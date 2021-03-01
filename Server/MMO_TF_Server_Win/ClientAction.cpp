#include "ClientAction.h"
#include "PacketDefine.h"
#include "PacketProc.h"
#include "NetworkProc.h"
#include <list>
using namespace std;

#define MAX_LEFT 0
#define MAX_TOP 0
#define MAX_RIGHT 6400
#define MAX_BOTTOM 6400

extern DWORD g_FrameMAX;

extern DWORD g_Frame;
extern DWORD g_UpdateTick;
extern list<stClient*> l_Client;
extern stSector g_Sector[dfSECTOR_HEIGHT][dfSECTOR_WIDTH];
//-------------------------------------------------------
//  17.01.18
//	���� �ֵ��� ó���� �༮
//-------------------------------------------------------
void Update()
{

	//-------------------------------------------------------
	//  17.01.18
	//	X,Y ��� 40 Pixel ���� Ʋ������ ����
	//-------------------------------------------------------
	DWORD nowTick = timeGetTime();
	DWORD time = nowTick - g_UpdateTick;
	if (time < 40)
		return;
	//if (g_Frame > 25)
		//return;
	g_FrameMAX = max(g_FrameMAX, time);

	list<stClient*>::iterator Iter_Client = l_Client.begin();
	for (Iter_Client; Iter_Client != l_Client.end(); ++Iter_Client)
	{
		ActionProc(*Iter_Client);
		CollisionProc(*Iter_Client);
	}


	g_UpdateTick = nowTick;
	g_Frame		++;

}
//-------------------------------------------------------
//  17.01.18
//	�ֵ��� ������ �༮
//-------------------------------------------------------
void ActionProc(stClient *pClient)
{
	switch (pClient->Msg)
	{
		case dfPACKET_MOVE_DIR_LL:
		{
			if (pClient->X > MAX_LEFT)
				pClient->X = max(MAX_LEFT, (int)(pClient->X - 6));
			break;
		}
		case dfPACKET_MOVE_DIR_LU:
		{
			if (pClient->X > MAX_LEFT && pClient->Y > MAX_TOP)
			{
				pClient->X = max(MAX_LEFT, (int)pClient->X - 6);
				pClient->Y = max(MAX_TOP, (int)pClient->Y - 4);
			}
			break;
		}
		case dfPACKET_MOVE_DIR_UU:
		{
			if (pClient->Y > MAX_TOP)
			{
				pClient->Y = max(MAX_TOP, (int)pClient->Y - 4);
			}
			break;
		}
		case dfPACKET_MOVE_DIR_RU:
		{
			if (pClient->X < MAX_RIGHT && pClient->Y > MAX_TOP)
			{
				pClient->X = min(MAX_RIGHT, (int)pClient->X + 6);
				pClient->Y = max(MAX_TOP, (int)pClient->Y - 4);
			}
			break;
		}
		case dfPACKET_MOVE_DIR_RR:
		{
			if (pClient->X < MAX_RIGHT)
				pClient->X = min(MAX_RIGHT, (int)pClient->X + 6);
			break;
		}
		case dfPACKET_MOVE_DIR_RD:
		{
			if (pClient->X < MAX_RIGHT && pClient->Y < MAX_BOTTOM)
			{
				pClient->X = min(MAX_RIGHT, (int)pClient->X + 6);
				pClient->Y = min(MAX_BOTTOM, (int)pClient->Y + 4);
			}
			break;
		}
		case dfPACKET_MOVE_DIR_DD:
		{
			if (pClient->Y < MAX_BOTTOM)
			{
				pClient->Y = min(MAX_BOTTOM, (int)pClient->Y + 4);
			}
			break;
		}
		case dfPACKET_MOVE_DIR_LD:
		{
			if (pClient->X > MAX_LEFT && pClient->Y < MAX_BOTTOM)
			{
				pClient->X = max(MAX_LEFT, (int)pClient->X - 6);
				pClient->Y = min(MAX_BOTTOM, (int)pClient->Y + 4);
			}
			break;
		}
		case dfPACKET_CS_MOVE_STOP:
		{
			break;
		}
		case dfPACKET_CS_ATTACK1:
		{
			pClient->byAttack = 1;
			break;
		}
		case dfPACKET_CS_ATTACK2:
		{
			pClient->byAttack = 2;
			break;
		}
		case dfPACKET_CS_ATTACK3:
		{
			pClient->byAttack = 3;
			break;
		}
	}
	CalcAroundSector(pClient);

	//-------------------------------------------------------
	//  17.01.25
	//	���� �����Ӱ� ���巹Ŀ�� �������� �󸶳� Ʋ������ �� ����
	//-------------------------------------------------------
	pClient->ServerFrame++;
}
//-------------------------------------------------------
//  17.01.18
//	Calc Sector
//
//	�氰�� ! ���������� �ù� ����������
//-------------------------------------------------------
void CalcAroundSector(stClient	*pClient)
{
	//iX �� �ް�, iY�� �޴´�.
	int iH, iW;	//	ĳ������ ��ǥ
	int NewH, NewW, OldH, OldW;	//	����, ������ ������ �ε���
	
	// �̾�!
	iW = pClient->X;	
	iH = pClient->Y;

	//oldIndex = pClient->pNow_Sector->_Index;
	//-------------------------------------------------------
	//  17.01.23
	//	2���� �ε����� ����
	//-------------------------------------------------------
	OldH = pClient->pNow_Sector->_iH;
	OldW = pClient->pNow_Sector->_iW;
	NewH = iH / dfSECTOR_SIZE;
	NewW = iW / dfSECTOR_SIZE;
	if (NewH == dfSECTOR_HEIGHT)
		NewH = dfSECTOR_HEIGHT - 1;
	if (NewW == dfSECTOR_WIDTH)
		NewW = dfSECTOR_WIDTH - 1;
	// index�� ��ȭ�� ���ٸ�, Sector�� ������ �ʿ䰡 ����.
	if ((OldH == NewH) && (OldW == NewW))
		return;	// ��� ����

//---------------------------------------------------------------------
//  17.01.19
//	Sector�� �̵��� �ʿ��� ����
//---------------------------------------------------------------------

	// ���� ���� ���Ϳ��� ���� �����ϰ�
	list<stClient*>::iterator ITER_SECTOR = pClient->pNow_Sector->l_Sector_Client.begin();
	for (ITER_SECTOR; ITER_SECTOR != pClient->pNow_Sector->l_Sector_Client.end(); ++ITER_SECTOR)
	{
		if (*ITER_SECTOR == pClient)
		{
			pClient->pNow_Sector->l_Sector_Client.erase(ITER_SECTOR);
			break;
		}
	}

	// ���� �� ���Ϳ��� ���� �ٽ� ����ִ´�.
	pClient->pOld_Sector = pClient->pNow_Sector;
	pClient->pNow_Sector = &g_Sector[NewH][NewW];
	// �� ĳ������ �����̵�, ���Ϳ� �� ĳ������ ����, �Խ�~ �Ϸ�		//	 17.01.23
	pClient->pNow_Sector->l_Sector_Client.push_back(pClient);
	

	// �������;����, ���ε����;���� ��
	//	oldIndex	== ������ �ִ� ���� �߽� ����
	//	newIndex	== ���� �ִ� ���� �߽� ����
	
	//-------------------------------------------------------
	//  17.01.23
	//	2���� ���� ����, �ڵ� �� �ٲٴ� ���� ��.��
	//-------------------------------------------------------
	
	stSector*	arrOld[9];
	stSector*	arrNew[9];
	stSector*	arrTempOld[9];

	// �õ��� �ֺ�, ���� �ֺ� ���� �ε��� ���� �Ϸ�...
	// ��...
	int FixedOldH, FixedOldW;
	int FixedNewH, FixedNewW;
	
	int i = 0;
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			// ����� ���� ����
			FixedOldH = OldH + h;
			FixedOldW = OldW + w;
			FixedNewH = NewH + h;
			FixedNewW = NewW + w;
			
			// ��ġ ������ �ʱ�ȭ�ϵ���... dfSECTOR_ARR_EMPTY = NULL �̴�. 17.01.23
			arrOld[i] = arrTempOld[i] = arrNew[i] = dfSECTOR_ARR_EMPTY;
			
			if (!(FixedOldH < 0 || FixedOldW < 0 || dfSECTOR_HEIGHT <= FixedOldH || dfSECTOR_WIDTH <= FixedOldW))
			{
				arrOld[i] = &g_Sector[FixedOldH][FixedOldW];
				arrTempOld[i] = &g_Sector[FixedOldH][FixedOldW];
			}
			if (!(FixedNewH < 0 || FixedNewW < 0 || dfSECTOR_HEIGHT <= FixedNewH || dfSECTOR_WIDTH <= FixedNewW))
			{
				arrNew[i] = &g_Sector[FixedNewH][FixedNewW];
			}
			i++;
		}
	}

	//	���� �̰� ���ؼ� �̾Ƴ���.
	int NEW;
	int OLD;

	//	�̷��� �Ǹ� �ʹ� ������;;
	for (NEW = 0; NEW < 9; NEW++)
	{
		for (OLD = 0; OLD < 9; OLD++)
		{
			if (arrOld[OLD] == arrNew[NEW])
			{
				arrOld[OLD] = dfSECTOR_ARR_EMPTY;
				break;
			}
				
		}
	}

	for (OLD = 0; OLD < 9; OLD++)
	{
		for (NEW = 0; NEW < 9; NEW++)
		{
			if (arrNew[NEW] == arrTempOld[OLD])
			{
				arrNew[NEW] = dfSECTOR_ARR_EMPTY;
				break;
			}
		}
	}
	//-------------------------------------------------------
	//  17.01.19
	//	arrOld ���� �� ĳ������ ������Ŷ�� ������,
	//	�³׵鿡 ���� ���� ��Ŷ�� �� ���� ���ڽ����� ��������.
	//
	//
	//	17.01.23
	//	2���� ���� ����
	//-------------------------------------------------------
	CProtoBuffer SendMyData;
	CProtoBuffer RecvOtherData;
	list<stClient*>::iterator ITER_CLIENT;
	SendMyData = MakePacket(pClient, dfPACKET_SC_DELETE_CHARACTER);
	int Limit = 0;
	for (i = 0; i < 9; i++)
	{
		if (arrOld[i] == dfSECTOR_ARR_EMPTY)
			continue;
		
		// �� ĳ������ ������Ŷ�� �θ���...
		Send_Sector(pClient, arrOld[i], &SendMyData);
		
		// �ٸ� ĳ������ ������Ŷ�� ���� �޴´�.
		ITER_CLIENT = arrOld[i]->l_Sector_Client.begin();
		for (ITER_CLIENT; ITER_CLIENT != arrOld[i]->l_Sector_Client.end(); ++ITER_CLIENT)
		{
			RecvOtherData.Clear();
			RecvOtherData = MakePacket(*ITER_CLIENT, dfPACKET_SC_DELETE_CHARACTER);
			Send_Uni(pClient, &RecvOtherData);
			Limit++;
			if (Limit > 100)
				break;
		}
	}
	//-------------------------------------------------------
	//  17.01.19
	//	arrNew ���� �� ĳ������ ������Ŷ�� ������ �ǰ�.
	//  ���ο� ���� �ֵ��� ������Ŷ�� �޾ƾ��Ѵ�.
	//-------------------------------------------------------
	CProtoBuffer StopMsg;
	CProtoBuffer SendMyMsg;
	StopMsg = MakePacket(pClient, dfPACKET_SC_MOVE_STOP);
	SendMyMsg = MakeMsgPacket(pClient);
	SendMyData.Clear();
	RecvOtherData.Clear();
	SendMyData = MakePacket(pClient, dfPACKET_SC_CREATE_OTHER_CHARACTER);
	Limit = 0;
	for (i = 0; i < 9 ;i++)
	{
		if (arrNew[i] == dfSECTOR_ARR_EMPTY)
			continue;

		// �� ĳ������ ���� ��Ŷ�� �Ѹ���
		Send_Sector(pClient, arrNew[i], &SendMyData);
		// ���� �ϰ��ִ� �ൿ�� �����ߵǴµ�
	//	Send_Sector(pClient, arrNew[i], &StopMsg);
		Send_Sector(pClient, arrNew[i], &SendMyMsg);

		//������ �����͸� �޴µ�
		ITER_CLIENT = arrNew[i]->l_Sector_Client.begin();
		for (ITER_CLIENT; ITER_CLIENT != arrNew[i]->l_Sector_Client.end(); ++ITER_CLIENT)
		{
			RecvOtherData.Clear();
			RecvOtherData = MakePacket(*ITER_CLIENT, dfPACKET_SC_CREATE_OTHER_CHARACTER);
			Send_Uni(pClient, &RecvOtherData);

	//		RecvOtherData.Clear();
	//		RecvOtherData = MakePacket(*ITER_CLIENT, dfPACKET_SC_MOVE_STOP);
	//		Send_Uni(pClient, &RecvOtherData);
			
			RecvOtherData.Clear();
			RecvOtherData = MakeMsgPacket(*ITER_CLIENT);
			Send_Uni(pClient, &RecvOtherData);
		}
	}

}




void	CollisionProc(stClient* pClient)
{
	int iXRange = 90;
	int iYRange = 15;
	BOOL bEnd = FALSE;
	////////////////////////////////////////////////////
	//	���鼭 ������ �� �������... ó�����ش�...
	////////////////////////////////////////////////////
	CProtoBuffer SendData;
	CProtoBuffer DeadData;
		if (pClient->byAttack != 0)
		{
			switch (pClient->Dir)
			{
				//�������� ����
			case dfPACKET_MOVE_DIR_LL:
			case dfPACKET_MOVE_DIR_LU:
			case dfPACKET_MOVE_DIR_LD:
			{
				int	iLeftX = pClient->X - 90;
				int iRightX = pClient->X - 20;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// �� ������ ������
				list<stClient*>::iterator ITER;
				ITER = pClient->pNow_Sector->l_Sector_Client.begin();
				for (ITER; ITER != pClient->pNow_Sector->l_Sector_Client.end(); ++ITER)
				{
					if ((*ITER)->X >= iLeftX && (*ITER)->X <= iRightX)
						if ((*ITER)->Y >= iTopY && (*ITER)->Y <= iBottomY)
						{
							SendData.Clear();
							SendData << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_DAMAGE << dfNETWORK_PACKET_END;
							SendData << pClient->ID << (*ITER)->ID;
							(*ITER)->HP = max(0, (*ITER)->HP - pClient->byAttack * 4);
							
							SendData << (*ITER)->HP << dfNETWORK_PACKET_END;
							Send_SectorAround((*ITER), (*ITER)->pNow_Sector, &SendData);
							Send_Uni((*ITER), &SendData);

							if ((*ITER)->HP < 0)
							{
								(*ITER)->HP = 100;

								//-------------------------------------------------------
								//  17.01.25
								//	���� �׽�Ʈ�� ���� ���� �����Ų��...
								//-------------------------------------------------------
							//	DeadData.Clear();
							//	DeadData = MakePacket(*ITER, dfPACKET_SC_DELETE_CHARACTER);
							//	Send_SectorAround(*ITER, (*ITER)->pNow_Sector, &DeadData);
							//	Send_Uni(*ITER, &DeadData);
							}
							bEnd = TRUE;
							break;
						}
				}

				int SecX = pClient->pNow_Sector->_iW - 1;
				int SecY = pClient->pNow_Sector->_iH;
				if (SecX < 0 && bEnd == TRUE)
					break;

				ITER = g_Sector[SecY][SecX].l_Sector_Client.begin();
				for (ITER; ITER != g_Sector[SecY][SecX].l_Sector_Client.end(); ++ITER)
				{
					if ((*ITER)->X >= iLeftX && (*ITER)->X <= iRightX)
						if ((*ITER)->Y >= iTopY && (*ITER)->Y <= iBottomY)
						{
							SendData.Clear();
							SendData << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_DAMAGE << dfNETWORK_PACKET_END;
							SendData << pClient->ID << (*ITER)->ID;
							(*ITER)->HP = max(0, (*ITER)->HP - pClient->byAttack * 4);

							SendData << (*ITER)->HP << dfNETWORK_PACKET_END;
							Send_SectorAround((*ITER), (*ITER)->pNow_Sector, &SendData);
							Send_Uni((*ITER), &SendData);

							if ((*ITER)->HP < 0)
							{
								(*ITER)->HP = 100;

								//-------------------------------------------------------
								//  17.01.25
								//	���� �׽�Ʈ�� ���� ���� �����Ų��...
								//-------------------------------------------------------
							//	DeadData.Clear();
							//	DeadData = MakePacket(*ITER, dfPACKET_SC_DELETE_CHARACTER);
							//	Send_SectorAround(*ITER, (*ITER)->pNow_Sector, &DeadData);
							//	Send_Uni(*ITER, &DeadData);
							}
							break;
						}
				}
				break;
			}
			//���������� ����
			case dfPACKET_MOVE_DIR_RU:
			case dfPACKET_MOVE_DIR_RR:
			case dfPACKET_MOVE_DIR_RD:
			{
				int	iLeftX = pClient->X + 20;
				int iRightX = pClient->X + 90;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// �� ������ ������
				list<stClient*>::iterator ITER;
				ITER = pClient->pNow_Sector->l_Sector_Client.begin();
				for (ITER; ITER != pClient->pNow_Sector->l_Sector_Client.end(); ++ITER)
				{
					if ((*ITER)->X >= iLeftX && (*ITER)->X <= iRightX)
						if ((*ITER)->Y >= iTopY && (*ITER)->Y <= iBottomY)
						{
							SendData.Clear();
							SendData << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_DAMAGE << dfNETWORK_PACKET_END;
							SendData << pClient->ID << (*ITER)->ID;
							(*ITER)->HP = max(0, (*ITER)->HP - pClient->byAttack * 4);
							

							SendData << (*ITER)->HP << dfNETWORK_PACKET_END;
							Send_SectorAround((*ITER), (*ITER)->pNow_Sector, &SendData);
							Send_Uni((*ITER), &SendData);

							if ((*ITER)->HP < 0)
							{
								(*ITER)->HP = 100;

								//-------------------------------------------------------
								//  17.01.25
								//	���� �׽�Ʈ�� ���� ���� �����Ų��...
								//-------------------------------------------------------
							//	DeadData.Clear();
							//	DeadData = MakePacket(*ITER, dfPACKET_SC_DELETE_CHARACTER);
							//	Send_SectorAround(*ITER, (*ITER)->pNow_Sector, &DeadData);
							//	Send_Uni(*ITER, &DeadData);
							}
							bEnd = TRUE;
							break;
						}
				}

				int SecX = pClient->pNow_Sector->_iW + 1;
				int SecY = pClient->pNow_Sector->_iH;
				if (SecX >= dfSECTOR_WIDTH || bEnd == TRUE)
					break;

				ITER = g_Sector[SecY][SecX].l_Sector_Client.begin();
				for (ITER; ITER != g_Sector[SecY][SecX].l_Sector_Client.end(); ++ITER)
				{
					if ((*ITER)->X >= iLeftX && (*ITER)->X <= iRightX)
						if ((*ITER)->Y >= iTopY && (*ITER)->Y <= iBottomY)
						{
							SendData.Clear();
							SendData << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_DAMAGE << dfNETWORK_PACKET_END;
							SendData << pClient->ID << (*ITER)->ID;
							(*ITER)->HP = max(0, (*ITER)->HP - pClient->byAttack * 4);

							SendData << (*ITER)->HP << dfNETWORK_PACKET_END;
							Send_SectorAround((*ITER), (*ITER)->pNow_Sector, &SendData);
							Send_Uni((*ITER), &SendData);

							if ((*ITER)->HP < 0)
							{
								(*ITER)->HP = 100;

								//-------------------------------------------------------
								//  17.01.25
								//	���� �׽�Ʈ�� ���� ���� �����Ų��...
								//-------------------------------------------------------
							//	DeadData.Clear();
							//	DeadData = MakePacket(*ITER, dfPACKET_SC_DELETE_CHARACTER);
							//	Send_SectorAround(*ITER, (*ITER)->pNow_Sector, &DeadData);
							//	Send_Uni(*ITER, &DeadData);
							}
							break;
						}
				}
				break;

			
			}

			default:
				if (0 == 0)
				{

				}
				break;
			}///////////////////////SWITCH
			pClient->byAttack = 0;
			pClient->Msg = dfPACKET_CS_MOVE_STOP;
		}//////////////////////IF

}