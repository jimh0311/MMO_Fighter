#pragma once
class CFrameControl
{
public:
	CFrameControl(int needFrame);
	~CFrameControl();
	bool FrameCheck();
private:

	int _iCheck;

	int _iBeginTime;
	int _iPrevEndTime;
	
	int _iPerFrameTime;

	int _iExtraTime;
	int _iTempTime;
	
	const DWORD _iDelayFrame;	// �� ���� ������ ���������� �����ڿ��� �޴´�.
	
};