// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


/*****************************************************************

������ ���� ���
1. ȭ�� ũ�� ����
2. �޼��� ����
3. ������ ������ ����
4. ��ü Ÿ�� ����

*****************************************************************/

// ȭ�� ����
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

// ��ü���� ���޵� �޼��� ����
#define dfACTION_MOVE_LL 0
#define dfACTION_MOVE_LU 1
#define dfACTION_MOVE_UU 2
#define dfACTION_MOVE_RU 3
#define dfACTION_MOVE_RR 4
#define dfACTION_MOVE_RD 5
#define dfACTION_MOVE_DD 6
#define dfACTION_MOVE_LD 7
#define dfACTION_ATTACK1 8
#define dfACTION_ATTACK2 9
#define dfACTION_ATTACK3 10
#define dfACTION_PUSH 11
#define dfACTION_STAND 12

//	������ ������ ���� ������
#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	4
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

//
#define dfPACKET_SIZE	4
// ��ü�� Ÿ�� ����
enum enOBJECT_TYPE
{
	enPLAYER = 0,
	enEFFECT
};


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <Windowsx.h>
#include "ScreenDIB.h"
#include "SpriteDIB.h"
#include "CFrameControl.h"
#include "LinkedList.h"

#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CSpark.h"

