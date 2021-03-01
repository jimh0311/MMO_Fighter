#ifndef _GLOBAL_STRUCT_H_
#define _GLOBAL_STRUCT_H_

#include <Windows.h>
#include <list>
#include "Net_Lib\CStreamRQ.h"
using namespace std;

//-------------------------------------------------------
//  17.01.18
//	Session 역할과 캐릭터 역할을 동시에 수행할 구조체.
//-------------------------------------------------------


// 정사각형 200x200을 사이즈로 간다.
#define dfSECTOR_SIZE 200
#define dfSECTOR_WIDTH (6400/dfSECTOR_SIZE)
#define dfSECTOR_HEIGHT (6400/dfSECTOR_SIZE)
#define dfSECTOR_ARR_EMPTY NULL
struct stClient
{
	SOCKET	hSocket;
	DWORD	ID;
	CStreamRQ	rqR;
	CStreamRQ	rqS;
	BYTE	Dir;
	BYTE	Msg;
	SHORT	X;
	SHORT	Y;
	BYTE	HP;
	int		SendCount;
	int		RecvCount;
	BYTE	byAttack;
	BOOL	bDisconn;
	DWORD	dwRecvTick;			//	반응 없는 유저를 끊기 위해서...
	DWORD	dwActionTick;		//	DeadRecording 용
	SHORT	ActionX;			//
	SHORT	ActionY;			//
	BYTE	ActionMsg;

	SHORT	ServerFrame;
	SHORT	DeadFrame;

	//	일단은 이렇게 합시다.
	struct stSector *pNow_Sector;
	struct stSector *pOld_Sector;
};

//-------------------------------------------------------
//  17.01.18
//	섹터 구조체.
//-------------------------------------------------------

struct stSector
{
	int _iLeft;
	int _iTop;
	int _iW;
	int _iH;
	list<stClient*> l_Sector_Client;
};
#endif