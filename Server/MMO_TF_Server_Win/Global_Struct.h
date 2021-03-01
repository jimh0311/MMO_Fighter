#ifndef _GLOBAL_STRUCT_H_
#define _GLOBAL_STRUCT_H_

#include <Windows.h>
#include <list>
#include "Net_Lib\CStreamRQ.h"
using namespace std;

//-------------------------------------------------------
//  17.01.18
//	Session ���Ұ� ĳ���� ������ ���ÿ� ������ ����ü.
//-------------------------------------------------------


// ���簢�� 200x200�� ������� ����.
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
	DWORD	dwRecvTick;			//	���� ���� ������ ���� ���ؼ�...
	DWORD	dwActionTick;		//	DeadRecording ��
	SHORT	ActionX;			//
	SHORT	ActionY;			//
	BYTE	ActionMsg;

	SHORT	ServerFrame;
	SHORT	DeadFrame;

	//	�ϴ��� �̷��� �սô�.
	struct stSector *pNow_Sector;
	struct stSector *pOld_Sector;
};

//-------------------------------------------------------
//  17.01.18
//	���� ����ü.
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