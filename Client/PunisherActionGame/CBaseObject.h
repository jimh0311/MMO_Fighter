#pragma once
#include "stdafx.h"
class CBaseObject
{
public:
	CBaseObject(int id, int type, int x, int y);
	//
	
	// 진행을 위한 ACTION(), DRAW() 함수;
	virtual bool Action() = 0;
	virtual bool Draw() = 0;

	// 메세지 전송을 위한 함수
	bool SendActionMsg(DWORD msg);
	void NextSprite();

	// Get 함수들
	int GetType()
	{
		return _ObjectType;
	}
	int GetID()
	{
		return _ObjectId;
	}
	int GetPosY()
	{
		return _iY;
	}
	void SetPosY(int y)
	{
		 _iY = y;
	}
	int GetPosX()
	{
		return _iX;
	}
	void SetPosX(int x)
	{
		 _iX = x;
	}
	
	//	상속, 편의성을 위해 프로텍티드로 묶는다.
protected:
	//	메세지 정보
	DWORD	_dwActionMsg;

	//------------------------------------------------------
	//	지민혁 17.01.26
	//
	//	Sprite가 실시간 반영안되는 게 있어서 추가함
	//	방향정보
	//------------------------------------------------------
	BYTE	_byDir;
	//	위치정보
	int _iX;
	int _iY;

	//	타입정보
	int _ObjectType;
	int _ObjectId;

	//	스프라이트를 그리기 위한 변수
	int _iStartIdx;
	int _iCurrentIdx;
	int _iEndIdx;

	int _iDelayCount;
	int _iFrameDelay;
	
	/*
	//	공격이 중간에 끊기지 않기 위해
	이동중 false , 메세지 받을 수 있음.
	공격중 true , 메세지 받을 수 없음.
	*/
	bool _bAttack;
	
};