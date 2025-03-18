#ifndef TASKRS_GETROUPS_H
#define TASKRS_GETROUPS_H

#include "TaskList.h"
#include "SGP2/SGP2.h"

// get list Groups 
class TaskRS_GetGroups: public TaskaBase 
{
	private:
		std::mutex tasksMutex;
		str_SGP2_ML_Message* test; 
	public:
//		TaskRS_GetGroups(int _socket, const str_SGP2_ML_Message& _pxSrcRequestMessage, str_SGP2_ML_Message* _pxDstResponseMessage) 
//        : TaskaBase(_socket, _pxSrcRequestMessage, _pxDstResponseMessage) {

//		}  

	
//	TaskRS_GetGroups() 
//        : TaskaBase(){
//    }	
        
    //Th     
    void Execute() {
    	
    	
    	try {
			std::lock_guard<std::mutex> lock(tasksMutex); 
			
    		str_SGP2_ML_Message ClientMessage = this->getMessage();
			thClientConnect* ClientRS = new thClientConnect();
			
			ClientRS->setSocket("tvzconnect.ru", 7000, 0);
			ClientRS->setUser("User","yi54NkK02hovXZpt");
			ClientRS->setIndex(1);
			
			cmdTask* _cmdTask = new cmdTask();
			_cmdTask->setClientMessage(&ClientMessage);
			
			ClientRS->setCommand(_cmdTask);
			ClientRS->startThread();
			ClientRS->joinThread();
			
			ucResponse = (unsigned char*)malloc(usResponseSize* sizeof(unsigned char));
			if (ucResponse == nullptr) { return; } 
			
		    str_SGP2_ML_Message* _msg = _cmdTask->getClientResponseMsg();
			
			// For sending client
			ucResponse     = _msg->pcData; 
			usResponseSize = _msg->usDataSize; 
			
		    setState(ID, ts_Ready);	
		} catch (...) {
			printf("Task error \n");
			setState(ID, ts_Error);	
		} 
    	
	}
};



#endif


