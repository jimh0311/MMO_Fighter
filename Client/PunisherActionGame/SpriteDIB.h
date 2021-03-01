/***************************************************
BMP ������ �о ��������Ʈ ���� Ŭ����
***************************************************/
#include <Windows.h>
#ifndef _SPRITE_DIB_H_
#define _SPRITE_DIB_H_

/**********************************************
DIB ��������Ʈ ����ü

��������Ʈ �̹����� ������ ������ ����
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
		������ int ��������Ʈ ����, �÷�Ű ( ����Ű ) �Է�
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
		BMP ������ �о �ϳ��� ���������� ����
	**********************************************/
	BOOL LoadDIBSprite(int iSpriteIndex, WCHAR *szFileName, int iCenterPointX, int iCenterPointY);
	/**********************************************
		�ش� ��������Ʈ ����
	**********************************************/
	void ReleaseSprite(int iSpriteIndex);
	/**********************************************
		Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. ( Į��Ű, Ŭ���� ó��)
	**********************************************/
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	void DrawSprite_Shadow(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	void DrawSprite_MyChar(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
	/**********************************************
		Ư�� �޸� ��ġ�� �̹����� ����Ѵ� ( Ŭ���� ó�� )
	**********************************************/
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
		int iDestPitch, int iDrawLen = 100);
protected:
	/**********************************************
		Sprite�迭 ����
		�迭���·� �޴´�.
	**********************************************/
	int m_iMaxSprite;
	stArrBitMap *m_pSprite;
	/**********************************************
		����� ó��
	**********************************************/
	DWORD m_dwColorKey;

};





#endif
