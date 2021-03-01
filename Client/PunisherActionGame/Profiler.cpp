#include "stdafx.h"
#include "Profiler.h"
#include <stdio.h>
/***************************************************
PROFILER DEFINE
***************************************************/

#define dfPROFILE_SIZE	256
#define FILE_NAME "Profiler.txt"

/***************************************************
PROFILER 전역함수
***************************************************/

PROFILE_SAMPLE stProSample[dfPROFILE_SIZE];
PROFILE_HISTORY stProHistory[dfPROFILE_SIZE];
LARGE_INTEGER lFreq, lStart, lEnd, lFirstTime;

int _numOfFrames = 0;
bool firstOpen = true;
int _numOfProfileOpen = 0;

/***************************************************
유저 사용 함수
***************************************************/

bool InitProfiler()
{
	QueryPerformanceFrequency(&lFreq);
	QueryPerformanceCounter(&lStart);
	QueryPerformanceCounter(&lFirstTime);
	memset(&stProSample, 0, sizeof(PROFILE_SAMPLE)*dfPROFILE_SIZE);
	memset(&stProHistory, 0, sizeof(PROFILE_HISTORY)*dfPROFILE_SIZE);
	return true;
}
bool OpenProfiler(char *name)
{
	if (firstOpen == true)
	{
		QueryPerformanceCounter(&lStart);
		firstOpen = false;
	}
	//이미 name에 해당하는 함수가 한 번 저장이 되어있는 경우라면???
	for (int j = 0; j < dfPROFILE_SIZE; j++)
	{
		if (stProSample[j].bFlag == true && strcmp(stProSample[j].szName, name) == 0)
		{
			stProSample[j].iProfileBegin++;
			stProSample[j].iProfileOpen++;
			QueryPerformanceCounter(&(stProSample[j].lStartTime));
			_numOfProfileOpen++;
			return true;
		}
	}

	//최초로 오픈 된 경우에...
	for (int i = 0; i < dfPROFILE_SIZE; i++)
	{
		// 비어있는 공간을 찾는다.
		// 
		if (stProSample[i].bFlag == false)
		{
			//전역함수 오픈
			stProSample[i].bFlag = true;
			stProSample[i].iProfileBegin = 1;
			stProSample[i].iProfileOpen = 1;
			strcpy_s(stProSample[i].szName, dfCHAR_SIZE, name);
			QueryPerformanceCounter(&(stProSample[i].lStartTime));

			stProSample[i].iParentsNum = _numOfProfileOpen;
			_numOfProfileOpen++;
			//
			//	이미 전에 오픈이 되어있다면, 고아 프로파일러의 본격 부모님 찾기 프로젝트?
			//	
			if (stProSample[i].iParentsNum != 0)
			{
				for (int k = 0; k < dfPROFILE_SIZE; k++)
				{
					if (stProSample[k].bFlag == true && stProSample[k].iParentsNum == stProSample[i].iParentsNum - 1)
					{
						strcpy_s(stProSample[i].szParentName, dfCHAR_SIZE, stProSample[k].szName);
						break;
					}
				}
			}
			return true;
		}
	}
	return false;
}

/***************************************************************************/

bool EndProfiler(char *name)
{
	for (int i = 0; i < dfPROFILE_SIZE; i++)
	{
		if (stProSample[i].bFlag == true && strcmp(stProSample[i].szName, name) == 0)
		{
			//
			//	종료시간을 저장한다;
			//
			QueryPerformanceCounter(&lEnd);
			stProSample[i].iProfileOpen--;	// 정상적인 종료시 모든 노드의 Open = 0 이 되어있어야 함.
			stProSample[i].fAccumulatorTime += lEnd.QuadPart - stProSample[i].lStartTime.QuadPart;
			//
			//	고아가 아닌 프로파일러는 부모님에게 자신의 실행시간을 입금시킨다... ㅠㅠ
			//
			if (stProSample[i].iParentsNum != 0)
			{
				for (int k = 0; k < dfPROFILE_SIZE; k++)
				{
					if (stProSample[k].bFlag == true && strcmp(stProSample[k].szName, stProSample[i].szParentName) == 0)
					{
						stProSample[k].fChildrenTime += stProSample[i].fAccumulatorTime;
						break;
					}
				}
			}
			_numOfProfileOpen--;
			return true;
		}
	}
	return false;
}

/***************************************************************************/

bool SaveProfiler()
{
	QueryPerformanceCounter(&lEnd);
	//double allTemp = lEnd.QuadPart - lStart.QuadPart;
	bool flag = false;
	//HSITORY에 복제한다;
	for (int i = 0; i < dfPROFILE_SIZE; i++)
	{
		if (stProSample[i].bFlag == true)
		{
			// 이미 한번 선언되어 있는 경우;
			for (int k = 0; k < dfPROFILE_SIZE; k++)
			{
				if (stProHistory[k].bFlag == true && strcmp(stProHistory[k].szName, stProSample[i].szName) == 0)
				{
					double temp = stProSample[i].fAccumulatorTime;

					stProHistory[k].fAverage += stProSample[i].fAccumulatorTime;
					//추 후에 저장시 iTCall 과 계산할거임;
					if (temp < stProHistory[k].fMin)
					{
						stProHistory[k].fMin = temp;
					}
					if (stProHistory[k].fMax < temp)
					{
						stProHistory[k].fMax = temp;
					}

					temp = stProSample[i].fAccumulatorTime - stProSample[i].fChildrenTime;
					stProHistory[k].fRateAverage += (stProSample[i].fAccumulatorTime - stProSample[i].fChildrenTime);
					if (stProHistory[k].fMax < temp)
					{
						stProHistory[k].fMax = temp;
					}
					if (stProHistory[k].fMin > temp)
					{
						stProHistory[k].fMin = temp;
					}

					stProHistory[k].iTCall += stProSample[i].iProfileBegin;
					flag = true;
					break;
				}
			}
			if (flag == true)
				continue;
			// 최초 선언된 경우;
			for (int j = 0; j < dfPROFILE_SIZE; j++)
			{
				if (stProHistory[j].bFlag == false)
				{
					stProHistory[j].bFlag = true;
					strcpy_s(stProHistory[j].szName, dfCHAR_SIZE, stProSample[i].szName);

					stProHistory[j].fAverage = stProSample[i].fAccumulatorTime;
					stProHistory[j].fMin = stProSample[i].fAccumulatorTime;
					stProHistory[j].fMax = stProSample[i].fAccumulatorTime;

					stProHistory[j].fRateAverage = (stProSample[i].fAccumulatorTime - stProSample[i].fChildrenTime);
					stProHistory[j].fRateMin = stProHistory[j].fRateAverage;
					stProHistory[j].fRateMax = stProHistory[j].fRateAverage;
					strcpy_s(stProHistory[j].szParentName, dfCHAR_SIZE, stProSample[i].szParentName);
					stProHistory[j].iTCall = stProSample[i].iProfileBegin;
					stProHistory[j].iParentsNum = stProSample[i].iParentsNum;
					break;
				}
			}
		}
	}

	memset(&stProSample, 0, sizeof(PROFILE_SAMPLE)*dfPROFILE_SIZE);
	firstOpen = true;
	_numOfFrames++;
	if (SaveFile() == true)
		return true;
	else
		return false;
}

/***************************************************
내부 함수
***************************************************/
bool SaveFile()
{
	QueryPerformanceCounter(&lEnd);
	double temp = (lEnd.QuadPart - lFirstTime.QuadPart) / (double)lFreq.QuadPart;
	FILE *fp;
	fopen_s(&fp, FILE_NAME, "wb");
	if (fp != NULL)
	{
		fprintf_s(fp, "\tAverage\t\t\t|\t\tMin\t\t|\t\tMax\t\t|\tF.Call / T.Call\t\t|\t\r\n");
		fprintf_s(fp, "---------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
		for (int i = 0; i < dfPROFILE_SIZE; i++)
		{
			if (stProHistory[i].bFlag == true)
			{
				fprintf_s(fp, "\t%.6Lfs (%.2Lf %%)\t|\t%.6Lfs (%.2Lf %%)\t|\t%.6Lfs (%.2Lf %%)\t|\t%lld \t/ %lld\t\t|\t",
					(stProHistory[i].fAverage / (double)lFreq.QuadPart) / (stProHistory[i].iTCall / _numOfFrames), (stProHistory[i].fRateAverage) / lFreq.QuadPart / (temp / _numOfFrames) / stProHistory[i].iTCall * 100,
					stProHistory[i].fMin / lFreq.QuadPart, ((stProHistory[i].fRateMin) / lFreq.QuadPart) / (temp / _numOfFrames) * 100,
					stProHistory[i].fMax / lFreq.QuadPart, (stProHistory[i].fRateMax) / lFreq.QuadPart / (temp / _numOfFrames) * 100,
					stProHistory[i].iTCall / _numOfFrames, stProHistory[i].iTCall);
				for (int j = 0; j < stProHistory[i].iParentsNum; j++)
				{
					fprintf_s(fp, "  ");
				}
				fprintf_s(fp, "%s \r\n", stProHistory[i].szName);
			}
		}
		fprintf_s(fp, "\r\nTotal Time : %.5Lfs \r\nTotal Frames : %d \r\nEach Frame`s Time : %.5Lfs",
			temp, _numOfFrames, temp / _numOfFrames);
		fclose(fp);
		return true;
	}
	return false;
}