#ifndef TASKCONNECTDS_H
#define TASKCONNECTDS_H

#include "TaskList.h"
#include <sys/stat.h>
#include "Logger.h"


class TaskConnect: 
	public TaskBaseConnect,
	public TaskaBase  
{
	private: 
		int FClientSocket = 0;
		thClientConnect* ClientDS = nullptr;
	public:

	TaskConnect() {
		ClientDS = new thClientConnect();	
//		Logger::log(Logger::LogLevel::log_DEBUG, "***TASK CONNECT CREATE!***");	
	}
	
    
    void setIndex(int _cls) { 
    	FClientSocket = _cls;	
	}

    //Th     
    void Execute() {
    	try {
			std::unique_lock<std::mutex> lock(tasksMutex); 
			if(FTaskCMD == nullptr) {
				Logger::log(Logger::LogLevel::log_ERROR, "No command!");
				return;
			}
			
    		str_SGP2_ML_Message ClientMessage = this->getMessage();

			if( ClientDS == nullptr) { 
				Logger::log(Logger::LogLevel::log_ERROR, "NOT CREATE Client DS!");
				return;
			}
			
			ClientDS->init();
			ClientDS->setSocket(cHost, usPort, uiAddress);
			ClientDS->setUser(cUser, cPass);
			ClientDS->setIndex(FClientSocket);	
			FTaskCMD->setClientMessage(&ClientMessage);
			ClientDS->setCommand(FTaskCMD);

//			Logger::log(Logger::LogLevel::log_DEBUG, "Client start...");	
			
			ClientDS->Execute();// Отправляем запрос задачи
			
			
			// ---- Final reqest ---- 
//			Logger::log(Logger::LogLevel::log_DEBUG, "Client termination - ok!");	
		
			unsigned char state = ClientDS->getLastError();
			if (state != SGP2_ML_RESULT_SUCCESS){
				Logger::log(Logger::LogLevel::log_ERROR, "LAST ERROR: %d ", state);	
			}
			
		    str_SGP2_ML_Message* _msgResponseClient = nullptr;	
		    
			if(state == SGP2_ML_RESULT_SUCCESS) {
//				 Logger::log(Logger::LogLevel::log_DEBUG, "Client SGP2_ML_RESULT_SUCCESS!");		
				_msgResponseClient = FTaskCMD->getClientResponseMsg();
				usParam	= _msgResponseClient->ucPar;
			} else {
				Logger::log(Logger::LogLevel::log_ERROR, "Client SGP2_ML_RESULT_error!");	
				_msgResponseClient = ClientDS->getMessageError();	
				usParam = _msgResponseClient->ucPar;	
			}		
			  
			ucResponse 	   = _msgResponseClient->pcData;
			usResponseSize = _msgResponseClient->usDataSize; 
			FCompleted     = true;	

		    setState(ID, ts_Ready);	
		    
//		    Logger::log(Logger::LogLevel::log_DEBUG, "Mode: READY!");
		    int i = 0;
		    while (true) {
			   if(getState() >= ts_Success) { 
			   	  break;
			   } 	
			   if(i >= 300) { 
			   	 Logger::log(Logger::LogLevel::log_ERROR, "Mode: TIMEOUT!"); 
			   	 break;
			   }
			   i++;    
			   vPortSleepMs(1);
			}
//		    Logger::log(Logger::LogLevel::log_DEBUG, "Mode: SUCCESS!"); 

			// ----- 
//			Logger::log(Logger::LogLevel::log_DEBUG, "Client Exit!");
			taskCompleted = true;  // Устанавливаем флаг завершения
		} catch (...) {
			Logger::log(Logger::LogLevel::log_ERROR, "***Task error!***");
			setState(ID, ts_Error);	
		} 
    	
	}
	
	bool directoryExists(const std::string& dirPath) {
	    struct stat info;
	    if (stat(dirPath.c_str(), &info) != 0) {
	        return false; // Ошибка при получении информации о файле
	    }
	    return (info.st_mode & S_IFDIR) != 0; // Проверка, является ли это каталогом
	}
    	
};



#endif


