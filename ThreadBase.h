#ifndef THREADBASE_H
#define THREADBASE_H

/*
	Base class thread 

	@kk 
*/

#if _WIN32
	#include "port_windows.h"	
	#include <Windows.h>
#elif __linux__
	#include "port_linux.h"	
#else 
	#error "UNKNOWN TARGET OS"
#endif

#include <mutex>
#include <thread>

class ThreadBase
{
	private:
		std::thread 	FThreadBase;
		uint8_t 		FTHID 			= 0x00;
		virtual void 	StartExecute()	= 0; 
	protected:
		std::mutex		FMutexBase;
		bool			Terminated 		= false;
		unsigned int	PollingTime 	= 20;	
	public:
		ThreadBase();
		~ThreadBase();
		
		void setPollingTime(int _timeMS); 
		
		void thWaitForCompletion();		
		void thStart();
		void thStop();
		
		
};

#endif
