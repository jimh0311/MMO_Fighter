#include "stdafx.h"
/********************************************************

********************************************************/
CScreenDIB::CScreenDIB(int iWidth, int iHeight, int iColorBit)
{
	// iPitch 계산
	int temp;
	temp = 4 - (((iWidth * iColorBit / 8)) % 4);
	if (temp == 4)
	{
		m_iPitch = (iWidth * (iColorBit / 8));
	}
	else
	{
		m_iPitch = (iWidth * (iColorBit / 8)) + temp;
	}

	// 이미지 정보들 저장
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iColorBit = iColorBit;
	
	// DIBINFOHEADER 셋팅
	m_stDibInfo.bmiHeader.biSize = sizeof(m_stDibInfo);	// INFOHEADER의 사이즈
	m_stDibInfo.bmiHeader.biWidth = iWidth;
	m_stDibInfo.bmiHeader.biHeight = -iHeight;
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biBitCount = iColorBit;
	m_stDibInfo.bmiHeader.biCompression = 0;
	m_stDibInfo.bmiHeader.biSizeImage = 0;
	m_stDibInfo.bmiHeader.biXPelsPerMeter = 0;
	m_stDibInfo.bmiHeader.biYPelsPerMeter = 0;
	m_stDibInfo.bmiHeader.biClrUsed = 0;
	m_stDibInfo.bmiHeader.biClrImportant = 0;

	// 생성
	m_iBufferSize = m_iPitch * m_iHeight;
	m_bypBuffer = new BYTE[m_iBufferSize];

}
CScreenDIB::~CScreenDIB()
{
	delete[] m_bypBuffer;
}


void CScreenDIB::CreateDIBBuffer(int iWidth, int iHeight, int iColorBit)
{
	//?
}
void CScreenDIB::ReleaseDIBBuffer(void)
{
	//?
}

void CScreenDIB::DrawBuffer(HWND hWnd, int iX , int iY )
{
	//얻어온다
	HDC hdc = GetDC(hWnd);
	SetDIBitsToDevice(hdc, iX, iX, m_iWidth, m_iHeight, 0, 0, 0, m_iHeight, m_bypBuffer, &m_stDibInfo, DIB_RGB_COLORS);
	memset(m_bypBuffer, 0xff, m_iBufferSize);
	ReleaseDC(hWnd, hdc);
}



BYTE* CScreenDIB::GetDIBBuffer()
{
	return m_bypBuffer;
}
int CScreenDIB::GetWidth()
{
	return m_iWidth;
}
int CScreenDIB::GetHeigt()
{
	return m_iHeight;
}
int CScreenDIB::GetPitch()
{
	return m_iPitch;
}


