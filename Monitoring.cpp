#include "Monitoring.h"




//=== Windows =====================================================================


	int MonitoringWin::CPULoad() {
		static ULARGE_INTEGER lastIdleTime = {0}, lastKernelTime = {0}, lastUserTime = {0};
		
		FILETIME idleTime, kernelTime, userTime;
		GetSystemTimes(&idleTime, &kernelTime, &userTime);
		
		ULARGE_INTEGER currentIdleTime, currentKernelTime, currentUserTime;
		currentIdleTime.LowPart = idleTime.dwLowDateTime;
		currentIdleTime.HighPart = idleTime.dwHighDateTime;
		currentKernelTime.LowPart = kernelTime.dwLowDateTime;
		currentKernelTime.HighPart = kernelTime.dwHighDateTime;
		currentUserTime.LowPart = userTime.dwLowDateTime;
		currentUserTime.HighPart = userTime.dwHighDateTime;
		
		ULONGLONG totalTime 	= (currentKernelTime.QuadPart - lastKernelTime.QuadPart) +
								  (currentUserTime.QuadPart   - lastUserTime.QuadPart);
		ULONGLONG idleTimeDelta =  currentIdleTime.QuadPart   - lastIdleTime.QuadPart;
		
		float cpuUsage = 100.0f * (1.0 - (static_cast<float>(idleTimeDelta) / static_cast<float>(totalTime)));
		
		lastIdleTime = currentIdleTime;
		lastKernelTime = currentKernelTime;
		lastUserTime = currentUserTime;
		
		return std::round(cpuUsage);	
	}
	
	
	void MonitoringWin::GetMemoryStatus() {
	    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	    
	    if (!GlobalMemoryStatusEx(&memoryStatus)) {
	        std::cerr << "Failed to get memory status. Error code: " << GetLastError() << "\n";
	    }
	}
	
	SIZE_T MonitoringWin::RAMTotal() {
	    GetMemoryStatus(); 
	    return memoryStatus.ullTotalPhys; 
	}
	
	SIZE_T MonitoringWin::RAMFree() {
	    GetMemoryStatus(); 
	    return memoryStatus.ullAvailPhys; 
	}
	
	SIZE_T MonitoringWin::RAMUsed() {
	    GetMemoryStatus(); 
	    return memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys; 
	}

// ===============================================================================



// ===== Linux ===================================================================

int MonitoringLinux::CPULoad() {
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line);

    std::istringstream ss(line);
    std::string cpu;
    unsigned long user, nice, system, idle;

    ss >> cpu >> user >> nice >> system >> idle;

    static unsigned long lastTotal = 0;
    static unsigned long lastIdle = 0;

    unsigned long total = user + nice + system + idle;
    unsigned long totalDelta = total - lastTotal;
    unsigned long idleDelta = idle - lastIdle;

    float cpuUsage = 100.0f * (1.0 - (static_cast<float>(idleDelta) / static_cast<float>(totalDelta)));

    lastTotal = total;
    lastIdle = idle;

    return std::round(cpuUsage);
}



SIZE_T  MonitoringLinux::RAMTotal() {
	
}


SIZE_T  MonitoringLinux::RAMFree() {
	
}

SIZE_T  MonitoringLinux::RAMUsed() {
	
}


// ===============================================================================









