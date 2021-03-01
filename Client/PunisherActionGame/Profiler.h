#pragma once
#include <Windows.h>

#define dfCHAR_SIZE	64

/////////////////////////////////////////////////////////////////////////////
// ProfileSample Struct.
/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	bool		bFlag;				// 프로파일의 사용 여부.
	int		iProfileBegin;			// Begin 호출 횟수.
	int		iProfileOpen;			// End를 하지 않은 Begin

	char		szName[dfCHAR_SIZE];			// 프로파일 샘플 이름.
	LARGE_INTEGER	lStartTime;		// 프로파일 샘플 실행 시간.
	double		fAccumulatorTime;	// 본 샘플의 사용시간.
	double		fChildrenTime;		// 하위 함수들의 사용시간.
	int		iParentsNum;			// 상위 함수들의 개수.

	char szParentName[dfCHAR_SIZE];	// 부모 이름

} PROFILE_SAMPLE;



/////////////////////////////////////////////////////////////////////////////
// ProfileSample History Struct.
/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	bool	bFlag;				// 프로파일 샘플의 History 의 사용 여부.
	char	szName[dfCHAR_SIZE];			// 프로파일 샘플 이름.

	double	fAverage;			// 평균 사용시간 Time.
	double	fMin;				// 최소 사용시간 Time.
	double	fMax;			// 최대 사용시간 Time.

	double	fRateAverage;		// 평균 사용율
	double	fRateMin;			// 최소 사용율
	double	fRateMax;			// 최대 사용율

	char szParentName[dfCHAR_SIZE];		// 부모 이름
	__int64	iTCall;			// 누적 호출 횟수.

	int iParentsNum;
} PROFILE_HISTORY;

/***************************************************
유저 사용 함수
***************************************************/
bool InitProfiler();	// 사용시 최초에 한번 불러주세요;
bool OpenProfiler(char *name);	// 확인하고 싶은 구역 시작지점에 불러주세요;
bool EndProfiler(char *name);	// 확인하고 싶은 구역 끝지점에 불러주세요;
bool SaveProfiler();

/***************************************************
내부 함수
***************************************************/
bool SaveFile();