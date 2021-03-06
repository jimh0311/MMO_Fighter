#pragma once

#include "Net_Lib\CProtoBuffer.h"
#include "Global_Struct.h"
//-------------------------------------------------------
//  17.01.18
//	이전에 만든거 복/붙함
//-------------------------------------------------------
CProtoBuffer MakePacket(stClient* pSess, BYTE msg);
CProtoBuffer MakeMsgPacket(stClient* pClient);


//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 이동
//-------------------------------------------------------
void Proc_PACKET_CS_MOVE_START(stClient *in_pClient, CProtoBuffer *pData);
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 멈춤
//-------------------------------------------------------
void Proc_PACKET_CS_MOVE_STOP(stClient *in_pClient, CProtoBuffer *pData);
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격 1
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK1(stClient *in_pClient, CProtoBuffer *pData);
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격2
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK2(stClient *in_pClient, CProtoBuffer *pData);
//-------------------------------------------------------
//  17.01.18
//	클라에서 서버로 보낸 패킷
//	캐릭터 공격3
//-------------------------------------------------------
void Proc_PACKET_CS_ATTACK3(stClient *in_pClient, CProtoBuffer *pData);

//------------------------------------------------------
//	지민혁 17.01.24
//
//	데드레코딩
//------------------------------------------------------
void DeadRecording(stClient *in_pClient);


