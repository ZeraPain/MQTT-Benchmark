#include <Windows.h>
#include "Psapi.h"
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

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

std::string getTimeStamp()
{
	time_t rawtime;
	struct tm timeinfo{};
	char buffer[80];
	
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeinfo);
	return std::string(buffer);
}

void writeLogFile(const std::string& aProcessName, double aCpuPercentage, size_t aRamValue)
{
	std::ofstream fileStream;
	if (!fileStream.is_open())
	{
		fileStream.open(aProcessName + ".log", std::ios_base::app);
	}
	fileStream << getTimeStamp() << ";" << aCpuPercentage << ";" << aRamValue << "\n";
}

int main(int argc, char* argv[])
{
	if (argc < 3) {
		// Tell the user how to run the program
		std::cerr << "Usage: " << argv[0] << " <processname> <delay>" << std::endl;
		/* "Usage messages" are a conventional way of telling the user
		* how to run a program if they enter the command incorrectly.
		*/
		return 1;
	}

	// user defined values
	const std::string processName = argv[1];
	const auto measureInterval = atoi(argv[2]);

	// program code
	int processId;

	std::cout << "Searching for process... ";

	do
	{
		processId = getProcessId(processName);
	} while (0 == processId);

	std::cout << "found! Process ID [" << processId << "]" << std::endl;

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
		const auto virtualMemUsedByMe = pmc.PrivateUsage;
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
		std::cout << "CPU: " << percent << "% \t RAM: " << virtualMemUsedByMe / 1024 / 1024 << "MB" << std::endl;
		writeLogFile(processName, percent, virtualMemUsedByMe);
	} while (STILL_ACTIVE == exitCode);

	CloseHandle(processHandle);

	std::cout << "Process terminated! Stopping measurements." << std::endl;
	system("PAUSE");

	return 0;
}