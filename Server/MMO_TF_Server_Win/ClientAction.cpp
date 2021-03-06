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
//	叔薦 蕉級聖 坦軒拝 橿汐
//-------------------------------------------------------
void Update()
{

	//-------------------------------------------------------
	//  17.01.18
	//	X,Y 乞砧 40 Pixel 舛亀 堂嬢走壱 赤製
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
//	蕉級聖 崇送析 橿汐
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
	//	辞獄 覗傾績引 汽球傾朕閑 覗傾績戚 杖原蟹 堂嬢走澗 走 左切
	//-------------------------------------------------------
	pClient->ServerFrame++;
}
//-------------------------------------------------------
//  17.01.18
//	Calc Sector
//
//	蝉旭精 ! せせせせせ 獣鴻 せせせせせ
//-------------------------------------------------------
void CalcAroundSector(stClient	*pClient)
{
	//iX 研 閤壱, iY研 閤澗陥.
	int iH, iW;	//	蝶遣斗税 疎妊
	int NewH, NewW, OldH, OldW;	//	戚穿, 走榎税 悉斗税 昔畿什
	
	// 戚上!
	iW = pClient->X;	
	iH = pClient->Y;

	//oldIndex = pClient->pNow_Sector->_Index;
	//-------------------------------------------------------
	//  17.01.23
	//	2託据 昔畿什稽 左舛
	//-------------------------------------------------------
	OldH = pClient->pNow_Sector->_iH;
	OldW = pClient->pNow_Sector->_iW;
	NewH = iH / dfSECTOR_SIZE;
	NewW = iW / dfSECTOR_SIZE;
	if (NewH == dfSECTOR_HEIGHT)
		NewH = dfSECTOR_HEIGHT - 1;
	if (NewW == dfSECTOR_WIDTH)
		NewW = dfSECTOR_WIDTH - 1;
	// index税 痕鉢亜 蒸陥檎, Sector研 崇送析 琶推亜 蒸陥.
	if ((OldH == NewH) && (OldW == NewW))
		return;	// 壱稽 軒渡

//---------------------------------------------------------------------
//  17.01.19
//	Sector税 戚疑戚 琶推廃 獣繊
//---------------------------------------------------------------------

	// 薄仙 蟹税 悉斗拭辞 蟹研 薦暗馬壱
	list<stClient*>::iterator ITER_SECTOR = pClient->pNow_Sector->l_Sector_Client.begin();
	for (ITER_SECTOR; ITER_SECTOR != pClient->pNow_Sector->l_Sector_Client.end(); ++ITER_SECTOR)
	{
		if (*ITER_SECTOR == pClient)
		{
			pClient->pNow_Sector->l_Sector_Client.erase(ITER_SECTOR);
			break;
		}
	}

	// 歯稽 級嬢哀 悉斗拭澗 蟹研 陥獣 増嬢隔澗陥.
	pClient->pOld_Sector = pClient->pNow_Sector;
	pClient->pNow_Sector = &g_Sector[NewH][NewW];
	// 鎧 蝶遣斗税 悉斗戚疑, 悉斗拭 鎧 蝶遣斗税 肢薦, 脊叔~ 刃戟		//	 17.01.23
	pClient->pNow_Sector->l_Sector_Client.push_back(pClient);
	

	// 戚穿悉斗嬢虞錘球, 歯稽級嬢娃悉斗嬢虞錘球 搾嘘
	//	oldIndex	== 戚穿拭 赤揮 員税 掻宿 悉斗
	//	newIndex	== 走榎 赤澗 員税 掻宿 悉斗
	
	//-------------------------------------------------------
	//  17.01.23
	//	2託据 悉斗 痕井, 坪球 金嬢辞 郊荷澗 呪層 せ.せ
	//-------------------------------------------------------
	
	stSector*	arrOld[9];
	stSector*	arrNew[9];
	stSector*	arrTempOld[9];

	// 臣球税 爽痕, 敢税 爽痕 悉斗 昔畿什 実特 刃戟...
	// 緋...
	int FixedOldH, FixedOldW;
	int FixedNewH, FixedNewW;
	
	int i = 0;
	for (int h = -1; h < 2; h++)
	{
		for (int w = -1; w < 2; w++)
		{
			// 域至聖 是廃 痕呪
			FixedOldH = OldH + h;
			FixedOldW = OldW + w;
			FixedNewH = NewH + h;
			FixedNewW = NewW + w;
			
			// 原帖 痕呪研 段奄鉢馬牛戚... dfSECTOR_ARR_EMPTY = NULL 戚陥. 17.01.23
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

	//	戚薦 戚杏 搾嘘背辞 嗣焼浬陥.
	int NEW;
	int OLD;

	//	戚係惟 鞠檎 格巷 弦精汽;;
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
	//	arrOld 拭澗 鎧 蝶遣斗税 肢薦鳶填聖 左鎧壱,
	//	安革級拭 企廃 肢薦 鳶填聖 暁 鎧亜 蟹切重廃砺 左鎧醤敗.
	//
	//
	//	17.01.23
	//	2託据 悉斗 痕井
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
		
		// 鎧 蝶遣斗税 肢薦鳶填聖 採軒壱...
		Send_Sector(pClient, arrOld[i], &SendMyData);
		
		// 陥献 蝶遣斗税 肢薦鳶填聖 鎧亜 閤澗陥.
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
	//	arrNew 拭澗 鎧 蝶遣斗税 持失鳶填聖 左鎧檎 鞠壱.
	//  歯稽錘 悉斗 蕉級税 持失鳶填聖 閤焼醤廃陥.
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

		// 鎧 蝶遣斗税 持失 鳶填聖 姿軒壱
		Send_Sector(pClient, arrNew[i], &SendMyData);
		// 鎧亜 馬壱赤揮 楳疑聖 左鎧醤鞠澗汽
	//	Send_Sector(pClient, arrNew[i], &StopMsg);
		Send_Sector(pClient, arrNew[i], &SendMyMsg);

		//雌企号税 汽戚斗研 閤澗汽
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
	//	宜檎辞 因維聖 廃 歯晦虞檎... 坦軒背層陥...
	////////////////////////////////////////////////////
	CProtoBuffer SendData;
	CProtoBuffer DeadData;
		if (pClient->byAttack != 0)
		{
			switch (pClient->Dir)
			{
				//図楕生稽 因維
			case dfPACKET_MOVE_DIR_LL:
			case dfPACKET_MOVE_DIR_LU:
			case dfPACKET_MOVE_DIR_LD:
			{
				int	iLeftX = pClient->X - 90;
				int iRightX = pClient->X - 20;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// 鎧 悉斗税 政煽幻
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
								//	希耕 砺什闘研 是背 巷廃 仙持獣轍陥...
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
								//	希耕 砺什闘研 是背 巷廃 仙持獣轍陥...
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
			//神献楕生稽 因維
			case dfPACKET_MOVE_DIR_RU:
			case dfPACKET_MOVE_DIR_RR:
			case dfPACKET_MOVE_DIR_RD:
			{
				int	iLeftX = pClient->X + 20;
				int iRightX = pClient->X + 90;
				int iTopY = pClient->Y - 15;
				int iBottomY = pClient->Y + 15;

				// 鎧 悉斗税 政煽幻
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
								//	希耕 砺什闘研 是背 巷廃 仙持獣轍陥...
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
								//	希耕 砺什闘研 是背 巷廃 仙持獣轍陥...
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