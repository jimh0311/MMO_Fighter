#include "stdafx.h"
#include "CMap.h"


//------------------------------------------------------------------
//  17.01.16
//
//	생성자
//------------------------------------------------------------------

CMap::CMap(int in_Size, int in_Width, int in_Height)
{
	//
	//시바알~_~
	_TileSize = in_Size;
	_MaxWidth = in_Width;
	_MaxHeight = in_Height;

	_PixelTop = 0;
	_PixelLeft = 0;
	//
	//2차원 배열 동적할당...
	_arrMap = new char*[_MaxHeight];
	for (int i = 0; i < _MaxHeight; i++)
	{
		_arrMap[i] = new char[_MaxWidth];
		memset(_arrMap[i], 0, sizeof(char)*_MaxWidth);
	}
}

//------------------------------------------------------------------
//  17.01.16
//
//	파괴자
//------------------------------------------------------------------

CMap::~CMap()
{
	for (int i = 0; i < _MaxHeight; i++)
	{
		delete[] _arrMap[i];
	}

}


//------------------------------------------------------------------
//  17.01.16
//
//	ViewPoint 설정...
//------------------------------------------------------------------
void CMap::SetViewPoint(int in_Top, int in_Left)
{
	_PixelTop = in_Top - 256;
	_PixelLeft = in_Left - 320;
}




//------------------------------------------------------------------
//  17.01.16
//
//	ViewPoint 얻어오기
//------------------------------------------------------------------
void CMap::GetViewPoint(int *Out_Top, int *Out_Left)
{
	*Out_Top = _PixelTop;
	*Out_Left = _PixelLeft;

	return;

	//g[i]
}