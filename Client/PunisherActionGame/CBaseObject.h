#pragma once
#include "stdafx.h"
class CBaseObject
{
public:
	CBaseObject(int id, int type, int x, int y);
	//
	
	// ������ ���� ACTION(), DRAW() �Լ�;
	virtual bool Action() = 0;
	virtual bool Draw() = 0;

	// �޼��� ������ ���� �Լ�
	bool SendActionMsg(DWORD msg);
	void NextSprite();

	// Get �Լ���
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
	
	//	���, ���Ǽ��� ���� ������Ƽ��� ���´�.
protected:
	//	�޼��� ����
	DWORD	_dwActionMsg;

	//------------------------------------------------------
	//	������ 17.01.26
	//
	//	Sprite�� �ǽð� �ݿ��ȵǴ� �� �־ �߰���
	//	��������
	//------------------------------------------------------
	BYTE	_byDir;
	//	��ġ����
	int _iX;
	int _iY;

	//	Ÿ������
	int _ObjectType;
	int _ObjectId;

	//	��������Ʈ�� �׸��� ���� ����
	int _iStartIdx;
	int _iCurrentIdx;
	int _iEndIdx;

	int _iDelayCount;
	int _iFrameDelay;
	
	/*
	//	������ �߰��� ������ �ʱ� ����
	�̵��� false , �޼��� ���� �� ����.
	������ true , �޼��� ���� �� ����.
	*/
	bool _bAttack;
	
};