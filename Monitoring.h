#ifndef MONITORING_H
#define MONITORING_H


#include <windows.h>
#include <iostream>
#include <sstream> 
#include <fstream>
#include <cmath>
#include <string>
#include <psapi.h>


class MonitoringBase 
{
	private:
	protected:
		int test2 		= 0;
		long long temp2 = 0;
		int tem2 		= 0;
	public:
		virtual int CPULoad()  = 0;
		virtual SIZE_T  RAMTotal() = 0;
		virtual SIZE_T  RAMFree()  = 0;
		virtual SIZE_T  RAMUsed()  = 0;	
};

//Windows
class MonitoringWin: public MonitoringBase {
	private: 
		 MEMORYSTATUSEX memoryStatus; 
		 void GetMemoryStatus();
		 int test 		= 0;
		 long long temp = 0;
		 int tem 		= 0;
	public:
		int CPULoad();
		SIZE_T  RAMTotal();
		SIZE_T  RAMFree();
		SIZE_T  RAMUsed();
};

// Linux
class MonitoringLinux: public MonitoringBase {
	int CPULoad();
	SIZE_T  RAMTotal();
	SIZE_T  RAMFree();
	SIZE_T  RAMUsed();
};

#endif
