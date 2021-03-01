#include "stdafx.h"
#include "MakePacket.h"
//
//	���̽� ������Ʈ�� ����;
//
CBaseObject::CBaseObject(int id, int type, int x, int y)
{
	_ObjectId = id;
	_ObjectType = type;
	_iX = x;
	_iY = y;
	_dwActionMsg = dfACTION_STAND;
	_bAttack = false;
	//------------------------------------------------------
	//	������ 17.01.26
	//
	//	Sprite�� �ǽð� �ݿ��ȵǴ� �� �־ �߰���
	//------------------------------------------------------
	_byDir = dfACTION_MOVE_LL;
}

//
//	CBaseObject�� �����͸� ���� ����Ʈ�� ������ ���̱� ������ �����ϾߎO;
//	�޼��� ������ �޴´�.
//
bool CBaseObject::SendActionMsg(DWORD msg)
{
	//	����ó��.
	if (msg < dfACTION_MOVE_LL || msg > dfACTION_STAND)
		return false;

	_dwActionMsg = msg;
	return true;
}
//
//	���� ��������Ʈ�� ����ؼ� �������ش�
//
//
void CBaseObject::NextSprite()
{
	if (_iDelayCount < _iFrameDelay)
	{
		_iDelayCount++;
	}
	else if (_iDelayCount >= _iFrameDelay)
	{
		_iDelayCount = 0;

		_iCurrentIdx++;
		if (_iCurrentIdx > _iEndIdx)
		{
			//Attack ���� ����;
			
			_iCurrentIdx = _iStartIdx;
			if (_bAttack == true)
			{
				_bAttack = false;
				_dwActionMsg = dfACTION_STAND;
				//MakePacket(dfACTION_STAND);
			}
		}
	}

}


