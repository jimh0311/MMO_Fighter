#pragma once

#include "Global_Struct.h"
#include "__Log.h"
#include <list>
#include "Net_Lib\CProtoBuffer.h"
#include "PacketDefine.h"
#define dfSELECT_LIMIT 64
using namespace std;
//-------------------------------------------------------
//  17.01.18
//	네트워크 프록시져, 리시브를 받고, 샌드를 할 것이다.
//-------------------------------------------------------
void NetworkProc();

//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void AcceptProc();
//-------------------------------------------------------
//  17.01.18
//	연결을 종료하는 녀석.	// 일단, 이 함수는 사용금지
//-------------------------------------------------------
void DisConnectProc();
//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void RecvProc(stClient *in_pClient);
//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void SendProc(stClient *in_pClient);
//-------------------------------------------------------
//  17.01.18
//	
//-------------------------------------------------------
void DisAssembleRecvQueue(stClient *in_pClient);
//-------------------------------------------------------
//  17.01.18
//	Switch Packet	
//-------------------------------------------------------
void SwitchPacket(stClient *in_pClient, stPACKET_HEADER *pHead, CProtoBuffer *pData);
//-------------------------------------------------------
//  17.01.18
//	헤더, 데이터, EndCode 까지 완성된 직렬화 버퍼의 포인터가 올 것이다.
//-------------------------------------------------------
void Send_Uni(stClient *in_pDest, CProtoBuffer *in_pData);		// 해당 클라이언트 하나에게
void Send_Sector(stClient *in_pSrc, stSector *in_pSector, CProtoBuffer *in_pData);	// 해당 섹터에 나를 제외한 모두에게
void Send_Broad(stClient *in_pFrom, CProtoBuffer *in_pData);						// 전체 유저에게
void Send_SectorAround(stClient *in_pSrc, stSector *in_Center, CProtoBuffer *in_pData);
//
//
//
