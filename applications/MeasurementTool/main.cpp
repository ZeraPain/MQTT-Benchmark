#include <Windows.h>
#include "psapi.h"
#include <TlHelp32.h>
#include <string>
#include <iostream>

DWORD getProcessId(const std::string& aProcessName)
{
	DWORD processId = 0;

	const auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapshot, &pe32) == TRUE)
		{
			do
			{
				if (pe32.szExeFile == aProcessName)
				{
					processId = pe32.th32ProcessID;
				}
			} while (Process32Next(hSnapshot, &pe32) == TRUE && 0 == processId);
		}

		CloseHandle(hSnapshot);
	}

	return processId;
}

int main()
{
	// user defined values
	const std::string processName = "ts3client_win64.exe";
	const auto measureInterval = 1000;

	// program code
	const auto processId = getProcessId(processName);
	if (processId > 0)
	{
		const auto processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

		// setup memory info
		PROCESS_MEMORY_COUNTERS_EX pmc;
		ZeroMemory(&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));

		// setup cpu info
		ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;

		SYSTEM_INFO sysInfo;
		FILETIME ftime, fsys, fuser;

		GetSystemInfo(&sysInfo);
		const int numProcessors = sysInfo.dwNumberOfProcessors;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&lastCPU, &ftime, sizeof(FILETIME));

		GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);
		memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
		memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

		DWORD exitCode;

		std::cout << "Printing resource values of [" << processName << "] PID: " << processId << std::endl;
		do
		{
			Sleep(measureInterval);

			GetProcessMemoryInfo(processHandle, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));
			const SIZE_T virtualMemUsedByMe = pmc.PrivateUsage / 1024 / 1024;
			//SIZE_T workingSetSize = pmc.WorkingSetSize;

			ULARGE_INTEGER now, sys, user;

			GetSystemTimeAsFileTime(&ftime);
			memcpy(&now, &ftime, sizeof(FILETIME));

			GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);
			memcpy(&sys, &fsys, sizeof(FILETIME));
			memcpy(&user, &fuser, sizeof(FILETIME));
			double percent = (sys.QuadPart - lastSysCPU.QuadPart) +
				(user.QuadPart - lastUserCPU.QuadPart);
			percent /= (now.QuadPart - lastCPU.QuadPart);
			percent /= numProcessors;
			percent *= 100;
			lastCPU = now;
			lastUserCPU = user;
			lastSysCPU = sys;

			GetExitCodeProcess(processHandle, &exitCode);
			std::cout << "CPU: " << percent << "% \t RAM: " << virtualMemUsedByMe << "MB" <<  std::endl;
		} while (0 != exitCode);

		CloseHandle(processHandle);
	}
	else
	{
		std::cout << "Unable to find process" << std::endl;
		system("PAUSE");
	}

	return 0;
}