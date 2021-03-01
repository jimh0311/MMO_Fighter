#include "stdafx.h"
#include "MakePacket.h"
//
//	베이스 오브젝트의 생성;
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
	//	지민혁 17.01.26
	//
	//	Sprite가 실시간 반영안되는 게 있어서 추가함
	//------------------------------------------------------
	_byDir = dfACTION_MOVE_LL;
}

//
//	CBaseObject의 포인터를 가진 리스트로 움직일 것이기 때문에 하하하야랳;
//	메세지 정보를 받는다.
//
bool CBaseObject::SendActionMsg(DWORD msg)
{
	//	예외처리.
	if (msg < dfACTION_MOVE_LL || msg > dfACTION_STAND)
		return false;

	_dwActionMsg = msg;
	return true;
}
//
//	현재 스프라이트를 계속해서 움직여준다
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
			//Attack 고정 해제;
			
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


