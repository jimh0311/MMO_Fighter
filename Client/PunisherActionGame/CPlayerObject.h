#pragma once
#include "stdafx.h"
class CPlayerObject : public CBaseObject
{
	/*
	CBaseObject가 가지고 있는 정보들
	위치정보.
	타입정보.
	Virtual 을 통해 행동한다.
	*/
	enum enDIRECTION
	{
		LEFT = false,
		RIGHT = true
	};
	
public:
	CPlayerObject(int id, int type, int x, int y, bool isMy, int hp) :CBaseObject(id, type, x, y)
	{
		_bIsMyPlayer = isMy;
		_iHP = hp;
		_bDirection = LEFT;

		_dwActionNow = dfACTION_STAND;
		SetActionStand();
	}
	bool Action();
	bool Draw();

	// ActionMsg 를 처리할 함수
	bool ActionMsgProc();

	//Sprite 움직임을 만들기 위한 시발놈아?
	void SetActionMove();
	void SetActionStand();
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	
	void SetHP(int hp)
	{
		_iHP = hp;
	}
	bool IsMyPlayer()
	{
		return _bIsMyPlayer;
	}
	bool GetDirection()
	{
		return _bDirection;
	}
	void Demaged(int hp)
	{
		_iHP = hp;
	}
private:
	bool _bIsMyPlayer;	// TRUE 면 내 유닛, FALSE 면 다른 유닛.
	int _iHP;
	//
	DWORD _dwActionNow;
	DWORD _dwActionOld;
	//
	//
	bool _bDirection;	// LEFT == 0 ; RIGHT == 1;
};