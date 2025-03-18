#include "ThreadBase.h"

ThreadBase::ThreadBase()
{
}

ThreadBase::~ThreadBase()
{
}

// Start thread
void ThreadBase::thStart()  
{
	std::lock_guard<std::mutex> lock(FMutexBase); 
	
	Terminated = false;	
	
	if(FTHID == 0xFE) { 
//		Logger::log(Logger::LogLevel::log_DEBUG, "thRequestinfo |thStart | thread is run | return");
		return;	
	}
	FTHID = 0xFE;
	FThreadBase = std::thread(&ThreadBase::StartExecute, this); 	
}


void ThreadBase::thStop() 
{ 
	std::lock_guard<std::mutex> lock(FMutexBase); 
	
	Terminated = true;
	FTHID = 0x00;
}	


void ThreadBase::thWaitForCompletion()
{
	std::lock_guard<std::mutex> lock(FMutexBase); 
		
	if (FThreadBase.joinable()) {
		FThreadBase.join(); 
	}
}

// set timeout 
void ThreadBase::setPollingTime(int _timeMS) 
{
	PollingTime = _timeMS;	
}


//void ThreadBase::StartExecute()
//{
//	while(!Terminated)
//	{
//		vPortSleepMs(PollingTime); /*20*/
//		std::lock_guard<std::mutex> lock(MutexBase);
//	}
//}




