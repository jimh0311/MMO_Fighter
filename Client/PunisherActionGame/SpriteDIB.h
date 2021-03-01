/***************************************************
BMP 파일을 읽어서 스프라이트 관리 클래스
***************************************************/
#include <Windows.h>
#ifndef _SPRITE_DIB_H_
#define _SPRITE_DIB_H_

/**********************************************
DIB 스프라이트 구조체

스프라이트 이미지와 사이즈 정보를 가짐
**********************************************/

struct stArrBitMap
{
	BYTE *_byBuffer;
	BITMAPINFOHEADER _InfoHeader;

	int _iHeight;
	int _iWidth;
	int _iPitch;
	int _iBitCount;

	int _iCenterPointX;
	int _iCenterPointY;
};


class CSpriteDIB
{
	
private:
	/**********************************************
		생성자 int 스프라이트 갯수, 컬러키 ( 투명키 ) 입력
	**********************************************/
	CSpriteDIB(int NumOfSprite, DWORD dwColorKey);
	virtual ~CSpriteDIB();
public:
	static CSpriteDIB* GetInstance()
	{
		static CSpriteDIB sprite(70, 0x00ffffff);
		return &sprite;
	}
	/**********************************************
		BMP 파일을 읽어서 하나의 프레임으로 저장
	**********************************************/
	BOOL LoadDIBSprite(int iSpriteIndex, WCHAR *szFileName, int iCenterPointX, int iCenterPointY);
	/**********************************************
		해당 스프라이트 삭제
	**********************************************/
	void ReleaseSprite(int iSpriteIndex);
	/**********************************************
		특정 메모리 위치에 스프라이트를 출력한다. ( 칼라키, 클리핑 처리)
	**********************************************/
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	void DrawSprite_Shadow(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	void DrawSprite_MyChar(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	/**********************************************
		특정 메모리 위치에 이미지를 출력한다 ( 클리핑 처리 )
	**********************************************/
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
protected:
	/**********************************************
		Sprite배열 정보
		배열형태로 받는다.
	**********************************************/
	int m_iMaxSprite;
	stArrBitMap *m_pSprite;
	/**********************************************
		투명색 처리
	**********************************************/
	DWORD m_dwColorKey;

};





#endif
