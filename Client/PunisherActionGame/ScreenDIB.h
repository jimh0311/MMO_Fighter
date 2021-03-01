#ifndef _SCREEN_DIB_H_
#define _SCREEN_DIB_H_
#include <Windows.h>
/*
	���� ȭ�� �ػ󵵸� ���Ѵ�
	BITMAPINFO �Ǵ� BITMAPINFOHEADER ����ü ������ Ȯ���Ѵ�
	BITMAPINFOHEADER �� ������ ��� ���� �Ѵ�.
	���� �ػ󵵿� �÷� ��Ʈ�� ����Ͽ� �̹��� �޸𸮸� �Ҵ� �޴´�. => ��ũ�� ����
	��ũ�� ���ۿ� �׸� �����͸� �ִ´�
	DIB->DC �Լ��� ����� �츮�� ���� ���� DIB�� ȭ�� DC�� ����Ѵ�.
*/

/*************************************************
	SceenDIB.h
**************************************************/

/*************************************************
	DIB�� ����� GDI�� ��ũ�� ����
	���������� HDC �� DIB�� ����Ͽ� �׸��� �׸���.
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