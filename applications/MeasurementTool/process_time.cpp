#include <windows.h>
#include "process_time.h"

process_time::process_time(HANDLE process)
	: m_nCpuUsage(-1)
	, m_dwLastRun(0)
	, m_lRunCount(0)
	, process_handle{ process }
{
	ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));
}
double process_time::GetUsage() {
	double nCpuCopy = m_nCpuUsage;
	if (::InterlockedIncrement(&m_lRunCount) == 1){ 
		if (!EnoughTimePassed()){
			::InterlockedDecrement(&m_lRunCount);
			return nCpuCopy;
        }
		FILETIME ftSysIdle, ftSysKernel, ftSysUser;
		FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

		if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
			!GetProcessTimes(process_handle, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser)){
				::InterlockedDecrement(&m_lRunCount);
		        return nCpuCopy;
		}

		if (!IsFirstRun()){
			ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, m_ftPrevSysKernel);
			ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, m_ftPrevSysUser);
			ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, m_ftPrevProcKernel);
			ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, m_ftPrevProcUser);
			ULONGLONG nTotalSys = ftSysKernelDiff + ftSysUserDiff;
			ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;
			if (nTotalSys > 0){
				m_nCpuUsage = (double)((100.0 * nTotalProc) / nTotalSys);
			}
		}
		m_ftPrevSysKernel = ftSysKernel;
		m_ftPrevSysUser = ftSysUser;
		m_ftPrevProcKernel = ftProcKernel;
		m_ftPrevProcUser = ftProcUser;
		m_dwLastRun = GetTickCount64();
		nCpuCopy = m_nCpuUsage;
	}

	::InterlockedDecrement(&m_lRunCount);

	return nCpuCopy;
}


ULONGLONG process_time::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB){
	LARGE_INTEGER a, b;
	a.LowPart = ftA.dwLowDateTime;
	a.HighPart = ftA.dwHighDateTime;
	b.LowPart = ftB.dwLowDateTime;
	b.HighPart = ftB.dwHighDateTime;
	return a.QuadPart - b.QuadPart;
}

bool process_time::EnoughTimePassed(){
	const int minElapsedMS = 250;//milliseconds
	ULONGLONG dwCurrentTickCount = GetTickCount64();
	return (dwCurrentTickCount - m_dwLastRun) > minElapsedMS;
}