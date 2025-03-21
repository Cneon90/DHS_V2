#ifndef TASKBASE_HK
#define TASKBASE_HK

#include <atomic>

#include "thClient.h"
#include "Session.h"
#include "SGP2/SGP2.h"


#define STATE_INIT       100
#define STATE_WAIT       102  
#define STATE_ERROR      404



enum TaskState {
    ts_Wait    = 10,          
    ts_Process = 11,
    ts_Ready   = 12,
    ts_Success = 13,
    ts_Error   = 14
//	,ts_Delete  = 15
	, ts_Used  = 15 // После использования, (не удаляем!)
	, ts_free  = 16
	
};

//struct TaskData { 
//	unsigned char* ucRequst;
//	unsigned char* ucResponse;	
//};

// (Base) One task
class TaskaBase 
{
	protected: 
	    virtual void Execute() = 0;
//	    const str_SGP2_ML_Message FRequestMessage; // CLient - Request
		str_SGP2_ML_Message FRequestMessage; //
    	TaskState State 			  = ts_Wait;
		std::thread thTask;
		
		int ID			              = 0; // ID task
		int TTL			              = 0; 
		short    sPayload 			  = 0;  
		uint64_t TimeStart 			  = 0;
		unsigned char* ucResponse     = 0;	
		unsigned short usResponseSize = 0; 
		unsigned short usParam        = 0;
		
		
		
		std::atomic<bool> taskCompleted;
		std::atomic<bool> Reset;
		bool FCompleted 			  = false;
		bool FSendResult              = false;
		
	
		
		str_SGP2_ML_Message*      ResponseMessage = nullptr; // only RESPONSE!
		
		std::mutex mtxTask;	
		std::mutex mtxState;
//		std::mutex mtxState2;
		std::mutex joinmtx;
//		std::mutex mtxInit;
	public:
		// Constructor
//		TaskaBase(int _socket, const str_SGP2_ML_Message& _pxSrcRequestMessage, str_SGP2_ML_Message* _pxDstResponseMessage)
//    	: FRequestMessage(_pxSrcRequestMessage), State(ts_Wait), ID(_socket), ResponseMessage(_pxDstResponseMessage) {
//	
//		}

		// Конструктор без параметров
 		TaskaBase() : FRequestMessage(str_SGP2_ML_Message()), State(ts_Wait), ID(0), ResponseMessage(nullptr) {
 			taskCompleted = false;
 			TimeStart = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
 			Reset = true;
		 }
		 
		 void setCompleted() { 
		 	std::unique_lock<std::mutex> lock(mtxTask);
//		 	if(thTask.joinable()) { 
				taskCompleted = true;	
//			} else { 
//				taskCompleted = false;	
//			}		 	
		 }
		 
		void setProcessed() { 
		 	std::unique_lock<std::mutex> lock(mtxTask);
			taskCompleted = false;	
		 }
		 
		 
		// Завершилось выполнение или нет	
		bool IsTaskCompleted() {
			std::unique_lock<std::mutex> lock(mtxTask);

        	return taskCompleted.load(std::memory_order_acquire);  // Проверяем флаг
    	}

	   bool getCompleted() { 
	   		std::unique_lock<std::mutex> lock(mtxTask);
	   		
	   		bool t = FCompleted;
	   		FCompleted = false;
	   		return t;
	   }
	   
	   bool getSendResult() { 
	   		std::unique_lock<std::mutex> lock(mtxTask);
	   		
	   		bool t = FSendResult;
	   		FSendResult = false; 
	   		return t;	
	   } 	
		
	    // Методы set для установки значений
	    void setRequestMessage(const str_SGP2_ML_Message& _pxSrcRequestMessage) {
	    	std::unique_lock<std::mutex> lock(mtxTask);
	        FRequestMessage = _pxSrcRequestMessage;
	    }
	

	    void setID(int _socket) {
	    	std::unique_lock<std::mutex> lock(mtxTask);	
	        ID = _socket;
	    }
	    
	    int getID() { 
	    	std::unique_lock<std::mutex> lock(mtxTask);	
	    	return ID;
		}
	
	    void setResponseMessage(str_SGP2_ML_Message* _pxDstResponseMessage) {
	    	std::unique_lock<std::mutex> lock(mtxTask);
	        ResponseMessage = _pxDstResponseMessage;
	    }
    	
    	
		// -----------------------
		
		void Init() {
			std::unique_lock<std::mutex> lock(mtxTask);
			ResponseMessage->ucPar = STATE_INIT;	
		}
		
		void setTimeStart(uint64_t _time){
			std::lock_guard<std::mutex> lock(mtxTask);	
			TimeStart = _time;	
		}
		
		int getTimeStart() { 
			std::lock_guard<std::mutex> lock(mtxTask);
			return TimeStart;
		}
		
		
		// Message (set,get)
		const str_SGP2_ML_Message& getMessage() const {
//			std::lock_guard<std::mutex> lock(mtxTask);	
			return FRequestMessage; 
		}
		
//		int getID() {
//			std::lock_guard<std::mutex> lock(mtxTask); 
//			return ID; 
//		}
		
		void task_reset() { 
			std::lock_guard<std::mutex> lock(mtxState); 
//			
//			mtxTask.lock(); 
//			Logger::log(Logger::LogLevel::log_DEBUG, "ENTER INIT task %d, ", getID());
			ID			   = 0; 
			TTL			   = 0; 
			sPayload 	   = 0;  
			TimeStart 	   = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			ucResponse     = 0;	
			usResponseSize = 0; 
			usParam        = 0;
			taskCompleted  = false;
			Reset          = true;
			
//			mtxState.lock(); 
			State 		   = ts_Wait;
//			mtxState.unlock(); 
			
			FCompleted  = false;
			FSendResult = false; 
			
			ResponseMessage               = nullptr;
			FRequestMessage.pcData        = nullptr;
			FRequestMessage.ucPar         = 0;
			FRequestMessage.usCmd         = 0;
			FRequestMessage.usDataMaxSize = 0; 
			FRequestMessage.usDataSize    = 0;
 
//			this->setTimeStart(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
//			Logger::log(Logger::LogLevel::log_DEBUG, "INIT task %d, ", getID());
//			mtxTask.unlock(); 
		}
		
		bool isJoinable() { 
			return thTask.joinable();
		}
		
		//---------- Thread ----------	
		void startThread() { 
//			if (thTask.joinable()) {
//            	thTask.join();  // Дожидаемся завершения потока
//        	}        	
//        	Logger::log(Logger::LogLevel::log_DEBUG, "Before start thread task");
			thTask = std::thread(&TaskaBase::Execute, this); 
			thTask.detach();
		}	
	    
		void thWaitForCompletion() {
			if (thTask.joinable()) {
				thTask.join(); 
			}
		}
	
	    virtual ~TaskaBase() {
			thWaitForCompletion(); 			
		}	
		//---------- --------- ----------
	    
	    //=== Response (set, get) ===
	    void setResponse(str_SGP2_ML_Message* _response){ 
	    	std::unique_lock<std::mutex> lock(mtxTask);
			ResponseMessage = _response; 
		}
		
	    str_SGP2_ML_Message* getResponse() { 
	    	std::unique_lock<std::mutex> lock(mtxTask);
	    	return ResponseMessage; 
		}
		//---------- --------- ----------
		//=== State (set, get) ===
		void setState(TaskState _state) {
			std::unique_lock<std::mutex> lock(mtxState);
//			Logger::log(Logger::LogLevel::log_DEBUG, "SetState | STATE: %d | NEW STATE: %d | TASK ID:%d",  State, _state, ID);
			State = _state;
		}
	
		void setState(int _id, TaskState _state) { 
			std::unique_lock<std::mutex> lock(mtxState);
			if(_id == ID) {	
//				int _cmd = 0;
//				int _pr  = 0; 
//				if( ResponseMessage){
//					_cmd = ResponseMessage->usCmd;	
//					_pr  = ResponseMessage->ucPar;
//				}
//				Logger::log(Logger::LogLevel::log_DEBUG, "SetState | STATE: %d | NEW STATE: %d | TASK ID:%d",  State, _state, ID);
//				Logger::log(Logger::LogLevel::log_DEBUG, "SetState | STATE: %d | NEW STATE: %d | TASK ID:%d (cmd: 0x%X, pr:%d)",  State, _state, ID, _cmd, _pr);
				State = _state; 
			}			 
		};

		// State (set, get)	
    	TaskState getState() { 
			std::lock_guard<std::mutex> lock(mtxTask);   
			return State; 
		}
		//---------- --------- ----------
		
		// Send Result or State
		bool prepareResponse() { 
			std::unique_lock<std::mutex> lock(mtxTask);
			
			mtxState.lock(); 
			TaskState CurrState = State;
			mtxState.unlock();  
			
			if(CurrState == ts_Ready) {	
				try {	
					if (ucResponse != nullptr) { 
						// Final response --- CMD ----
						ResponseMessage->ucPar = usParam;
						usSGP2_ML_MessagePutData(ResponseMessage, ucResponse, usResponseSize); 
						setState(ts_Success);
						FSendResult = true;
//						Logger::log(Logger::LogLevel::log_DEBUG, "RESPONSE SEND DONE | NEW STATE: SUCCESS | TASK ID:%d", ID);
						return true;	
					} 					
//					printf("\n\n\n !*** CMD - %d, PAR - %d datasize - %d ****\n\n", ResponseMessage->usCmd, ResponseMessage->ucPar, ResponseMessage->usDataSize );	
				} catch(...) {
					Logger::log(Logger::LogLevel::log_ERROR, "RESPONSE SEND DONE | ERROR | TASK ID:%d", ID);
				}
			
			}
	
			if(CurrState == ts_Wait || State == ts_Process) { 
				uint8_t _mass[5] = KEY_DELETE_CMD;
				usSGP2_ML_MessagePutData(ResponseMessage, _mass, sizeof(_mass));
//				Logger::log(Logger::LogLevel::log_DEBUG, "RESPONSE SEND | PROCESS | TASK ID:%d", ID);
				return true;
			} 
			
			if(CurrState == ts_Error) { 
				uint8_t _mass[5] = KEY_DELETE_CMD;
				usSGP2_ML_MessagePutData(ResponseMessage, _mass, sizeof(_mass));
				setState(ID, ts_Success);	
				Logger::log(Logger::LogLevel::log_ERROR, "RESPONSE SEND | ERROR | TASK ID:%d", ID);
				return true;	
			}	
		};
		
		
		void joinThread() { 
			joinmtx.lock(); 
			if (thTask.joinable()) {
				thTask.join();
			}
			joinmtx.unlock();
		}
};

// Task - connect to server
class TaskBaseConnect
{
	protected:
		std::mutex tasksMutex;
		char cUser[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
		char cPass[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
		char cHost[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
		unsigned short usPort;
		unsigned int   uiAddress;
		AbstractCommandProxy* FTaskCMD = nullptr;	
	public:

	// set User	
	void setUser(const std::string& user, const std::string& pass) {
		std::unique_lock<std::mutex> lock(tasksMutex);
        // Очистка массивов перед копированием
        std::memset(cUser, 0, SERVER_CONN_ATTR_TEXT_MAX_SIZE);
        std::memset(cPass, 0, SERVER_CONN_ATTR_TEXT_MAX_SIZE);

        // Копируем строки с учетом их длины, гарантируя, что они будут правильно завершены нулем
        size_t userLength = min(user.size(), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1));
        std::memcpy(cUser, user.c_str(), userLength);
        cUser[userLength] = '\0';  // Завершающий ноль

        size_t passLength = min(pass.size(), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1));
        std::memcpy(cPass, pass.c_str(), passLength);
        cPass[passLength] = '\0';  // Завершающий ноль
	}
	
	// set Socket
    void setSocket(const std::string& host, unsigned short port, unsigned int address) {
        std::unique_lock<std::mutex> lock(tasksMutex);
		std::strncpy(cHost, host.c_str(), SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1);
        usPort = port;
        uiAddress = address;
    }
    
    // set command run
	void setCMD(AbstractCommandProxy* _cmd) {
		if(_cmd == nullptr) {
			Logger::log(Logger::LogLevel::log_ERROR, "CMD is null ");
			return;		 
		}
		std::unique_lock<std::mutex> lock(tasksMutex);
		FTaskCMD = _cmd;	
	}
	
//	// Принимаем сразу указатель на сессию
	void setSession(Session* _session)
	{
        std::lock_guard<std::mutex> lock(tasksMutex);  // Автоматическая блокировка и разблокировка мьютекса
        try {
            if (_session == nullptr) {
                Logger::log(Logger::LogLevel::log_ERROR, "TASK_BASE | SESSION | nullptr");
                return;
            }

            std::string _host = _session->getHost();
            unsigned short _port = _session->getPort();
            unsigned int _id = _session->getAddress();
            std::string _user = _session->getUser();
            std::string _pass = _session->getPass();

            // Проверка на пустой хост
            if (_host.empty()) {
                Logger::log(Logger::LogLevel::log_ERROR, "Host cannot be empty");
                return;
            }

            // Проверка на корректный диапазон порта
            if (_port == 0 || _port > 65535) {
                Logger::log(Logger::LogLevel::log_ERROR, "Port must be in the range 1-65535");
                return;
            }

            // Проверка на пустые имя пользователя и пароль
            if (_user.empty() || _pass.empty()) {
                Logger::log(Logger::LogLevel::log_ERROR, "Username and password cannot be empty");
                return;
            }

            // Логика загрузки сессии
            setSocket(_host, _port, _id);
            setUser(_user, _pass);
        }
        catch (const std::exception& e) {
            // Логирование конкретного типа исключения
//            Logger::log(Logger::LogLevel::log_ERROR, "An error occurred while setting the session: "   string(e.what()));
        }
        catch (...) {
            // Логирование, если произошло не предусмотренное исключение
            Logger::log(Logger::LogLevel::log_ERROR, "An unknown error occurred while setting the session.");
        }
	}


};
	

#endif
