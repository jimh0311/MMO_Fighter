#pragma once
#include "stdafx.h"
class CPlayerObject : public CBaseObject
{
	/*
	CBaseObject�� ������ �ִ� ������
	��ġ����.
	Ÿ������.
	Virtual �� ���� �ൿ�Ѵ�.
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

	// ActionMsg �� ó���� �Լ�
	bool ActionMsgProc();

	//Sprite �������� ����� ���� �ù߳��?
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
	bool _bIsMyPlayer;	// TRUE �� �� ����, FALSE �� �ٸ� ����.
	int _iHP;
	//
	DWORD _dwActionNow;
	DWORD _dwActionOld;
	//
	//
	bool _bDirection;	// LEFT == 0 ; RIGHT == 1;
};