//
//	����:	�ʸ� ���̰� ��ü�� ������� ����;�
//
//

#include "stdafx.h"
#include "MakePacket.h"
#include "CMap.h"
#define MAX_LEFT 15
#define MAX_RIGHT 6385
#define MAX_TOP 15
#define MAX_BOTTOM 6385

extern CMap g_Map;
//
//	MAIN PROC()
//
BOOL	g_KeyProc = true;
bool CPlayerObject::Action()
{
	NextSprite();
	/*
		���� WinMain���� KeyProc()�� ���� Ű���带 �Է¹ް�
		_dwActionMsg �� ���� ���ƿ� Msg�� ����Ǿ� �ִ� ��Ȳ.
	*/

	// ���� �׼��� �����ϰ� ���� �׼����� ����
	if (_bIsMyPlayer == true)
	{
		if (_bAttack == false)
		{
			g_KeyProc = true;
			_dwActionOld = _dwActionNow;
			_dwActionNow = _dwActionMsg;	//	Msg�� �θ𿡼� ���ƿ��� ��;
		}
	}
	else
	{
		_dwActionOld = _dwActionNow;
		_dwActionNow = _dwActionMsg;	//	Msg�� �θ𿡼� ���ƿ��� ��;
	}
	
	ActionMsgProc();				//	���� �׼��� ó���Ѵ�.

	if (_bIsMyPlayer)
		g_Map.SetViewPoint(_iY, _iX);

	//------------------------------------------------------
	//	������ 17.01.26
	//
	//	�����׽�Ʈ���� ü�� 0�γ�� ������� �� ������ ���ؼ�
	//------------------------------------------------------
	if (_iHP <= 0)
		return true;

	return true;
}
bool CPlayerObject::Draw()
{
	//SINGTONE ��ü ����;
	CScreenDIB *pSC = CScreenDIB::GetInstance();
	CSpriteDIB *pSP = CSpriteDIB::GetInstance();

	//Screen �� �׸��� ���� ���۸� ����;
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
	//�׸���.
	pSP->DrawSprite_Shadow(68, _iX - Left, _iY - Top, bypBuffer, iDestWidth, iDestHeight, iDestPitch);
	if (_bIsMyPlayer)
	{
		pSP->DrawSprite_MyChar(_iCurrentIdx, _iX - Left, _iY - Top, bypBuffer, iDestWidth, iDestHeight, iDestPitch);
	}
	else
	{
		pSP->DrawSprite(_iCurrentIdx, _iX - Left, _iY - Top, bypBuffer, iDestWidth, iDestHeight, iDestPitch);
	}
	pSP->DrawSprite(67, _iX - Left - 33, _iY - Top + 9, bypBuffer, iDestWidth, iDestHeight, iDestPitch, _iHP);
	//��������Ʈ�� �̵���Ų��.


	return true;
}





//
//	_dwActionNow�� �м��� ĳ���͸� �����δ�.
//
bool CPlayerObject::ActionMsgProc()
{
	//�������̸� �̵��� �Ұ���;
	if (_bIsMyPlayer == true)
	{
		if (_bAttack)
			return true;
	}
	
	bool isChange;
	if (_dwActionNow == _dwActionOld)
	{
		isChange = false;
	}
	else
	{
		isChange = true;
		//////////////////////////////////////////////////////////
		//	MAKE PACKET �� �����ҵ�...
		//////////////////////////////////////////////////////////
		if(_bIsMyPlayer==TRUE)
			MakePacket(_dwActionNow);
	}

	// �޼����� ������ �ù߾�?
	switch (_dwActionNow)
	{
		case dfACTION_STAND:
		{
			if (isChange)
				SetActionStand();
			break;
		}
		case dfACTION_MOVE_LL:
		{
			// �¿� ������ȯ
			_bDirection = LEFT;
			// ��ǥ�� ����
			if (_iX > MAX_LEFT)
			{
				_iX -= 3;
			}
			// ���� �ൿ�� ���� �ൿ�� �ٲ�� ������ �Ǹ� Sprite�� �ٲ۴�.
			if (isChange)
			{
				switch (_dwActionOld)
				{
				case dfACTION_MOVE_LD:
				case dfACTION_MOVE_LU:
			//	case dfACTION_MOVE_UU:
			//	case dfACTION_MOVE_DD:
					break;
				default:
					SetActionMove();
				}
			}
				
			break;
		}
		case dfACTION_MOVE_RR:
		{
			_bDirection = RIGHT;
			//
			if (_iX <= MAX_RIGHT)
			{
				_iX += 3;
			}

			if (isChange)
			{
				switch (_dwActionOld)
				{
				case dfACTION_MOVE_RD:
				case dfACTION_MOVE_RU:
		//		case dfACTION_MOVE_UU:
		//		case dfACTION_MOVE_DD:
					break;
				default:
					SetActionMove();
				}
			}
			break;
		}
		case dfACTION_MOVE_UU:
		{
			//��, �Ʒ��� ������ȯ �ʿ� ����
			if (_iY >= MAX_TOP) 
			{
				_iY-=2;
			}

			if (_dwActionOld==dfACTION_STAND || _dwActionOld==dfACTION_ATTACK1 ||
				_dwActionOld == dfACTION_ATTACK2 || _dwActionOld == dfACTION_ATTACK3)
				SetActionMove();
			break;
		}
		case dfACTION_MOVE_DD:
		{
			if (_iY <= MAX_BOTTOM)
			{
				_iY+=2;
			}
	
			if (_dwActionOld == dfACTION_STAND || _dwActionOld == dfACTION_ATTACK1
				|| _dwActionOld == dfACTION_ATTACK2 || _dwActionOld == dfACTION_ATTACK3)
				SetActionMove();
			break;
		}
		//�밢��
		case dfACTION_MOVE_LU:
		{
			_bDirection = LEFT;
			if (_iX >= MAX_LEFT && _iY >= MAX_TOP)
			{
				_iX-= 3;
				_iY -= 2;
			}
			switch (_dwActionOld)
			{
			case dfACTION_MOVE_LL:
			case dfACTION_MOVE_LD:
			case dfACTION_MOVE_LU:
				break;
			default:
				SetActionMove();
				break;
			}
				
			break;
		}
		case dfACTION_MOVE_LD:
		{
			_bDirection = LEFT;
			if (_iX >= MAX_LEFT && _iY <= MAX_BOTTOM)
			{
				_iX-= 3;
				_iY+= 2;
			}
			switch (_dwActionOld)
			{
			case dfACTION_MOVE_LL:
			case dfACTION_MOVE_LD:
			case dfACTION_MOVE_LU:
				break;
			default:
				SetActionMove();
				break;
			}
			break;
		}
		case dfACTION_MOVE_RU:
		{
			_bDirection = RIGHT;
			if (_iX <= MAX_RIGHT && _iY >= MAX_TOP)
			{
				_iX+= 3;
				_iY-= 2;
			}
			switch (_dwActionOld)
			{
			case dfACTION_MOVE_RR:
			case dfACTION_MOVE_RU:
			case dfACTION_MOVE_RD:
				break;
			default:
				SetActionMove();
				break;
			}
			break;
		}
		case dfACTION_MOVE_RD:
		{
			_bDirection = RIGHT;
			if (_iX <= MAX_RIGHT && _iY <= MAX_BOTTOM)
			{
				_iX+= 3;
				_iY+= 2;
			}
			switch (_dwActionOld)
			{
			case dfACTION_MOVE_RR:
			case dfACTION_MOVE_RD:
			case dfACTION_MOVE_RU:
				break;
			default:
				SetActionMove();
				break;
			}
			break;
		}
		//���ݸ޼��� ó��
		case dfACTION_ATTACK1:
		{
			if (isChange && _bAttack == false)
				SetActionAttack1();
			break;
		}
		case dfACTION_ATTACK2:
		{
			if (isChange && _bAttack == false)
				SetActionAttack2();
			break;
		}
		case dfACTION_ATTACK3:
		{
			if (isChange && _bAttack == false)
				SetActionAttack3();
			break;
		}
		default:
			//����ó��??
			return false;
			break;
	}
	return true;
}


/*
	���� : ���� ����
*/

void CPlayerObject::SetActionMove()
{
	if (_bIsMyPlayer == true)
	{
		if (_bAttack)
			return;
	}
	switch (_bDirection)
	{
		case LEFT:
		{
			_iStartIdx = 11;
			_iCurrentIdx = _iStartIdx;
			_iEndIdx = 22;

			_iDelayCount = 0;
			_iFrameDelay = dfDELAY_MOVE;
			break;
		}
		case RIGHT:
		{
			_iStartIdx = 23;
			_iCurrentIdx = _iStartIdx;
			_iEndIdx = 34;

			_iDelayCount = 0;
			_iFrameDelay = dfDELAY_MOVE;
			break;
		}
	}
}
void CPlayerObject::SetActionStand()
{
	if (_bIsMyPlayer == true)
	{
		if (_bAttack)
			return;
	}

	switch (_bDirection)
	{
	case LEFT:
	{
		_iStartIdx = 1;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 5;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_STAND;
		break;
	}
	case RIGHT:
	{
		_iStartIdx = 6;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 10;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_STAND;
		break;
	}
	}
}

void CPlayerObject::SetActionAttack1()
{
	if (_bIsMyPlayer == TRUE)
	{
		if (_bAttack)
			return;
		else
		{
			_bAttack = true;
			g_KeyProc = false;
		}
			
	}
	else
	{
		//------------------------------------------------------
		//	������ 17.01.26
		//
		//	���̵��� �ڲ� ���� ���ٱ淡
		//------------------------------------------------------
		_bAttack = true;
	}
	
	switch (_bDirection)
	{
	case LEFT:
	{
		_iStartIdx = 35;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 38;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK1;
		break;
	}
	case RIGHT:
	{
		_iStartIdx = 39;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 42;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK1;
		break;
	}
	}
}

void CPlayerObject::SetActionAttack2()
{
	if (_bIsMyPlayer == TRUE)
	{
		if (_bAttack)
			return;
		else
		{
			_bAttack = true;
			g_KeyProc = false;
		}
	}
	else
	{
		//------------------------------------------------------
		//	������ 17.01.26
		//
		//	���̵��� �ڲ� ���� ���ٱ淡
		//------------------------------------------------------
		_bAttack = true;
	}


	switch (_bDirection)
	{
	case LEFT:
	{
		_iStartIdx = 43;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 46;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK2;
		break;
	}
	case RIGHT:
	{
		_iStartIdx = 47;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 50;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK2;
		break;
	}
	}
}
void CPlayerObject::SetActionAttack3()
{
	if (_bIsMyPlayer == TRUE)
	{
		if (_bAttack)
			return;
		else
		{
			_bAttack = true;
			g_KeyProc = false;
		}
	}
	else
	{
		//------------------------------------------------------
		//	������ 17.01.26
		//
		//	���̵��� �ڲ� ���� ���ٱ淡
		//------------------------------------------------------
		_bAttack = true;
	}


	switch (_bDirection)
	{
	case LEFT:
	{
		_iStartIdx = 51;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 56;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK3;
		break;
	}
	case RIGHT:
	{
		_iStartIdx = 57;
		_iCurrentIdx = _iStartIdx;
		_iEndIdx = 62;

		_iDelayCount = 0;
		_iFrameDelay = dfDELAY_ATTACK3;
		break;
	}
	}
}