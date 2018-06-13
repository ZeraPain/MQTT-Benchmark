#pragma once

#include <windows.h>
class process_time
{
public:
	process_time(HANDLE process);
	double GetUsage();

private:
	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
	bool EnoughTimePassed();
	inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

	HANDLE process_handle;

    //system total times
	FILETIME m_ftPrevSysKernel;
	FILETIME m_ftPrevSysUser;

	//process times
	FILETIME m_ftPrevProcKernel;
	FILETIME m_ftPrevProcUser;
	double m_nCpuUsage;
    ULONGLONG m_dwLastRun;
	volatile LONG m_lRunCount;
};

