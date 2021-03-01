#pragma once
#include <Windows.h>

#define dfCHAR_SIZE	64

/////////////////////////////////////////////////////////////////////////////
// ProfileSample Struct.
/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	bool		bFlag;				// ���������� ��� ����.
	int		iProfileBegin;			// Begin ȣ�� Ƚ��.
	int		iProfileOpen;			// End�� ���� ���� Begin

	char		szName[dfCHAR_SIZE];			// �������� ���� �̸�.
	LARGE_INTEGER	lStartTime;		// �������� ���� ���� �ð�.
	double		fAccumulatorTime;	// �� ������ ���ð�.
	double		fChildrenTime;		// ���� �Լ����� ���ð�.
	int		iParentsNum;			// ���� �Լ����� ����.

	char szParentName[dfCHAR_SIZE];	// �θ� �̸�

} PROFILE_SAMPLE;



/////////////////////////////////////////////////////////////////////////////
// ProfileSample History Struct.
/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	bool	bFlag;				// �������� ������ History �� ��� ����.
	char	szName[dfCHAR_SIZE];			// �������� ���� �̸�.

	double	fAverage;			// ��� ���ð� Time.
	double	fMin;				// �ּ� ���ð� Time.
	double	fMax;			// �ִ� ���ð� Time.

	double	fRateAverage;		// ��� �����
	double	fRateMin;			// �ּ� �����
	double	fRateMax;			// �ִ� �����

	char szParentName[dfCHAR_SIZE];		// �θ� �̸�
	__int64	iTCall;			// ���� ȣ�� Ƚ��.

	int iParentsNum;
} PROFILE_HISTORY;

/***************************************************
���� ��� �Լ�
***************************************************/
bool InitProfiler();	// ���� ���ʿ� �ѹ� �ҷ��ּ���;
bool OpenProfiler(char *name);	// Ȯ���ϰ� ���� ���� ���������� �ҷ��ּ���;
bool EndProfiler(char *name);	// Ȯ���ϰ� ���� ���� �������� �ҷ��ּ���;
bool SaveProfiler();

/***************************************************
���� �Լ�
***************************************************/
bool SaveFile();