#include "stdafx.h"
#include "MakePacket.h"
//#include "PacketDefine.h"
#include "NetWorkProc.h"
extern CPlayerObject*		g_Player;


//---------------------------------------------------------------
// 패킷헤더.
//
//---------------------------------------------------------------
/*
BYTE	byCode;			// 패킷코드 0x89 고정.
BYTE	bySize;			// 패킷 사이즈.  (실제 페이로드 사이즈, 헤더/End 코드 제외)
BYTE	byType;			// 패킷타입.
BYTE	byTemp;			// 사용안함.
*/

void MakePacket(DWORD dfACTION_MSG)
{
	st_NETWORK_PACKET_HEADER	stHeader;
	stHeader.byCode = 0x89;
	switch (dfACTION_MSG)
	{
		/////////////////////////////////////////////////////////////////
		//	움직이는 거 관련 패킷보내기...
		/////////////////////////////////////////////////////////////////
	case dfACTION_MOVE_LL :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LL;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_LU :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LU;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_UU :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_UU;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_RU :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RU;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_RR :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RR;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_RD :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_RD;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_DD :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_DD;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
	case dfACTION_MOVE_LD :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_START;
		stPACKET_CS_MOVE_START	stMoveData;
		stMoveData.Direction = dfACTION_MOVE_LD;
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
		/////////////////////////////////////////////////////////////////
		//	때리는 거 관련 패킷 보내기...
		/////////////////////////////////////////////////////////////////
	case dfACTION_ATTACK1 :
	{
		stHeader.bySize = sizeof(stPACKET_CS_ATTACK);
		stHeader.byType = dfPACKET_CS_ATTACK1;
		stPACKET_CS_ATTACK	stAttackData;
		//stAttackData.ID = g_Player->GetID();
		stAttackData.X = (SHORT)(g_Player->GetPosX());
		stAttackData.Y = (SHORT)(g_Player->GetPosY());
		if (g_Player->GetDirection() == 0)
		{
			stAttackData.DIR = dfACTION_MOVE_LL;
		}
		else
		{
			stAttackData.DIR = dfACTION_MOVE_RR;
		}
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stAttackData, sizeof(stPACKET_CS_ATTACK));
		break;
	}
	case dfACTION_ATTACK2 :
	{
		stHeader.bySize = sizeof(stPACKET_CS_ATTACK);
		stHeader.byType = dfPACKET_CS_ATTACK2;
		stPACKET_CS_ATTACK	stAttackData;
		//stAttackData.ID = g_Player->GetID();
		stAttackData.X = (SHORT)(g_Player->GetPosX());
		stAttackData.Y = (SHORT)(g_Player->GetPosY());
		if (g_Player->GetDirection() == 0)
		{
			stAttackData.DIR = dfACTION_MOVE_LL;
		}
		else
		{
			stAttackData.DIR = dfACTION_MOVE_RR;
		}
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stAttackData, sizeof(stPACKET_CS_ATTACK));
		break;
	}
	case dfACTION_ATTACK3 :
	{
		stHeader.bySize = sizeof(stPACKET_CS_ATTACK);
		stHeader.byType = dfPACKET_CS_ATTACK3;
		stPACKET_CS_ATTACK	stAttackData;
		//stAttackData.ID = g_Player->GetID();
		stAttackData.X = (SHORT)(g_Player->GetPosX());
		stAttackData.Y = (SHORT)(g_Player->GetPosY());
		if (g_Player->GetDirection() == 0)
		{
			stAttackData.DIR = dfACTION_MOVE_LL;
		}
		else
		{
			stAttackData.DIR = dfACTION_MOVE_RR;
		}
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stAttackData, sizeof(stPACKET_CS_ATTACK));
		break;
	}
	case dfACTION_PUSH :
		/////////////////////////////////////////////////////////////////
		//	멈추는 거 패킷 보내기...
		/////////////////////////////////////////////////////////////////
	case dfACTION_STAND :
	{
		stHeader.bySize = sizeof(stPACKET_CS_MOVE_START);
		stHeader.byType = dfPACKET_CS_MOVE_STOP;
		stPACKET_CS_MOVE_START	stMoveData;
		// LEFT == 0 ; RIGHT == 1;
		if (g_Player->GetDirection() == 0)
		{
			stMoveData.Direction = dfPACKET_MOVE_DIR_LL;
		}
		else
		{
			stMoveData.Direction = dfPACKET_MOVE_DIR_RR;
		}
		stMoveData.X = (SHORT)(g_Player->GetPosX());
		stMoveData.Y = (SHORT)(g_Player->GetPosY());
		SendProc(stHeader, dfPACKET_SIZE, (char*)&stMoveData, sizeof(stPACKET_CS_MOVE_START));
		break;
	}
		break;
	}
}