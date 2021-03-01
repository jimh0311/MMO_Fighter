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
//	��Ʈ��ũ ���Ͻ���, ���ú긦 �ް�, ���带 �� ���̴�.
//-------------------------------------------------------
void NetworkProc();

//-------------------------------------------------------
//  17.01.18
//
//-------------------------------------------------------
void AcceptProc();
//-------------------------------------------------------
//  17.01.18
//	������ �����ϴ� �༮.	// �ϴ�, �� �Լ��� ������
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
//	���, ������, EndCode ���� �ϼ��� ����ȭ ������ �����Ͱ� �� ���̴�.
//-------------------------------------------------------
void Send_Uni(stClient *in_pDest, CProtoBuffer *in_pData);		// �ش� Ŭ���̾�Ʈ �ϳ�����
void Send_Sector(stClient *in_pSrc, stSector *in_pSector, CProtoBuffer *in_pData);	// �ش� ���Ϳ� ���� ������ ��ο���
void Send_Broad(stClient *in_pFrom, CProtoBuffer *in_pData);						// ��ü ��������
void Send_SectorAround(stClient *in_pSrc, stSector *in_Center, CProtoBuffer *in_pData);
//
//
//
