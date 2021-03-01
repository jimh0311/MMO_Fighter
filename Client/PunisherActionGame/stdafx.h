// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


/*****************************************************************

건전한 정의 목록
1. 화면 크기 정의
2. 메세지 정의
3. 프레임 딜레이 정의
4. 객체 타입 정의

*****************************************************************/

// 화면 정의
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

// 객체에게 전달될 메세지 정의
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

//	프레임 관리를 위한 디파인
#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	4
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

//
#define dfPACKET_SIZE	4
// 객체의 타입 정의
enum enOBJECT_TYPE
{
	enPLAYER = 0,
	enEFFECT
};


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <Windowsx.h>
#include "ScreenDIB.h"
#include "SpriteDIB.h"
#include "CFrameControl.h"
#include "LinkedList.h"

#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CSpark.h"

