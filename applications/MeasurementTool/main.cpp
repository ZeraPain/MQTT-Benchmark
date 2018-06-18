#include <Windows.h>
#include "Psapi.h"
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

struct log_struct
{
	int		logID;
	double	cpuPercent;
	SIZE_T	ramUsage;
};

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

struct Comma final : std::numpunct<char>
{
	char do_decimal_point() const override { return ','; }
};

void writeLogFile(const std::string& aProcessName, const std::vector<log_struct>& aLogVector)
{
	std::ofstream fileStream;
	fileStream.open(aProcessName + ".txt");

	fileStream.imbue(std::locale(std::locale::classic(), new Comma));

	for (auto& log : aLogVector)
	{
		fileStream 
			<< log.logID << "\t" 
			<< log.cpuPercent << "\t" 
			<< std::to_string(log.ramUsage) << "\n";
	}

	fileStream.close();
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
	const auto measureMax = atoi(argv[2]);

	// program code
	int processId;

	std::cout << "Searching for process... " << std::endl;

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

	std::cout << "Printing resource values of [" << processName << "] PID: " << processId << std::endl;
	std::vector<log_struct> logger;

	int logIndex = 0;
	DWORD exitCode;

	do
	{
		Sleep(1000);

		GetProcessMemoryInfo(processHandle, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));
		const auto virtualMemUsedByMe = pmc.PrivateUsage;
		//SIZE_T workingSetSize = pmc.WorkingSetSize;

		ULARGE_INTEGER now, sys, user;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		double percent = (double)((sys.QuadPart - lastSysCPU.QuadPart) +
			(user.QuadPart - lastUserCPU.QuadPart));
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= numProcessors;
		percent *= 100;
		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;

		GetExitCodeProcess(processHandle, &exitCode);
		++logIndex;
		std::cout << "[" << logIndex << "/" << measureMax << "]\t"
			<< " CPU: " << percent << "% \t RAM: " << virtualMemUsedByMe / 1024 / 1024 << "MB" << std::endl;
		logger.push_back({ logIndex , percent , virtualMemUsedByMe });

	} while (STILL_ACTIVE == exitCode && logIndex < measureMax);

	std::cout << "Process terminated! Stopping measurements." << std::endl;
	CloseHandle(processHandle);

	std::cout << "Storing results to [" << processName  << ".txt]" << std::endl;
	writeLogFile(processName, logger);

	return 0;
}