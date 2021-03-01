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
//	실제 애들을 처리할 녀석
//-------------------------------------------------------
void Update()
{

	//-------------------------------------------------------
	//  17.01.18
	//	X,Y 모두 40 Pixel 정도 틀어지고 있음
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
//	애들을 움직일 녀석
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
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	pClient->ServerFrame++;
}
//-------------------------------------------------------
//  17.01.18
//	Calc Sector
//
//	잦같은 ! ㅋㅋㅋㅋㅋ 시밤 ㅋㅋㅋㅋㅋ
//-------------------------------------------------------
void CalcAroundSector(stClient	*pClient)
{
	//iX 를 받고, iY를 받는다.
	int iH, iW;	//	캐릭터의 좌표
	int NewH, NewW, OldH, OldW;	//	이전, 지금의 섹터의 인덱스
	
	// 이얍!
	iW = pClient->X;	
	iH = pClient->Y;

	//oldIndex = pClient->pNow_Sector->_Index;
	//-------------------------------------------------------
	//  17.01.23
	//	2차원 인덱스로 보정
	//-------------------------------------------------------
	OldH = pClient->pNow_Sector->_iH;
	OldW = pClient->pNow_Sector->_iW;
	NewH = iH / dfSECTOR_SIZE;
	NewW = iW / dfSECTOR_SIZE;
	if (NewH == dfSECTOR_HEIGHT)
		NewH = dfSECTOR_HEIGHT - 1;
	if (NewW == dfSECTOR_WIDTH)
		NewW = dfSECTOR_WIDTH - 1;
	// index의 변화가 없다면, Sector를 움직일 필요가 없다.
	if ((OldH == NewH) && (OldW == NewW))
		return;	// 고로 리턴

//---------------------------------------------------------------------
//  17.01.19
//	Sector의 이동이 필요한 시점
//---------------------------------------------------------------------

	// 현재 나의 섹터에서 나를 제거하고
	list<stClient*>::iterator ITER_SECTOR = pClient->pNow_Sector->l_Sector_Client.begin();
	for (ITER_SECTOR; ITER_SECTOR != pClient->pNow_Sector->l_Sector_Client.end(); ++ITER_SECTOR)
	{
		if (*ITER_SECTOR == pClient)
		{
			pClient->pNow_Sector->l_Sector_Client.erase(ITER_SECTOR);
			break;
		}
	}

	// 새로 들어갈 섹터에는 나를 다시 집어넣는다.
	pClient->pOld_Sector = pClient->pNow_Sector;
	pClient->pNow_Sector = &g_Sector[NewH][NewW];
	// 내 캐릭터의 섹터이동, 섹터에 내 캐릭터의 삭제, 입실~ 완료		//	 17.01.23
	pClient->pNow_Sector->l_Sector_Client.push_back(pClient);
	

	// 이전섹터어라운드, 새로들어간섹터어라운드 비교
	//	oldIndex	== 이전에 있던 곳의 중심 섹터
	//	newIndex	== 지금 있는 곳의 중심 섹터
	
	//-------------------------------------------------------
	//  17.01.23
	//	2차원 섹터 변경, 코드 뜯어서 바꾸는 수준 ㅋ.ㅋ
	//-------------------------------------------------------
	
	stSector*	arrOld[9];
	stSector*	arrNew[9];
	stSector*	arrTempOld[9];

	// 올드의 주변, 뉴의 주변 섹터 인덱스 셋팅 완료...
	// 흠...
	int FixedOldH, FixedOldW;
	int FixedNewH, FixedNewW;
	
	int i = 0;
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			// 계산을 위한 변수
			FixedOldH = OldH + h;
			FixedOldW = OldW + w;
			FixedNewH = NewH + h;
			FixedNewW = NewW + w;
			
			// 마치 변수를 초기화하듯이... dfSECTOR_ARR_EMPTY = NULL 이다. 17.01.23
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

	//	이제 이걸 비교해서 뽑아낸다.
	int NEW;
	int OLD;

	//	이렇게 되면 너무 많은데;;
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
	//	arrOld 에는 내 캐릭터의 삭제패킷을 보내고,
	//	걔네들에 대한 삭제 패킷을 또 내가 나자신한테 보내야함.
	//
	//
	//	17.01.23
	//	2차원 섹터 변경
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
		
		// 내 캐릭터의 삭제패킷을 부리고...
		Send_Sector(pClient, arrOld[i], &SendMyData);
		
		// 다른 캐릭터의 삭제패킷을 내가 받는다.
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
	//	arrNew 에는 내 캐릭터의 생성패킷을 보내면 되고.
	//  새로운 섹터 애들의 생성패킷을 받아야한다.
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

		// 내 캐릭터의 생성 패킷을 뿌리고
		Send_Sector(pClient, arrNew[i], &SendMyData);
		// 내가 하고있던 행동을 보내야되는데
	//	Send_Sector(pClient, arrNew[i], &StopMsg);
		Send_Sector(pClient, arrNew[i], &SendMyMsg);

		//상대방의 데이터를 받는데
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
	//	돌면서 공격을 한 새끼라면... 처리해준다...
	////////////////////////////////////////////////////
	CProtoBuffer SendData;
	CProtoBuffer DeadData;
		if (pClient->byAttack != 0)
		{
			switch (pClient->Dir)
			{
				//왼쪽으로 공격
			case dfPACKET_MOVE_DIR_LL:
			case dfPACKET_MOVE_DIR_LU:
			case dfPACKET_MOVE_DIR_LD:
			{
				int	iLeftX = pClient->X - 90;
				int iRightX = pClient->X - 20;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// 내 섹터의 유저만
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
								//	더미 테스트를 위해 무한 재생시킨다...
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
								//	더미 테스트를 위해 무한 재생시킨다...
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
			//오른쪽으로 공격
			case dfPACKET_MOVE_DIR_RU:
			case dfPACKET_MOVE_DIR_RR:
			case dfPACKET_MOVE_DIR_RD:
			{
				int	iLeftX = pClient->X + 20;
				int iRightX = pClient->X + 90;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// 내 섹터의 유저만
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
								//	더미 테스트를 위해 무한 재생시킨다...
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
								//	더미 테스트를 위해 무한 재생시킨다...
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