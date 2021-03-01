#pragma once
#include "CStreamRQ.h"
#include "PacketDefine.h"
#include <Windows.h>

void	PackitProc();
void	SendProc(st_NETWORK_PACKET_HEADER pHeader, int iHeaderSize, char* pDataLoad, int iDataSize);
void	RecvProc(SOCKET sock);