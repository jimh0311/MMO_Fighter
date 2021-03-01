#pragma once
#include "Net_Lib\CStreamRQ.h"
#include <Windows.h>


struct stSession
{
	SOCKET		Socket;
	DWORD		ID;
	SHORT		shX, shY;
	BYTE		dir;
	BYTE		msg;
	CStreamRQ	rqR;
	CStreamRQ	rqS;


	DWORD		dwActionTick;

	BOOL bConnect;
};