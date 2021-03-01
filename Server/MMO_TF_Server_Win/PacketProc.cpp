
#include "PacketProc.h"
#include "PacketDefine.h"
#include "NetworkProc.h"

#define MAX_LEFT 0
#define MAX_TOP 0
#define MAX_RIGHT 6400
#define MAX_BOTTOM 6400

#define dfSYNC_LIMIT	30

extern void CalcAroundSector(stClient	*pClient);

//-------------------------------------------------------
//  17.01.18
//	복사 붙여넣기 함
//	그나저나, 연산자 오버로딩이 되어있어서, 상관없음
//-------------------------------------------------------
CProtoBuffer MakePacket(stClient* pSess, BYTE msg)
{
	CProtoBuffer Packit;
	if (pSess == NULL)
	{
		__Log(dfLOG_SHUTDOWN, L"MakePacket, pClient = NULL !!!");
		return NULL;
	}
		

	switch (msg)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)10 << (BYTE)dfPACKET_SC_CREATE_MY_CHARACTER << (BYTE)0;
		Packit << pSess->ID << (BYTE)dfPACKET_CS_MOVE_STOP << pSess->X << pSess->Y << pSess->HP << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)10 << (BYTE)dfPACKET_SC_CREATE_OTHER_CHARACTER << (BYTE)0;
		Packit << pSess->ID << (BYTE)dfPACKET_CS_ATTACK3 << pSess->X << pSess->Y << pSess->HP << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_DELETE_CHARACTER:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)4 << (BYTE)dfPACKET_SC_DELETE_CHARACTER << (BYTE)0;
		Packit << pSess->ID << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_MOVE_START:
	{
		//헤더
		Packit << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_MOVE_START << (BYTE)0;
		Packit << pSess->ID << pSess->Msg << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_MOVE_STOP:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_MOVE_STOP << (BYTE)0;
		Packit << pSess->ID << pSess->Msg << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_ATTACK1:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_ATTACK1 << (BYTE)0;
		Packit << pSess->ID << pSess->Msg << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_ATTACK2:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_ATTACK2 << (BYTE)0;
		Packit << pSess->ID << pSess->Msg << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_ATTACK3:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)9 << (BYTE)dfPACKET_SC_ATTACK3 << (BYTE)0;
		Packit << pSess->ID << pSess->Msg << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}
	case dfPACKET_SC_SYNC:
	{
		Packit << dfNETWORK_PACKET_CODE << (BYTE)8 << (BYTE)dfPACKET_SC_SYNC << dfNETWORK_PACKET_TEMP;
		Packit << pSess->ID << pSess->X << pSess->Y << dfNETWORK_PACKET_END;
		return Packit;
	}

	}
	__Log(dfLOG_SHUTDOWN, L"MakePacket, pClient = NULL !!!");
	return NULL;
}

CProtoBuffer MakeMsgPacket(stClient* pClient)
{
	switch (pClient->Msg)
	{
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_UU:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RD:
	case dfPACKET_MOVE_DIR_DD:
	case dfPACKET_MOVE_DIR_LD:
		return MakePacket(pClient, dfPACKET_SC_MOVE_START);
		break;

	case dfPACKET_CS_MOVE_STOP:
	case dfPACKET_CS_ATTACK1:
	case dfPACKET_CS_ATTACK2:
	case dfPACKET_CS_ATTACK3:
		return MakePacket(pClient, dfPACKET_SC_MOVE_STOP);
		break;
	}

	return NULL;
}



//------------------------------------------------------
//	지민혁 17.01.24
//
//	데드레코딩
//------------------------------------------------------
void DeadRecording(stClient *in_pClient)
{

	//-------------------------------------------------------
	//  17.01.25
	//	넘 단순하게 여기서 timeGetTime을 하고 있으니 에러가 생길 수 밖에 없지 않니?
	//-------------------------------------------------------

	DWORD dwIntervalTime = timeGetTime() - in_pClient->dwActionTick;

	//-------------------------------------------------------
	//  17.01.25
	//	서버와 데드레커닝 프레임 차이좀 보고싶어서 넣음
	//-------------------------------------------------------

	int iActionFrame = dwIntervalTime / 20;	// 50 프레임 기준으로 가는 듯?

	//-------------------------------------------------------
	//  17.01.25
	//
	//-------------------------------------------------------
	in_pClient->DeadFrame = iActionFrame;
	//
	//
	//
	int iRemoveFrame = 0;

	int iRPosX = in_pClient->ActionX;
	int iRPosY = in_pClient->ActionY;

	int iValue = 0;

	int iDistanceX = iActionFrame * 3;
	int iDistanceY = iActionFrame * 2;
	switch (in_pClient->Msg)
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

	if (iRPosX <= MAX_LEFT)
	{
		iValue = abs(MAX_LEFT - abs(iRPosX)) / 3;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosX >= MAX_RIGHT)
	{
		iValue = abs(MAX_RIGHT - iRPosX) / 3;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY <= MAX_TOP)
	{
		iValue = abs(MAX_TOP - abs(iRPosY)) / 2;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY >= MAX_BOTTOM)
	{
		iValue = abs(MAX_BOTTOM - iRPosY) / 2;
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
		in_pClient->DeadFrame = iActionFrame;
		
		//보정된좌표 다시 계산
		iDistanceX = iActionFrame * 3;
		iDistanceY = iActionFrame * 2;

		iRPosX = in_pClient->ActionX;
		iRPosY = in_pClient->ActionY;

		switch (in_pClient->Msg)
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

	iRPosX = min(iRPosX, MAX_RIGHT);
	iRPosX = max(iRPosX, MAX_LEFT);
	iRPosY = min(iRPosY, MAX_BOTTOM);
	iRPosY = max(iRPosY, MAX_TOP);

	in_pClient->X = iRPosX;
	in_pClient->Y = iRPosY;

	return;
}



//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 이동
//-------------------------------------------------------
void Proc_PACKET_CS_MOVE_START(stClient *in_pClient, CProtoBuffer *pData)
{
	BYTE Dir;
	SHORT X, Y;		//	클라이언트가 보내준 정보

	*pData >> Dir >> X >> Y; 

	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	이제 싱크를 보내는 조건을 컨트롤 해보자.
	//------------------------------------------------------
	int SyncX, SyncY;
	int OldX, OldY;
	OldX = in_pClient->X;
	OldY = in_pClient->Y;

	SyncX = in_pClient->X - X;
	SyncY = in_pClient->Y - Y;

	CProtoBuffer SendData;
	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	DeadRecording을 해줘야 하는 상황이라면?
	//------------------------------------------------------
	if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
	{
		
		DeadRecording(in_pClient);
		CalcAroundSector(in_pClient);

		SyncX = in_pClient->X - X;
		SyncY = in_pClient->Y - Y;
		
		//이라면...
		if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
		{
			__Log(dfLOG_WARNING, L"Sync USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
			__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
			if (abs(SyncX) + abs(SyncY) > 100)
			{
				__Log(dfLOG_WARNING, L"!!!!!INCORRECT USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
				__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
				
				//-------------------------------------------------------
				//  17.01.25
				//	MMO 스타일이기 때문에, 튀는거는 그대로 냅둔다.
				//-------------------------------------------------------
				//in_pClient->bDisconn = TRUE;
			}
			SendData = MakePacket(in_pClient, dfPACKET_SC_SYNC);
			Send_Uni(in_pClient, &SendData);
			Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);
			SendData.Clear();
		}
	}
	else
	{
		//-------------------------------------------------------
		//  17.01.25
		//	선생님의 추가설명 추가함...
		//-------------------------------------------------------
		in_pClient->X = X;
		in_pClient->Y = Y;
		CalcAroundSector(in_pClient);
	}

	//-------------------------------------------------------
	//  17.01.25
	//	얘로 움직여봄
	//-------------------------------------------------------
	//in_pClient->dwActionTick = timeGetTime();
	in_pClient->dwActionTick = in_pClient->dwRecvTick;
	in_pClient->ActionX = in_pClient->X;
	in_pClient->ActionY = in_pClient->Y;
	
	// 보내기 직전에 얘로 교체해줘야 한다.
	in_pClient->Dir = Dir;
	in_pClient->Msg = Dir;

	SendData = MakePacket(in_pClient, dfPACKET_SC_MOVE_START);
	Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);

	//-------------------------------------------------------
	//  17.01.23
	//	여기서 Action의 상태를 바꿈으로, dwActionTick을 갱신해준다.
	//-------------------------------------------------------
	

	//-------------------------------------------------------
	//  17.01.25
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	in_pClient->ServerFrame = 0;
	in_pClient->DeadFrame = 0;
}


//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 멈춤
//-------------------------------------------------------
void Proc_PACKET_CS_MOVE_STOP(stClient *in_pClient, CProtoBuffer *pData)
{
	BYTE Dir;
	SHORT X, Y;

	*pData >> Dir >> X >> Y;

	CProtoBuffer SendData;

	
	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	이제 싱크를 보내는 조건을 컨트롤 해보자.
	//------------------------------------------------------
	int OldX, OldY;
	OldX = in_pClient->X;
	OldY = in_pClient->Y;

	int SyncX, SyncY;
	SyncX = in_pClient->X - X;
	SyncY = in_pClient->Y - Y;
	if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
	{
		
		DeadRecording(in_pClient);
		CalcAroundSector(in_pClient);

		SyncX = in_pClient->X - X;
		SyncY = in_pClient->Y - Y;
		//이라면...
		if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
		{
			__Log(dfLOG_WARNING, L"Sync USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
			__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
			if (abs(SyncX) + abs(SyncY) > 100)
			{
				__Log(dfLOG_WARNING, L"!!!!!INCORRECT USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
				__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
				//-------------------------------------------------------
				//  17.01.25
				//	MMO 스타일이기 때문에, 튀는거는 그대로 냅둔다.
				//-------------------------------------------------------
				//in_pClient->bDisconn = TRUE;
			}
			SendData = MakePacket(in_pClient, dfPACKET_SC_SYNC);
			Send_Uni(in_pClient, &SendData);
			Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);
			SendData.Clear();
		}
	}
	else
	{
		//-------------------------------------------------------
		//  17.01.25
		//	선생님의 추가설명 추가함...
		//-------------------------------------------------------
		in_pClient->X = X;
		in_pClient->Y = Y;
		CalcAroundSector(in_pClient);
	}
	//-------------------------------------------------------
	//  17.01.25
	//	얘로 움직여봄
	//-------------------------------------------------------
	//in_pClient->dwActionTick = timeGetTime();
	in_pClient->dwActionTick = in_pClient->dwRecvTick;
	in_pClient->ActionX = in_pClient->X;
	in_pClient->ActionY = in_pClient->Y;

	in_pClient->Dir = Dir;
	in_pClient->Msg = dfPACKET_CS_MOVE_STOP;

	SendData = MakePacket(in_pClient, dfPACKET_SC_MOVE_STOP);
	Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);

	//-------------------------------------------------------
	//  17.01.23
	//	여기서 Action의 상태를 바꿈으로, dwActionTick을 갱신해준다.
	//-------------------------------------------------------
	

	//-------------------------------------------------------
	//  17.01.25
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	in_pClient->ServerFrame = 0;
	in_pClient->DeadFrame = 0;
}
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격 1
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK1(stClient *in_pClient, CProtoBuffer *pData)
{
	BYTE Dir;
	SHORT X, Y;

	*pData >> Dir >> X >> Y;


	CProtoBuffer SendData;
	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	이제 싱크를 보내는 조건을 컨트롤 해보자.
	//------------------------------------------------------
	int OldX, OldY;
	OldX = in_pClient->X;
	OldY = in_pClient->Y;

	int SyncX, SyncY;
	SyncX = in_pClient->X - X;
	SyncY = in_pClient->Y - Y;
	if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
	{
		DeadRecording(in_pClient);
		CalcAroundSector(in_pClient);

		SyncX = in_pClient->X - X;
		SyncY = in_pClient->Y - Y;
		//이라면...
		if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
		{
			__Log(dfLOG_WARNING, L"Sync USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
			__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
			if (abs(SyncX) + abs(SyncY) > 100)
			{
				__Log(dfLOG_WARNING, L"!!!!!INCORRECT USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
				__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
				//-------------------------------------------------------
				//  17.01.25
				//	MMO 스타일이기 때문에, 튀는거는 그대로 냅둔다.
				//-------------------------------------------------------
				//in_pClient->bDisconn = TRUE;
			}
			SendData = MakePacket(in_pClient, dfPACKET_SC_SYNC);
			Send_Uni(in_pClient, &SendData);
			Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);
			SendData.Clear();
		}
	}
	else
	{
		//-------------------------------------------------------
		//  17.01.25
		//	선생님의 추가설명 추가함...
		//-------------------------------------------------------
		in_pClient->X = X;
		in_pClient->Y = Y;
		CalcAroundSector(in_pClient);
	}
	//-------------------------------------------------------
	//  17.01.25
	//	얘로 움직여봄
	//-------------------------------------------------------
	//in_pClient->dwActionTick = timeGetTime();
	in_pClient->dwActionTick = in_pClient->dwRecvTick;
	in_pClient->ActionX = in_pClient->X;
	in_pClient->ActionY = in_pClient->Y;


	in_pClient->Msg = dfPACKET_CS_ATTACK1;
	in_pClient->Dir = Dir;


	SendData = MakePacket(in_pClient, dfPACKET_SC_ATTACK1);
	Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);


	//-------------------------------------------------------
	//  17.01.25
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	in_pClient->ServerFrame = 0;
	in_pClient->DeadFrame = 0;
	
}
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격2
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK2(stClient *in_pClient, CProtoBuffer *pData)
{
	BYTE Dir;
	SHORT X, Y;

	*pData >> Dir >> X >> Y;


	CProtoBuffer SendData;
	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	이제 싱크를 보내는 조건을 컨트롤 해보자.
	//------------------------------------------------------
	int OldX, OldY;
	OldX = in_pClient->X;
	OldY = in_pClient->Y;

	int SyncX, SyncY;
	SyncX = in_pClient->X - X;
	SyncY = in_pClient->Y - Y;
	if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
	{
		DeadRecording(in_pClient);
		CalcAroundSector(in_pClient);

		SyncX = in_pClient->X - X;
		SyncY = in_pClient->Y - Y;
		//이라면...
		if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
		{
			__Log(dfLOG_WARNING, L"Sync USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
			__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
			if (abs(SyncX) + abs(SyncY) > 100)
			{
				__Log(dfLOG_WARNING, L"!!!!!INCORRECT USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
				__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
				//-------------------------------------------------------
				//  17.01.25
				//	MMO 스타일이기 때문에, 튀는거는 그대로 냅둔다.
				//-------------------------------------------------------
				//in_pClient->bDisconn = TRUE;
			}
			SendData = MakePacket(in_pClient, dfPACKET_SC_SYNC);
			Send_Uni(in_pClient, &SendData);
			Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);
			SendData.Clear();
		}
	}
	else
	{
		//-------------------------------------------------------
		//  17.01.25
		//	선생님의 추가설명 추가함...
		//-------------------------------------------------------
		in_pClient->X = X;
		in_pClient->Y = Y;
		CalcAroundSector(in_pClient);
	}

	//-------------------------------------------------------
	//  17.01.25
	//	얘로 움직여봄
	//-------------------------------------------------------
	//in_pClient->dwActionTick = timeGetTime();
	in_pClient->dwActionTick = in_pClient->dwRecvTick;
	in_pClient->ActionX = in_pClient->X;
	in_pClient->ActionY = in_pClient->Y;

	in_pClient->Msg = dfPACKET_CS_ATTACK2;
	in_pClient->Dir = Dir;

	SendData = MakePacket(in_pClient, dfPACKET_SC_ATTACK2);
	Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);


	//-------------------------------------------------------
	//  17.01.23
	//	여기서 Action의 상태를 바꿈으로, dwActionTick을 갱신해준다.
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	//  17.01.25
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	in_pClient->ServerFrame = 0;
	in_pClient->DeadFrame = 0;
}
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격3
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK3(stClient *in_pClient, CProtoBuffer *pData)
{
	BYTE Dir;
	SHORT X, Y;

	*pData >> Dir >> X >> Y;


	CProtoBuffer SendData;
	//------------------------------------------------------
	//	지민혁 17.01.24
	//
	//	이제 싱크를 보내는 조건을 컨트롤 해보자.
	//------------------------------------------------------
	int OldX, OldY;
	OldX = in_pClient->X;
	OldY = in_pClient->Y;

	int SyncX, SyncY;
	SyncX = in_pClient->X - X;
	SyncY = in_pClient->Y - Y;
	if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
	{
		
		DeadRecording(in_pClient);
		CalcAroundSector(in_pClient);

		SyncX = in_pClient->X - X;
		SyncY = in_pClient->Y - Y;
		//이라면...
		if (abs(SyncX) >= dfSYNC_LIMIT || abs(SyncY) >= dfSYNC_LIMIT)
		{
			__Log(dfLOG_WARNING, L"Sync USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
			__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
			if (abs(SyncX) + abs(SyncY) > 100)
			{
				__Log(dfLOG_WARNING, L"!!!!!INCORRECT USER(%d) Server OLD(%d,%d) DEAD(%d,%d) Dir(%d) Msg(%d)", in_pClient->ID, OldX, OldY, in_pClient->X, in_pClient->Y, in_pClient->Dir, in_pClient->Msg);
				__Log(dfLOG_WARNING, L"Client(%d,%d) Dir(%d) ServerFrame(%d) DeadFrame(%d)", X, Y, Dir, in_pClient->ServerFrame, in_pClient->DeadFrame);
				//-------------------------------------------------------
				//  17.01.25
				//	MMO 스타일이기 때문에, 튀는거는 그대로 냅둔다.
				//-------------------------------------------------------
				//in_pClient->bDisconn = TRUE;
			}
			SendData = MakePacket(in_pClient, dfPACKET_SC_SYNC);
			Send_Uni(in_pClient, &SendData);
			Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);
			SendData.Clear();
		}
	}
	else
	{
		//-------------------------------------------------------
		//  17.01.25
		//	선생님의 추가설명 추가함...
		//-------------------------------------------------------
		in_pClient->X = X;
		in_pClient->Y = Y;
		CalcAroundSector(in_pClient);
	}


	//-------------------------------------------------------
	//  17.01.25
	//	얘로 움직여봄
	//-------------------------------------------------------
	//in_pClient->dwActionTick = timeGetTime();
	in_pClient->dwActionTick = in_pClient->dwRecvTick;
	in_pClient->ActionX = in_pClient->X;
	in_pClient->ActionY = in_pClient->Y;

	in_pClient->Msg = dfPACKET_CS_ATTACK3;
	in_pClient->Dir = Dir;

	SendData = MakePacket(in_pClient, dfPACKET_SC_ATTACK3);
	Send_SectorAround(in_pClient, in_pClient->pNow_Sector, &SendData);

	//-------------------------------------------------------
	//  17.01.23
	//	여기서 Action의 상태를 바꿈으로, dwActionTick을 갱신해준다.
	//-------------------------------------------------------



	//-------------------------------------------------------
	//  17.01.25
	//	서버 프레임과 데드레커닝 프레임이 얼마나 틀어지는 지 보자
	//-------------------------------------------------------
	in_pClient->ServerFrame = 0;
	in_pClient->DeadFrame = 0;
}

