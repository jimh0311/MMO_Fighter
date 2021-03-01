#include "stdafx.h"
#include <timeapi.h>
CFrameControl::CFrameControl(int needFrame) :_iDelayFrame(needFrame)
{
	_iBeginTime = 0;
	_iPrevEndTime = 0;
	_iTempTime = 0;
	_iExtraTime = 0;
	_iCheck = 2222;
	timeBeginPeriod(1);
}
CFrameControl::~CFrameControl()
{
	timeEndPeriod(1);
}
bool CFrameControl::FrameCheck()
{
	//iTempTime = timeGetTime();
	//_ = (_iTempTime - _iBeginTime) - _iDelayFrame;

	if (_iCheck == 2222)
	{
		_iCheck = timeGetTime();
	}
	//프레임 시간
	_iTempTime = timeGetTime();
	_iPerFrameTime = _iTempTime - _iCheck - _iDelayFrame;
	
		if ( _iPerFrameTime <= 0 )
		{
			Sleep(-_iPerFrameTime);
			_iCheck = _iTempTime - _iPerFrameTime;
			return true;
		}
		else if (_iPerFrameTime > 0)
		{
			_iCheck = _iTempTime - (_iPerFrameTime);
			return false;
		}
	
	
	return true;
}
