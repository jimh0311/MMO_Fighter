#include "stdafx.h"


#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470


/**********************************************
	생성자 int 스프라이트 갯수, 컬러키 ( 투명키 ) 입력
**********************************************/
CSpriteDIB::CSpriteDIB(int NumOfSprite, DWORD dwColorKey)
{
	m_iMaxSprite = NumOfSprite;
	m_pSprite = new stArrBitMap[m_iMaxSprite];
	m_dwColorKey = dwColorKey;
}
CSpriteDIB::~CSpriteDIB()
{
	for (int i = 0; i < m_iMaxSprite; i++)
	{
		ReleaseSprite(i);
	}
	delete[] m_pSprite;
}

/**********************************************
	BMP 파일을 읽어서 하나의 프레임으로 저장
**********************************************/
BOOL CSpriteDIB::LoadDIBSprite(int iSpriteIndex, WCHAR *szFileName, int iCenterPointX, int iCenterPointY)
{

	//	인덱스 예외처리

	if (iSpriteIndex < 0 || iSpriteIndex >= m_iMaxSprite)
	{
		return FALSE;
	}

	//	파일 열기

	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//	파일 예외처리

	if (hFile == NULL)
	{
		return FALSE;
	}

	int iPos = iSpriteIndex;
	
	m_pSprite[iPos]._iCenterPointX = iCenterPointX;
	m_pSprite[iPos]._iCenterPointY = iCenterPointY;

	//	파일 생성 및 처리;
	
	//	파일헤더, 인포헤더 떼어내기
	DWORD	ttttemp;
	BITMAPFILEHEADER stBitFileHeader;
	if (ReadFile(hFile, &stBitFileHeader, sizeof(BITMAPFILEHEADER), &ttttemp, 0) == TRUE)
	{
		if (ReadFile(hFile, &(m_pSprite[iPos]._InfoHeader), sizeof(BITMAPINFOHEADER), &ttttemp, 0) == TRUE)
		{
		
			// 구조체 내용 셋업

			m_pSprite[iPos]._iWidth = m_pSprite[iPos]._InfoHeader.biWidth;
			m_pSprite[iPos]._iHeight = m_pSprite[iPos]._InfoHeader.biHeight;
			m_pSprite[iPos]._iBitCount = m_pSprite[iPos]._InfoHeader.biBitCount;
			m_pSprite[iPos]._iPitch = 4 - ((m_pSprite[iPos]._iWidth * (m_pSprite[iPos]._iBitCount / 8)) % 4);
			if (m_pSprite[iPos]._iPitch != 4)
			{
				m_pSprite[iPos]._iPitch = m_pSprite[iPos]._iPitch + (m_pSprite[iPos]._iWidth * (m_pSprite[iPos]._iBitCount / 8));
			}
			else
			{
				m_pSprite[iPos]._iPitch = (m_pSprite[iPos]._iWidth * (m_pSprite[iPos]._iBitCount / 8));
			}
			
			// 뒤집기 용 임시 버퍼 

			BYTE *temp = new BYTE[m_pSprite[iPos]._iPitch * m_pSprite[iPos]._iHeight];
			m_pSprite[iPos]._byBuffer = new BYTE[m_pSprite[iPos]._iPitch * m_pSprite[iPos]._iHeight];
	
			ReadFile(hFile, temp, m_pSprite[iPos]._iPitch * m_pSprite[iPos]._iHeight, &ttttemp, NULL);
			

			// 복사를 위한 지역 변수들
			int t_iPitch, t_iHeight;
			t_iPitch = m_pSprite[iPos]._iPitch;
			t_iHeight = m_pSprite[iPos]._iHeight;

			BYTE *dest = m_pSprite[iPos]._byBuffer;
			BYTE *src = temp;
			src += (t_iHeight-1) * t_iPitch;
			
			// 뒤집기

			for (int i = 0; i < t_iHeight; i++)
			{
				memcpy_s(dest, t_iPitch, src, t_iPitch);
				dest += t_iPitch;
				src -= t_iPitch;
			}
			
			//	임시객체 삭제

			delete[] temp;
			
			CloseHandle(hFile);
	
		}
	}
	return TRUE;
}
/**********************************************
	해당 스프라이트 삭제
**********************************************/
void CSpriteDIB::ReleaseSprite(int iSpriteIndex)
{
	if (iSpriteIndex >= 0 || iSpriteIndex < m_iMaxSprite)
	{
		if (m_pSprite[iSpriteIndex]._byBuffer != nullptr)
		{
			delete[] m_pSprite[iSpriteIndex]._byBuffer;
		}
		memset(&m_pSprite[iSpriteIndex], 0, sizeof(stArrBitMap));
	}
}
/**********************************************
	특정 메모리 위치에 스프라이트를 출력한다. ( 칼라키, 클리핑 처리)
**********************************************/
void CSpriteDIB::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	/**********************************************
		인덱스 예외처리
	**********************************************/
	if (iSpriteIndex < 0 || iSpriteIndex >= m_iMaxSprite)
	{
		return;
	}
	else
	{
		/**********************************************
			지역변수로 필요정보 셋팅
		**********************************************/
		BYTE* src = m_pSprite[iSpriteIndex]._byBuffer;
		BYTE* dest = bypDest;
		
		int srcWidth = m_pSprite[iSpriteIndex]._iWidth * iDrawLen /100;
		int srcHeight = m_pSprite[iSpriteIndex]._iHeight;
		int srcPitch = m_pSprite[iSpriteIndex]._iPitch;

		int srcCenterX = m_pSprite[iSpriteIndex]._iCenterPointX;
		int srcCenterY = m_pSprite[iSpriteIndex]._iCenterPointY;
		/**********************************************
			출력 중점으로 좌표 계산
		**********************************************/
		int FixedDrawX = iDrawX - srcCenterX;
		int FixedDrawY = iDrawY - srcCenterY;
		/**********************************************
			상하좌우 클리핑 처리
		**********************************************/
		
		int iCountX = 0;
		int iCountY = 0;

		if (FixedDrawX < 0)
		{
			iCountX += (-FixedDrawX);
			FixedDrawX = 0;
		}
		if (FixedDrawX + srcWidth > 640)
		{
			srcWidth -= FixedDrawX + srcWidth - 640;
		}
		if (FixedDrawY < 0)
		{
			iCountY += (-FixedDrawY);
			FixedDrawY = 0;
		}
		if (FixedDrawY + srcHeight > 480)
		{
			srcHeight -= FixedDrawY + srcHeight - 480;
		}

		/**********************************************
			포인터 이동
		**********************************************/

		dest += (FixedDrawX * 4) + (FixedDrawY * iDestPitch);
		src += (iCountX * 4) + (iCountY * srcPitch);

		/**********************************************
			전체 돌면서 픽셀단위로 찍기
		**********************************************/

		DWORD *pdwSrc, *pdwDest;

		for (int iPosY = iCountY; iPosY < srcHeight; iPosY++)
		{
			pdwSrc = (DWORD*)src;
			pdwDest = (DWORD*)dest;
			for (int iPosX = iCountX; iPosX < srcWidth; iPosX++)
			{
				if ( ((*pdwSrc)&0x00ffffff) != m_dwColorKey)
				{
					*pdwDest = *pdwSrc;
				}
				pdwDest++;
				pdwSrc++;
			}
			src += srcPitch;
			dest += iDestPitch;
		}




		/**********************************************
			전체 돌면서 픽셀단위로 찍기
		**********************************************/
		
		 /*
		for (  int iCountY = 0  ; iCountY < srcHeight; iCountY++)
		{
			for ( int iCountX = 0 ; iCountX < srcPitch; iCountX++)
			{
				if (memcmp(&(src[iCountX + (iCountY * srcPitch)]), &m_dwColorKey, 3) == 0)
				{
					iCountX += 3;
				}
				else
				{
					if ((FixedDrawY+iCountY)*iDestPitch < 470* iDestPitch && (FixedDrawY + iCountY)*iDestPitch > 10 * iDestPitch && (iCountX + (FixedDrawX * 4) > 10*4) && (iCountX + (FixedDrawX * 4) < 630 * 4))
					{
						dest[iCountX + (FixedDrawX * 4) + (iCountY * iDestPitch)] = src[iCountX + (iCountY * srcPitch)];
					}
				}
			}
		}
		 */	
	}
}

/**********************************************
	특정 메모리 위치에 이미지를 출력한다 ( 클리핑 처리 )
**********************************************/

void CSpriteDIB::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	/**********************************************
	인덱스 예외처리
	**********************************************/
	if (iSpriteIndex < 0 || iSpriteIndex >= m_iMaxSprite)
	{
		return;
	}
	else
	{
		/**********************************************
		지역변수로 필요정보 셋팅
		**********************************************/
		BYTE* src = m_pSprite[iSpriteIndex]._byBuffer;
		BYTE* dest = bypDest;

		int srcWidth = m_pSprite[iSpriteIndex]._iWidth;
		int srcHeight = m_pSprite[iSpriteIndex]._iHeight;
		int srcPitch = m_pSprite[iSpriteIndex]._iPitch;

		int srcCenterX = m_pSprite[iSpriteIndex]._iCenterPointX;
		int srcCenterY = m_pSprite[iSpriteIndex]._iCenterPointY;
		/**********************************************
		출력 중점으로 좌표 계산
		**********************************************/
		int FixedDrawX = iDrawX - srcCenterX;
		int FixedDrawY = iDrawY - srcCenterY;
		/**********************************************
		상하좌우 클리핑 처리
		**********************************************/

		int iCountX = 0;
		int iCountY = 0;

		if (FixedDrawX < 0)
		{
			iCountX += (-FixedDrawX);
			FixedDrawX = 0;
		}
		if (FixedDrawX + srcWidth > 640)
		{
			srcWidth -= FixedDrawX + srcWidth - 640;
		}
		if (FixedDrawY < 0)
		{
			iCountY += (-FixedDrawY);
			FixedDrawY = 0;
		}
		if (FixedDrawY + srcHeight > 480)
		{
			srcHeight -= FixedDrawY + srcHeight - 480;
		}

		/**********************************************
		포인터 이동
		**********************************************/

		dest += (FixedDrawX * 4) + (FixedDrawY * iDestPitch);
		src += (iCountX * 4) + (iCountY * srcPitch);

		/**********************************************
		전체 돌면서 픽셀단위로 찍기
		**********************************************/

	//	DWORD *pdwSrc, *pdwDest;

		for (int iPosY = iCountY; iPosY < srcHeight; iPosY++)
		{
			memcpy_s(dest, srcWidth * 4, src, srcWidth * 4);
			dest += iDestPitch;
			src += srcPitch;
		}
	}
}

/**********************************************
특정 메모리 위치에 그림자( 반 투명 ) 스프라이트를 출력한다. ( 칼라키, 클리핑 처리)
**********************************************/
void CSpriteDIB::DrawSprite_Shadow(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	/**********************************************
	인덱스 예외처리
	**********************************************/
	if (iSpriteIndex < 0 || iSpriteIndex >= m_iMaxSprite)
	{
		return;
	}
	else
	{
		/**********************************************
		지역변수로 필요정보 셋팅
		**********************************************/
		BYTE* src = m_pSprite[iSpriteIndex]._byBuffer;
		BYTE* dest = bypDest;

		int srcWidth = m_pSprite[iSpriteIndex]._iWidth * iDrawLen / 100;
		int srcHeight = m_pSprite[iSpriteIndex]._iHeight;
		int srcPitch = m_pSprite[iSpriteIndex]._iPitch;

		int srcCenterX = m_pSprite[iSpriteIndex]._iCenterPointX;
		int srcCenterY = m_pSprite[iSpriteIndex]._iCenterPointY;
		/**********************************************
		출력 중점으로 좌표 계산
		**********************************************/
		int FixedDrawX = iDrawX - srcCenterX;
		int FixedDrawY = iDrawY - srcCenterY;
		/**********************************************
		상하좌우 클리핑 처리
		**********************************************/

		int iCountX = 0;
		int iCountY = 0;

		if (FixedDrawX < 0)
		{
			iCountX += (-FixedDrawX);
			FixedDrawX = 0;
		}
		if (FixedDrawX + srcWidth > 640)
		{
			srcWidth -= FixedDrawX + srcWidth - 640;
		}
		if (FixedDrawY < 0)
		{
			iCountY += (-FixedDrawY);
			FixedDrawY = 0;
		}
		if (FixedDrawY + srcHeight > 480)
		{
			srcHeight -= FixedDrawY + srcHeight - 480;
		}

		/**********************************************
		포인터 이동
		**********************************************/

		dest += (FixedDrawX * 4) + (FixedDrawY * iDestPitch);
		src += (iCountX * 4) + (iCountY * srcPitch);

		/**********************************************
		전체 돌면서 픽셀단위로 찍기
		**********************************************/

		DWORD *pdwSrc, *pdwDest;

		for (int iPosY = iCountY; iPosY < srcHeight; iPosY++)
		{
			pdwSrc = (DWORD*)src;
			pdwDest = (DWORD*)dest;
			for (int iPosX = iCountX; iPosX < srcWidth; iPosX++)
			{
				if (((*pdwSrc) & 0x00ffffff) != m_dwColorKey)
				{
					BYTE *bpDest;
					BYTE *bpSrc;
					BYTE *bpResult;
					DWORD dwResult = 0;
					bpSrc = (BYTE*)pdwSrc;
					bpDest = (BYTE*)pdwDest;
					bpResult = (BYTE*)&dwResult;
					//B G R A
					for (int i = 0; i < 3; i++)
					{
						bpResult[i] = (bpSrc[i] / 2 + bpDest[i] / 2);
					}
					*pdwDest = dwResult;
					//*pdwDest = *pdwSrc;
				}
				pdwDest++;
				pdwSrc++;
			}
			src += srcPitch;
			dest += iDestPitch;
		}
	}
}

/**********************************************
특정 메모리 위치에 내 캐릭터...빨간색 강조 스프라이트를 출력한다. ( 칼라키, 클리핑 처리)
**********************************************/
void CSpriteDIB::DrawSprite_MyChar(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	/**********************************************
	인덱스 예외처리
	**********************************************/
	if (iSpriteIndex < 0 || iSpriteIndex >= m_iMaxSprite)
	{
		return;
	}
	else
	{
		/**********************************************
		지역변수로 필요정보 셋팅
		**********************************************/
		BYTE* src = m_pSprite[iSpriteIndex]._byBuffer;
		BYTE* dest = bypDest;

		int srcWidth = m_pSprite[iSpriteIndex]._iWidth * iDrawLen / 100;
		int srcHeight = m_pSprite[iSpriteIndex]._iHeight;
		int srcPitch = m_pSprite[iSpriteIndex]._iPitch;

		int srcCenterX = m_pSprite[iSpriteIndex]._iCenterPointX;
		int srcCenterY = m_pSprite[iSpriteIndex]._iCenterPointY;
		/**********************************************
		출력 중점으로 좌표 계산
		**********************************************/
		int FixedDrawX = iDrawX - srcCenterX;
		int FixedDrawY = iDrawY - srcCenterY;
		/**********************************************
		상하좌우 클리핑 처리
		**********************************************/

		int iCountX = 0;
		int iCountY = 0;

		if (FixedDrawX < 0)
		{
			iCountX += (-FixedDrawX);
			FixedDrawX = 0;
		}
		if (FixedDrawX + srcWidth > 640)
		{
			srcWidth -= FixedDrawX + srcWidth - 640;
		}
		if (FixedDrawY < 0)
		{
			iCountY += (-FixedDrawY);
			FixedDrawY = 0;
		}
		if (FixedDrawY + srcHeight > 480)
		{
			srcHeight -= FixedDrawY + srcHeight - 480;
		}

		/**********************************************
		포인터 이동
		**********************************************/

		dest += (FixedDrawX * 4) + (FixedDrawY * iDestPitch);
		src += (iCountX * 4) + (iCountY * srcPitch);

		/**********************************************
		전체 돌면서 픽셀단위로 찍기
		**********************************************/

		DWORD *pdwSrc, *pdwDest;

		for (int iPosY = iCountY; iPosY < srcHeight; iPosY++)
		{
			pdwSrc = (DWORD*)src;
			pdwDest = (DWORD*)dest;
			for (int iPosX = iCountX; iPosX < srcWidth; iPosX++)
			{
				if (((*pdwSrc) & 0x00ffffff) != m_dwColorKey)
				{
					//BGRA 순으로 들어가있고
					//B,G 의 값을 떨궈서 R 를 붉게 만든다.
					DWORD dwResult = 0;
					dwResult = *pdwSrc;
					BYTE* pdwResult = (BYTE*)&dwResult;
					pdwResult[0] = pdwResult[0]/2;
					pdwResult[1] = pdwResult[1]/2;
					//Fix된 RGB 값 = dwResult 를 입력한다.
					*pdwDest = dwResult;
				}
				pdwDest++;
				pdwSrc++;
			}
			src += srcPitch;
			dest += iDestPitch;
		}
	}
}