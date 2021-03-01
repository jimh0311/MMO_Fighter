#pragma once
class CMap
{
public:
	CMap(int, int, int);
	~CMap();

	void SetViewPoint(int in_Top, int in_Left);
	void GetViewPoint(int *Out_Top, int *Out_Left);
protected:
	int _PixelTop;
	int _PixelLeft;

	int _TileSize;
	int _MaxWidth;
	int _MaxHeight;

	char **_arrMap;
};