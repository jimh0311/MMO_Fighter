#pragma once
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

void MakePacket(DWORD dfACTION_MSG);
