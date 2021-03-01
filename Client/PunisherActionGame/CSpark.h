#include "stdafx.h"

#ifndef _SPARK_H_
#define _SPARK_H_
#include "CMap.h"
extern CMap g_Map;
class CSpark : public CBaseObject
{
public:
	CSpark(int x, int y, bool b3Attack = false) : CBaseObject(0x9283,enEFFECT,x,y)
	{
		_iStartIdx = 63;
		_iCurrentIdx = 62;
		_iEndIdx = 66;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_EFFECT;
	}
	//virtual ~CSpark();
	
	bool Action()
	{
		if (_iCurrentIdx == _iEndIdx)
			return false;
		return true;
	}
	bool Draw()
	{
		//SINGTONE 객체 셋팅;
		CScreenDIB *pSC = CScreenDIB::GetInstance();
		CSpriteDIB *pSP = CSpriteDIB::GetInstance();

		//Screen 에 그리기 위해 버퍼를 생성;
		BYTE* bypBuffer = pSC->GetDIBBuffer();
		int iDestHeight = pSC->GetHeigt();
		int iDestPitch = pSC->GetPitch();
		int iDestWidth = pSC->GetWidth();

		int Left, Top;
		g_Map.GetViewPoint(&Top, &Left);
		if (Left < 0)
			Left = 0;
		else if (Left >= (6400 - 640))
			Left = 6400 - 640;

		if (Top < 0)
			Top = 0;
		else if (Top >= (6400 - 512))
			Top = 6500 - 512;

		
		if (_iCurrentIdx >= 63)
		{
			pSP->DrawSprite(_iCurrentIdx, _iX - Left, _iY - 60 - Top, bypBuffer, iDestWidth, iDestHeight, iDestPitch);
		}
		//EFFECT 출력
		NextSprite();
		return true;
	}
};

#endif