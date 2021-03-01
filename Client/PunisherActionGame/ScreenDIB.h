#ifndef _SCREEN_DIB_H_
#define _SCREEN_DIB_H_
#include <Windows.h>
/*
	게임 화면 해상도를 정한다
	BITMAPINFO 또는 BITMAPINFOHEADER 구조체 변수를 확보한다
	BITMAPINFOHEADER 의 정보를 모두 셋팅 한다.
	게임 해상도와 컬러 비트를 계산하여 이미지 메모리를 할당 받는다. => 스크린 버퍼
	스크린 버퍼에 그림 데이터를 넣는다
	DIB->DC 함수를 사용해 우리가 만든 버퍼 DIB를 화면 DC로 출력한다.
*/

/*************************************************
	SceenDIB.h
**************************************************/

/*************************************************
	DIB를 사용한 GDI용 스크린 버퍼
	ㅇㄴ도우의 HDC 에 DIB를 사용하여 그림을 그린다.
**************************************************/
void Update(void);

class CScreenDIB
{
public:
	static CScreenDIB* GetInstance()
	{
		static CScreenDIB screen(640, 480, 32);
		return &screen;
	}
private:
	CScreenDIB(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDIB();

protected:
	void CreateDIBBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDIBBuffer(void);

public:
	void DrawBuffer(HWND hWnd, int iX = 0, int iY = 0);

	BYTE* GetDIBBuffer();
	int GetWidth();
	int GetHeigt();
	int GetPitch();

protected:
	BITMAPINFO	m_stDibInfo;
	BYTE		*m_bypBuffer;
	int			m_iWidth;
	int			m_iHeight;
	int			m_iPitch;
	int			m_iColorBit;
	int			m_iBufferSize;
};






#endif