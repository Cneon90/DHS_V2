#include "thServer2.h"

// Загрузка RS реквизитов из файла в session 
Session* SessionRSLoad() 
{
	std::vector<TRelayServer> records = RSHandler::readAllFromFile(RS_SERVERS_FILE_NAME);
	Session* _sesRS = new Session();
	
	if(records.size() == 0) {
		Logger::log(Logger::LogLevel::log_ERROR, "NOT RS RECORDS");
		return nullptr;
	}
	
	TRelayServer _rs;
	_rs =  records.at(0);

	std::string _host    = _rs.getHost();
	unsigned short _port = _rs.getPort();
	unsigned int _id 	 = _rs.getAddress();
	std::string _user 	 = _rs.getUser();
	std::string _pass 	 = _rs.getPass();

//    Logger::log(Logger::LogLevel::log_DEBUG, "Session load: user: %s, pass:%s, host:%s, port:%d", _user.c_str(), _pass.c_str(), _host.c_str(), _port  );

	_sesRS->setSocket(_host, _port, _id);
	_sesRS->setUser(_user, _pass);  
	
	return _sesRS;
}

// Сюда передаем указатель на задачу (RS) и сессию RS, 
// Тут получаем и заполняем реквезитами 
// Получаем из файла и кэшуруем, если уже есть, то отдаем, то что в памяти
bool Fill_Task(TaskBaseConnect* _task, Session* _session) 
{
	if(_session == nullptr) {

		return false;	
	}
	
	try {
		std::string    _host = _session->getHost();
		unsigned short _port = _session->getPort();
		unsigned int     _id = _session->getAddress();
		std::string    _user = _session->getUser();
		std::string    _pass = _session->getPass();			

		_task->setSocket(_host, _port, _id);
		_task->setUser(_user, _pass); 	
	} catch(...) { 
		return false;
	}

	
	return true; 	
}

//******************************************************************************//
void vServerClientRequestMessage (void* pvCB_Arg, char* pcUserName, const str_SGP2_ML_Message* pxSrcRequestMessage, str_SGP2_ML_Message* pxDstResponseMessage)
{
	pcSGP_ClientStack* _clStack =  reinterpret_cast<pcSGP_ClientStack*> (pvCB_Arg);
	
	vsServer*  _server    = _clStack->xServer; 
	TasksList* _tasksList = _server->getTasksList();
	int _clSocket         = _clStack->iSocket; 
	Session* _sessionDS   = _clStack->xSessionDS;
	DialogAPI* QuestAPI   = _clStack->QuestAPI;

	// Request rights
	if(pxSrcRequestMessage->usCmd == CMD_REQRIG) {
		if (pxSrcRequestMessage->ucPar == 0x00) {
			std::unique_lock<std::mutex> lock(_server->xMessageMutex); 	
			Accounts* Acc     = _server->getAccount();
			account_item user = Acc->FindAccountByLogin(pcUserName);
			
			char right = user.pcRights[0];
			usSGP2_ML_MessagePutByte(pxDstResponseMessage, right);	
		}		
	}
	
	
	// Ping
	if (pxSrcRequestMessage->usCmd == 0x0000) {usSGP2_ML_MessagePutData(pxDstResponseMessage, pxSrcRequestMessage->pcData, pxSrcRequestMessage->usDataSize);}

	//Start session
	if(pxSrcRequestMessage->usCmd == CMD_SES) {
		try{
			if ((pxSrcRequestMessage->ucPar == 0x00) && (pxSrcRequestMessage->usDataSize > 0)) {
				std::unique_lock<std::mutex> lock(_server->xMessageMutex);
                Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | START ENTER  |  " );
				//Get param session 
				char* User = reinterpret_cast<char*>(pxSrcRequestMessage->pcData);
                size_t length = strlen(User);  // Находим длину строки
                User[length] = '\0';           // Добавляем нулевой символ в конец строки

				unsigned char* nextData = pxSrcRequestMessage->pcData + std::strlen(User) + 1;
				char* Password = reinterpret_cast<char*>(nextData);
                size_t lengthP = strlen(Password);  // Находим длину строки
                Password[lengthP] = '\0';           // Добавляем нулевой символ в конец строки
				nextData = nextData + std::strlen(Password) + 1;
				char* Host = reinterpret_cast<char*>(nextData);
                size_t lengthH = strlen(Host);  // Находим длину строки
                Host[lengthH] = '\0';           // Добавляем нулевой символ в конец строки
				// Port
				nextData = nextData + std::strlen(Host) + 1;
				unsigned short Port = *reinterpret_cast<unsigned short*>(nextData);
				// id
				nextData += sizeof(unsigned short);
				unsigned int id = *reinterpret_cast<unsigned int*>(nextData);
				if (std::strlen(User) == 0) {
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE1);
				    return;
				}
				if (std::strlen(Password) == 0) {
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE2);
				    return;
				}
				if (std::strlen(Host) == 0) {
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE3);
				    return;
				}
				if (Port == 0) {
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE4);
				    return;
				}
                Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | SET SETTING |  " );
				// Save attr Session
				_sessionDS->setSocket(Host, Port, id);
				_sessionDS->setUser(User, Password);
				//=================================================================== 
				
				//=== Session load === 
					std::string    _host = _sessionDS->getHost();
					unsigned short _port = _sessionDS->getPort();
					unsigned int     _id = _sessionDS->getAddress();
					std::string    _user = _sessionDS->getUser();
					std::string    _pass = _sessionDS->getPass();
				//=====================
				
				//=================================================================== 
			
				// Получаем указатель 
				TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
				TaskConnect* _taskDS = nullptr;	
			
				if(taskBasePtr == nullptr) { // Если не получили экземпляр, создаем свой
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instan / CMD_SES");
					_taskDS = new TaskConnect();
				} else { 
					_taskDS = static_cast<TaskConnect*>(taskBasePtr);
				}	
				// Init
				if(_taskDS->getID() == 0) {
					_taskDS->setID(_clSocket);
					_taskDS->setRequestMessage(*pxSrcRequestMessage);
					_taskDS->setResponseMessage(pxDstResponseMessage);
					_taskDS->setIndex(_clSocket); 
					_taskDS->setSocket(_host, _port, _id);
					_taskDS->setUser(_user, _pass);  
					cmdPingProxy* _cmdPing = _clStack->xCmdPing; //new cmdPingProxy();
//					Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | START | ");
					 _clStack->cxManagerData->threadInfo->thStart();
					//SESSION INIT
					//
//					cmdTaskTerminalListDS* _cmdListDS = _clStack->xCmdTaskDSList;   // Создаём команду получения списка терминалов с DS сервера
//					_cmdListDS->setManager(_clStack->cxManagerData);				// Передаем команде указатель на Менеджера (у каждого клиента он свой)
					//-----------------------
					_taskDS->setCMD(_cmdPing);	
				}	
				
				_tasksList->AddToStack(_clSocket, _taskDS);	
			}
			
			// Stop Session
			if (pxSrcRequestMessage->ucPar == 0x02) { 
//				Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminal LIST | GET0 ");					
				Terminal_Info_List* termList = _clStack->cxManagerData->GetTerminalInfoList();
//				Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminal LIST | GET1 ");
				if(termList) {
					Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminal LIST %d |  ", termList->Count() );
					 _clStack->cxManagerData->threadInfo->thStop();
					 _clStack->cxManagerData->threadInfo->thWaitForCompletion();
					 Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminated |  ", termList->Count() );
					 vPortSleepMs(200);
					 termList->Clear();	
//					 _clStack->cxManagerData->threadInfo->thStart();
					Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminal LIST %d |  ", termList->Count() );
				} else {
					Logger::log(Logger::LogLevel::log_DEBUG, "SESSION | Terminal LIST | NO-CLEAR ");
				}
			}
			
			
		} catch(...) { 
			Logger::log(Logger::LogLevel::log_ERROR, "CMD_SES");	
		}
	}
	
	// Получение списка терминалов с DS сервера
	if(pxSrcRequestMessage->usCmd == CMD_TERM_LIST_DS) {
		if ((pxSrcRequestMessage->ucPar == 0x00) && (pxSrcRequestMessage->usDataSize > 0)) {
			std::unique_lock<std::mutex> lock(_server->xMessageMutex);
            Logger::log(Logger::LogLevel::log_DEBUG, "CMD_TERM_LIST_DS | START ENTER  |  " );
			try {
				// Получаем указатель 
				TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
				TaskConnect* _taskDS = nullptr;	
				// Преобразуем указатель в тип TaskConnect		
				if(taskBasePtr == nullptr) { // Если не получили экземпляр, создаем свой
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instans / CMD_TERMS");
					_taskDS = new TaskConnect();
				} else { 
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | Existing Instans / CMD_TERMS");
					_taskDS = dynamic_cast<TaskConnect*>(taskBasePtr);// dynamic_cast<TaskConnect*>(taskBasePtr);
				}	
				
				if (! _taskDS) { // Если это не TaskConnect
					Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERMS Could not convert object of type TaskaBase to TaskConnect");
				}	
				
				//Init
				if(_taskDS->getID() == 0) {
                    Logger::log(Logger::LogLevel::log_DEBUG, "CMD_TERM_LIST_DS | INIT | socket %d ", _clSocket );
                    _taskDS->setID(_clSocket);
					_taskDS->setRequestMessage(*pxSrcRequestMessage);
					_taskDS->setResponseMessage(pxDstResponseMessage);
				
					if(_taskDS == nullptr) { 
						Logger::log(Logger::LogLevel::log_ERROR, "_taskDS is null");
						return;
					}
                    _taskDS->setSession(_sessionDS);
                    _taskDS->setIndex(_clSocket);



                    if(_sessionDS) {
                        Logger::log(Logger::LogLevel::log_DEBUG, "CMD_TERM_LIST_DS | SET SESSION |");
//                        _taskDS->setSession(_sessionDS);
                        _sessionDS->printProperties();
                    } else {
                        Logger::log(Logger::LogLevel::log_DEBUG, "CMD_TERM_LIST_DS | _sessionDS | nullptr  ");
					}

					cmdTaskTerminalListDS* _cmdListDS = _clStack->xCmdTaskDSList;   // Создаём команду получения списка терминалов с DS сервера
//					_cmdListDS->setManager(_clStack->cxManagerData);				// Передаем команде указатель на Менеджера (у каждого клиента он свой)
					_cmdListDS->setTerminalList(_clStack->cxManagerData->GetTerminalInfoList());
					_taskDS->setCMD(_cmdListDS);	

//					cmdTask* _cmdTask = _clStack->xCmdTask;
//					_taskDS->setCMD(_cmdTask);	
				}
				
				_tasksList->AddToStack(_clSocket, _taskDS);

			} catch(...) {
				Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERMS");	
			}	    		
		}		
	}
	

	// Список терминалов RS сервера
	if(pxSrcRequestMessage->usCmd == CMD_TERM_LIST_RS) {
		
		if ((pxSrcRequestMessage->ucPar == 0x01) && (pxSrcRequestMessage->usDataSize > 0)) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
    		try {
				// Получаем указатель 
				TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
				TaskConnect* _taskRS = nullptr;	
				
				if(taskBasePtr == nullptr) { // ???? ?? ???????? ?????????, ??????? ????
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instans / CMD_TERM_LIST");
					_taskRS = new TaskConnect();
				} else { 
					_taskRS = static_cast<TaskConnect*>(taskBasePtr);
				}
				
				if (! _taskRS) { // Если это не TaskConnect
					Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERM_LIST 0x01 Could not convert object of type TaskaBase to TaskConnect");
				}

				if(_taskRS->getID() == 0) {

					_taskRS->setID(_clSocket);
					_taskRS->setRequestMessage(*pxSrcRequestMessage);
					_taskRS->setResponseMessage(pxDstResponseMessage);
		
					if(_clStack->xSessionRS == nullptr) {
                        Logger::log(Logger::LogLevel::log_DEBUG, " TERMINAL LIST 0x0 | SESSION EMPTY | LOAD SESSION  ");
						_clStack->xSessionRS = SessionRSLoad();
                        _taskRS->setSession(_clStack->xSessionRS);
//						_clStack->cxManagerData->threadInfo->setCurrentSessionRS(_clStack->xSessionRS);
					}
				   //Заполняем задачу, реквезитами RS сервера
//				   if(! Fill_Task(_taskRS, _clStack->xSessionRS)) {
//                        Logger::log(Logger::LogLevel::log_ERROR, " CMD_TERM_LIST_RS | FILL_Task | 0xE1");
//				   		usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE1);
//						return;
//				   }
					
					cmdTask* _cmdTask = _clStack->xCmdTask;//new cmdTask();
					_taskRS->setCMD(_cmdTask);	
				}
				
				_tasksList->AddToStack(_clSocket, _taskRS);	
			} catch (...) { 
				Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERM_LIST 0x01");
			}
		}		
		
		if (pxSrcRequestMessage->ucPar == 0x30) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
			try{
				// Получаем указатель 
				TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
				// Преобразуем указатель в тип TaskConnect
				TaskConnect* _taskRS = dynamic_cast<TaskConnect*>(taskBasePtr); //dynamic_cast<TaskConnect*>(taskBasePtr);
	
				if(taskBasePtr == nullptr) { // ???? ?? ???????? ?????????, ??????? ????
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instans / CMD_TERM_LIST");
					_taskRS = new TaskConnect();
				} else { 
					_taskRS = static_cast<TaskConnect*>(taskBasePtr);
				}
				
				if (! _taskRS) { // Если это не TaskConnect
					Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERM_LIST 0x30 Could not convert object of type TaskaBase to TaskConnect");
				}	
				
				if(_taskRS->getID() == 0) {

					_taskRS->setID(_clSocket);
					_taskRS->setRequestMessage(*pxSrcRequestMessage);
					_taskRS->setResponseMessage(pxDstResponseMessage);
					
					if(_clStack->xSessionRS == nullptr) {
                        Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST  0x30 | SESSION EMPTY | LOAD SESSION  ");

                        _clStack->xSessionRS = SessionRSLoad();
//                        _clStack->cxManagerData->threadInfo->setCurrentSessionRS(_clStack->xSessionRS);
                        _taskRS->setSession(_clStack->xSessionRS);
					}
					
					//Заполняем задачу, реквезитами RS сервера
					if(! Fill_Task(_taskRS, _clStack->xSessionRS)) {
                        Logger::log(Logger::LogLevel::log_ERROR, " CMD_TERM_LIST_RS | FILL_Task 0x30 | 0xE1");
					    usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0xE1);
					    return;
					} 
					
					cmdTask* _cmdTask = _clStack->xCmdTask;//new cmdTask();
					_taskRS->setCMD(_cmdTask);
				}
				
				_tasksList->AddToStack(_clSocket, _taskRS);	
			} catch(...) {
				Logger::log(Logger::LogLevel::log_ERROR, "CMD_TERM_LIST 0x30");	
			}
		}			
	}
	
	//RS -- CRUD 
	if(pxSrcRequestMessage->usCmd == CMD_RS ) {
		// ALL record - list
		if (pxSrcRequestMessage->ucPar == 0x00) {
			try {
				if ( pxSrcRequestMessage->usDataSize < 4) {	return;	}
				std::vector<TRelayServer> records = RSHandler::readAllFromFile(RS_SERVERS_FILE_NAME);
	
				usSGP2_ML_MessagePutInt   (pxDstResponseMessage, time(NULL)); // current UnixTime 
				unsigned short _startID = uiSGP2_GetIntFromArray(pxSrcRequestMessage->pcData);
				unsigned short _cnt 	= uiSGP2_GetIntFromArray(pxSrcRequestMessage->pcData+2);
			    int RecordsCount 		= records.size();
				int SendCount           = 0;
				
				if(_startID < 0) { return; }
				if(_cnt     < 0) { return; } 
			    // send actual count of item
			    if ( (RecordsCount - _startID) < _cnt ) {
					SendCount = RecordsCount - _startID;
				} else { SendCount = _cnt; }
			    
				usSGP2_ML_MessagePutShort(pxDstResponseMessage, SendCount);	
			    int t = (_startID + SendCount);

				for(int i=_startID; i < t; i++ ) {
					if(i >= RecordsCount ) { 
						Logger::log(Logger::LogLevel::log_ERROR, "Records count");
						break;
					}
					TRelayServer item = records.at(i);

					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) item.getUser()); 
					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) item.getPass());
					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) item.getHost());
					usSGP2_ML_MessagePutShort(pxDstResponseMessage, item.getPort());
					usSGP2_ML_MessagePutInt(pxDstResponseMessage, item.getAddress());
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, item.getIsActive());
					usSGP2_ML_MessagePutInt(pxDstResponseMessage, i);
				} 
			} catch(...) { 
				Logger::log(Logger::LogLevel::log_ERROR, "CMD_RS 0x00");	
			}
			
		}
		
		// Add record RS 
		if (pxSrcRequestMessage->ucPar == 0x01) {
			if ( pxSrcRequestMessage->usDataSize < 4) {	return;	}
			try {
				
				int skip = 0;
				char cUser[ATTR_MAX_SIZE];
				char cPass[ATTR_MAX_SIZE];
				char cHost[ATTR_MAX_SIZE];
				unsigned short usPort;
				unsigned int uiAddress;
				bool isActive;
				unsigned char*     arr = pxSrcRequestMessage->pcData;
				
				// Чтение пользователя
				strncpy(cUser, (char*) arr, ATTR_MAX_SIZE - 1);
				cUser[ATTR_MAX_SIZE - 1] = '\0'; // Обеспечиваем завершение строки
				int userL = strlen(cUser);
				skip += userL + 1;
				
				// Чтение пароля
				strncpy(cPass, (char*)arr + skip, ATTR_MAX_SIZE - 1);
				cPass[ATTR_MAX_SIZE - 1] = '\0';
				skip += strlen(cPass) + 1;
				
				// Чтение хоста
				strncpy(cHost, (char*)arr + skip, ATTR_MAX_SIZE - 1);
				cHost[ATTR_MAX_SIZE - 1] = '\0';
				skip += strlen(cHost) + 1;
				
				// Чтение порта
				usPort = usSGP2_GetShortFromArray(arr + skip);
				skip += sizeof(short);
				
				// Чтение адреса
				uiAddress = uiSGP2_GetIntFromArray(arr + skip);
				skip += sizeof(int);
				TRelayServer* RS = new TRelayServer();
				RS->initialize(0, cUser, cPass, cHost, usPort, uiAddress, true);
				
				RSHandler::addRecord(RS_SERVERS_FILE_NAME, *RS);
				
//				usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0x20);
				Logger::log(Logger::LogLevel::log_INFO, "Add record rs - %s ", cHost);
				
				RSHandler::readAllFromFile(RS_SERVERS_FILE_NAME);
				
				_clStack->xSessionRS = SessionRSLoad();	
				usSGP2_ML_MessagePutByte(pxDstResponseMessage, 1);
			} catch (...) { 
				Logger::log(Logger::LogLevel::log_ERROR, "ADD RECORD 0x01");
				usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0);	
			}
		}
		
		
		if (pxSrcRequestMessage->ucPar == 0x02) {
			if (pxSrcRequestMessage->usDataSize < 1) {	return;	}
			
		}
		
		// Delete  all
		if (pxSrcRequestMessage->ucPar == 0x03) {
//			if (pxSrcRequestMessage->usDataSize < 1) {	return;	}
			try{
				RSHandler::deleteFile(RS_SERVERS_FILE_NAME);				
				Logger::log(Logger::LogLevel::log_INFO, "File RS records delete ");	
				usSGP2_ML_MessagePutByte(pxDstResponseMessage, 1);
			} catch(...) {
				Logger::log(Logger::LogLevel::log_ERROR, "Delete file RS 0x03");
				usSGP2_ML_MessagePutByte(pxDstResponseMessage, 0);	
			}

		}
	} 
	
	
	// GET FILE Card Table 
	if(pxSrcRequestMessage->usCmd == CMD_DIR) {
//		if ((pxSrcRequestMessage->ucPar == 0x01) && (pxSrcRequestMessage->usDataSize > 0)) {
			std::unique_lock<std::mutex> lock(_server->xMessageMutex); 
				try {
				// Получаем указатель 
				TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
			
				TaskConnect* _taskDS = nullptr;	
				
				if(taskBasePtr == nullptr) { // Если не получили экземпляр, создаем свой
//					Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instans / CMD_DIR");
					_taskDS = new TaskConnect();
				} else { 
					_taskDS = static_cast<TaskConnect*>(taskBasePtr);
				}	

				if (! _taskDS) { // Если это не TaskConnect
					Logger::log(Logger::LogLevel::log_ERROR, "CMD_DIR Could not convert object of type TaskaBase to TaskConnect");
				}	
				
				if(_taskDS->getID() == 0) {		
					_taskDS->setID(_clSocket);
					_taskDS->setRequestMessage(*pxSrcRequestMessage);
					_taskDS->setResponseMessage(pxDstResponseMessage);
					
					if(_taskDS == nullptr) { return; } 
				
					std::string _host    = _sessionDS->getHost();
					unsigned short _port = _sessionDS->getPort();
					unsigned int _id  = _sessionDS->getAddress();
					std::string _user = _sessionDS->getUser();
					std::string _pass = _sessionDS->getPass();
			
					_taskDS->setSocket(_host, _port, _id);
					_taskDS->setUser(_user, _pass);  
					
					cmdTask* _cmdTask = _clStack->xCmdTask;//new cmdTask();
					if(_cmdTask == nullptr) { return; } 
					_taskDS->setCMD(_cmdTask);	
				}
				
				_tasksList->AddToStack(_clSocket, _taskDS);	
				} catch (...) {
					Logger::log(Logger::LogLevel::log_ERROR, "CMD_DIR");	
				}
	
	}
	
	
	//Admin
	if(pxSrcRequestMessage->usCmd == CMD_ADMIN) {
		// Send all task (soket, state)
		if (pxSrcRequestMessage->ucPar == 0x00) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, time(NULL));
		
		    int Taskcount = _tasksList->getCount();
			for (int i = 0; i < Taskcount; ++i)	{
				TaskaBase* task =  _tasksList->getTaskID(i); 
				usSGP2_ML_MessagePutInt(pxDstResponseMessage,   task->getID());
				usSGP2_ML_MessagePutShort(pxDstResponseMessage, task->getState());	
			}			
		}	
		
		// Send statistic task
		if (pxSrcRequestMessage->ucPar == 0x01) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
			
			int TasksWait    = _tasksList->TaskSum(ts_Wait);
			int TasksProcess = _tasksList->TaskSum(ts_Process);
			int TasksReady   = _tasksList->TaskSum(ts_Ready);
			int TasksSuccess = _tasksList->TaskSum(ts_Success);
			int TasksError   = _tasksList->TaskSum(ts_Error);
			int TasksFree    = _tasksList->TaskSum(ts_free);
			
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, time(NULL));
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksWait);
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksProcess);
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksReady);
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksSuccess);
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksError); 
			usSGP2_ML_MessagePutShort (pxDstResponseMessage, TasksFree); 
		}
		
		
		// Send statistic Server
		if (pxSrcRequestMessage->ucPar == 0x02) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
		  	MonitoringBase* xMonitoring = _clStack->xMonitoring;	
			
			int CPULoad  = xMonitoring->CPULoad(); 
			int RAMFree  = xMonitoring->RAMFree()  / (1024 * 1024);
			int RAMUsed  = xMonitoring->RAMUsed()  / (1024 * 1024);
			int RAMTotal = xMonitoring->RAMTotal() / (1024 * 1024);
			
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, time(NULL));
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, CPULoad);	
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, RAMTotal);
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, RAMFree);
			usSGP2_ML_MessagePutInt   (pxDstResponseMessage, RAMUsed);		
		}			
	}
	
	
	// SYNC TERMINAL
	if(pxSrcRequestMessage->usCmd == CMD_SYNC) {
		if (pxSrcRequestMessage->ucPar == 0x30) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
			//=================================================================== 
			TaskaBase* taskBasePtr = _tasksList->getTaskInstance(_clSocket, pxSrcRequestMessage);
			TaskConnect* _taskDS = nullptr;	
			
			if(taskBasePtr == nullptr) { 
//				Logger::log(Logger::LogLevel::log_DEBUG, "GET | nullptr | new Instans / CMD_SYNC");
				_taskDS = new TaskConnect();
			} else { 
				_taskDS = static_cast<TaskConnect*>(taskBasePtr);
			}	
			// Init
			if(_taskDS->getID() == 0) {
				_taskDS->setID(_clSocket);
				_taskDS->setRequestMessage(*pxSrcRequestMessage);
				_taskDS->setResponseMessage(pxDstResponseMessage);
				_taskDS->setIndex(_clSocket); 
				_taskDS->setSession(_sessionDS); 
				try {
					cmdTask* _cmdTask = _clStack->xCmdTask;
					_taskDS->setCMD(_cmdTask);		
				} catch(...) { 
					Logger::log(Logger::LogLevel::log_ERROR, "CREATE | cmdTask |  CMD_SYNC");	
				}
			}	
			
			_tasksList->AddToStack(_clSocket, _taskDS);	
		}
	}
	
	// Информация о терминале, скэшированная на DHS 
	if(pxSrcRequestMessage->usCmd == CMD_TERM_LIST_DHS) {
		if (pxSrcRequestMessage->ucPar == 0x00) {
			std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
			
			if(_clStack->xSessionRS == nullptr) {
				Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | LOAD SESSION  ");
				_clStack->xSessionRS = SessionRSLoad();	
				_clStack->cxManagerData->threadInfo->setCurrentSessionRS(_clStack->xSessionRS);
			}
			
			unsigned int StartIndex = usSGP2_GetShortFromArray(pxSrcRequestMessage->pcData);
			unsigned int ReqCount 	= usSGP2_GetShortFromArray(pxSrcRequestMessage->pcData+4);
			
			if (ReqCount <= 0) {
			    Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | ReqCount <= 0 | Return ");
			    usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
			    return;
			}
			
			Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | Start index = %d | count = %d ", StartIndex, ReqCount);
			
			Terminal_Info_List* termList = _clStack->cxManagerData->GetTerminalInfoList(); // TERMINAL LIST
			int listCount = termList->Count();

			if(listCount == 0) { 
				Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | listCount = 0 | Return ");
				usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
				return;
			}
			
			// Если вышли за пределы списка, отправляем 0
			if (StartIndex >= listCount) {
			    Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | StartIndex >= listCount | Return ");
			    usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
			    return;
			}
			
			// Вычисляем фактическое количество терминалов
			int factCount = min(ReqCount, listCount - StartIndex);
			Logger::log(Logger::LogLevel::log_DEBUG, "TERMINAL LIST | factCount = %d |", factCount);
		 
		  	try{
			  
			  	for(int i = StartIndex; i < StartIndex + factCount; i++) { 
			  		Terminal_Info_Item terminal_item = termList->getItem(i);
					usSGP2_ML_MessagePutData(pxDstResponseMessage, terminal_item.rs_info,  terminal_item.rs_infoLen /*TERMINAL_RS_INFO_LENGTH */); 	
				}
				
			} catch(...){ 
					Logger::log(Logger::LogLevel::log_ERROR, "TERMINAL LIST | ERROR GET |");	
			}
		}
	}
	
	
	// Server Info ID[4]TYPE[4]VERSION[2]RELEASE_DATE[3]NAME[]
	if(pxSrcRequestMessage->usCmd == CMD_INFO) {
		std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
		if ((pxSrcRequestMessage->ucPar == 0x01) && (pxSrcRequestMessage->usDataSize > 0)) {
			// Set Server Name
			int i = 0;
			char bufName[CONFIG_SERVER_NAME_MAX_SIZE];
			for (i = 0; i < pxSrcRequestMessage->usDataSize; i ++) {
				if (i >= CONFIG_SERVER_NAME_MAX_SIZE) break; 
				bufName[i] = pxSrcRequestMessage->pcData[i];
			};

			bufName[i] = 0;
			_server->setName(bufName);
				// todo save config
		};	
		
		usSGP2_ML_MessagePutInt(pxDstResponseMessage,  MY_SERVER_ID			);
		usSGP2_ML_MessagePutInt(pxDstResponseMessage,  MY_SERVER_TYPE		);
		usSGP2_ML_MessagePutByte(pxDstResponseMessage, _server->getVerH()	);
		usSGP2_ML_MessagePutByte(pxDstResponseMessage, _server->getVerL()	);
		usSGP2_ML_MessagePutByte(pxDstResponseMessage, _server->getDateD()	);
		usSGP2_ML_MessagePutByte(pxDstResponseMessage, _server->getDateM()	);
		usSGP2_ML_MessagePutByte(pxDstResponseMessage, _server->getDateY()	);

		
		std::string  StrName = _server->getName(); 
		char* mutableCharStr = new char[StrName.length() + 1];
		std::strcpy(mutableCharStr, StrName.c_str());
		
		if(mutableCharStr != nullptr)
		usSGP2_ML_MessagePutString(pxDstResponseMessage, mutableCharStr);
	}
	
	
	
	/* Dialogs*/
//	if(pxSrcRequestMessage->usCmd == CMD_DIALOG) {
//		std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
//		if ((pxSrcRequestMessage->ucPar == 0x00) && (pxSrcRequestMessage->usDataSize > 0)) {
//			//Dialogs list
//			unsigned int StartIndex = usSGP2_GetShortFromArray(pxSrcRequestMessage->pcData);
//			unsigned int ReqCount 	= usSGP2_GetShortFromArray(pxSrcRequestMessage->pcData+4);
//			
//			if (ReqCount <= 0) {
//			    Logger::log(Logger::LogLevel::log_DEBUG, "Quest LIST | ReqCount <= 0 | Return ");
//			    usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
//			    return;
//			}
//			
//			Logger::log(Logger::LogLevel::log_DEBUG, "Quest LIST | Start index = %d | count = %d ", StartIndex, ReqCount);
//			
//			TQuestList* questList = _clStack->xServer->getQuestList();
//			int  questListCount = questList->Count();
//
//			if(questListCount == 0) { 
//				Logger::log(Logger::LogLevel::log_DEBUG, "Quest LIST | QuestListCount = 0 | Return ");
//				usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
//				return;
//			}
//			
//			// Если вышли за пределы списка, отправляем 0
//			if (StartIndex >= questListCount) {
//			    Logger::log(Logger::LogLevel::log_DEBUG, "Quest LIST | StartIndex >= listCount | Return ");
//			    usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0);
//			    return;
//			}
//			
//			// Вычисляем фактическое количество терминалов
//			int factCount = std::min(ReqCount, questListCount - StartIndex);
//			Logger::log(Logger::LogLevel::log_DEBUG, "Quest LIST | factCount = %d |", factCount);
//		 
//		  	try{
//			  
//			  	for(int i = StartIndex; i < StartIndex + factCount; i++) { 
//			  		QeustItem Quest_item = questList->getItem(i);
//					char c_filename[32];
//					// Заполняем массив нулями
//    				std::memset(c_filename, 0, sizeof(c_filename));
//    				// Копируем содержимое filename в c_filename, ограничив копирование 32 байтами
//    				std::strncpy(c_filename, Quest_item.fileName.c_str(), sizeof(c_filename) - 1);
//					usSGP2_ML_MessagePutData(pxDstResponseMessage, c_filename,  32); 	
//				}
//				
//			} catch(...){ 
//					Logger::log(Logger::LogLevel::log_ERROR, "Quest LIST | ERROR GET | ");	
//			}		
//		}		
//	}

	
	if(pxSrcRequestMessage->usCmd == CMD_DIALOG) {
		std::lock_guard<std::mutex> lock(_server->xMessageMutex); 
		if ((pxSrcRequestMessage->ucPar == 0x00) && (pxSrcRequestMessage->usDataSize > 0)) {
			// Путь до файла с вопросами
			std::string QuestFilePath = _clStack->xServer->getQuestPath();
			
//			if(QuestFilePath == "") { 
//				Logger::log(Logger::LogLevel::log_ERROR, "Quest NOT FILE  | ERROR | Return ");
//				return;
//			}
//			
//			if (! fileExists(QuestFilePath)) {
//				Logger::log(Logger::LogLevel::log_ERROR, "Quest NOT EXIST FILE  | ERROR | Return ");
//				return;
//			}				
			
			bool isOpenFile = QuestAPI->init(QuestFilePath);
			if(! isOpenFile) { 
				Logger::log(Logger::LogLevel::log_ERROR, "Error open quest FILE %s ", QuestFilePath.c_str());
				usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0xE1);
				return;	
			}
			
			unsigned int RequestID = usSGP2_GetShortFromArray(pxSrcRequestMessage->pcData);
			unsigned int DialogID = 0;

			if(RequestID == 0) {
				DialogID = QuestAPI->getQuestID_First();	// Init Dialog (search first diaolog)
			} else { 
				DialogID = RequestID;
			} 			

			// Узнаем тип запрашиваемого item 
			strSgDialogItem DialogItem = QuestAPI->getDialogItemByID(DialogID);			
			
			switch (DialogItem.eType) {
				case SDIT_DIALOG: {
					Logger::log(Logger::LogLevel::log_DEBUG, "QUEST");	
					TDialogDTO _Dialog = QuestAPI->getDialogDTO(DialogItem);
					/*  BUILDER RESPONSE
						Type[1]QuestLen[4]QuestText[QuestLen]AnswerCount[byte] 
					    for (AnswerCount) {
					 		AnswerLen[4]      AnswerText[AnswerLen]       NextQuestID[4]
						}  
					*/  
					/* TYPE */
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, SDIT_DIALOG );	
					/* QUEST TEXT */
					usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.QuestText.length());
					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) _Dialog.QuestText.c_str());				 
					/* ANSWERS*/
					uint8_t AnswersCount = _Dialog.AnswerCount;
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, AnswersCount);
					for(int i=0; i < AnswersCount; i++) { 
						usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.AnswerText[i].length());
						usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) _Dialog.AnswerText[i].c_str());			
						usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.NextActID[i]);
					}
					
					break;	
				}
				
				case SDIT_SOLUTION: {
						Logger::log(Logger::LogLevel::log_DEBUG, "SOLUTION");	
					/* 	BUILDER RESPONSE
						Type[1]SolutionLen[4]SolutionText[QuestLen]			
					*/
					
					/* TYPE */
					usSGP2_ML_MessagePutByte(pxDstResponseMessage, SDIT_SOLUTION );
					/* Solution text */
					std::string SolutionText = QuestAPI->getSolutionText(DialogItem.uiID);
					usSGP2_ML_MessagePutInt(pxDstResponseMessage, SolutionText.length());
					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) SolutionText.c_str());
					
					break;	
				}
				
				default: {
					Logger::log(Logger::LogLevel::log_ERROR, "ERROR | Dialog not found");
					usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0xEE);
					break;
				}			
			}
			
			//Если item - диалог (Вопрос с вариантами ответа)
//			if(DialogItem.eType == SDIT_DIALOG) {
//				Logger::log(Logger::LogLevel::log_DEBUG, "QUEST");	
//				TDialogDTO _Dialog = QuestAPI->getDialogDTO(DialogItem);
//				/* 
//				    BUILDER RESPONSE
//					Type[1]QuestLen[4]QuestText[QuestLen]AnswerCount[byte] 
//				    for (AnswerCount) {
//				 		AnswerLen[4]      AnswerText[AnswerLen]       NextQuestID[4]
//					}  
// 				*/  
//				/* TYPE */
//				usSGP2_ML_MessagePutByte(pxDstResponseMessage, SDIT_DIALOG );	
//				/* QUEST TEXT */
//				usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.QuestText.length());
//				usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) _Dialog.QuestText.c_str());				 
//				/* ANSWERS*/
//				uint8_t AnswersCount = _Dialog.AnswerCount;
//				usSGP2_ML_MessagePutByte(pxDstResponseMessage, AnswersCount);
//				for(int i=0; i < AnswersCount; i++) { 
//					usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.AnswerText[i].length());
//					usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) _Dialog.AnswerText[i].c_str());			
//					usSGP2_ML_MessagePutInt(pxDstResponseMessage, _Dialog.NextActID[i]);
//				}
//			}
//			
//			// Если item - решение 
//			if(DialogItem.eType == SDIT_SOLUTION) {
//				Logger::log(Logger::LogLevel::log_DEBUG, "SOLUTION");	
//				/* 
//					BUILDER RESPONSE
//					Type[1]SolutionLen[4]SolutionText[QuestLen]			
//				*/
//				
//				/* TYPE */
//				usSGP2_ML_MessagePutByte(pxDstResponseMessage, SDIT_SOLUTION );
//				/* Solution text */
//				std::string SolutionText = QuestAPI->getSolutionText(DialogItem.uiID);
//				usSGP2_ML_MessagePutInt(pxDstResponseMessage, SolutionText.length());
//				usSGP2_ML_MessagePutString(pxDstResponseMessage, (char*) SolutionText.c_str());				
//			}
			
		
			
					
		}
	
	}
	
	
	
	

}

//******************************************************************************//
unsigned char* ucServerGetClientUserPasword       (void* pvCB_Arg, const char* pcUserName)
{
	pcSGP_ClientStack* _clStack =  reinterpret_cast<pcSGP_ClientStack*> (pvCB_Arg);
	vsServer* _server = _clStack->xServer; 
	std::lock_guard<std::mutex> lock(_server->PasswordMutex); 
	
	if (_server == nullptr) {
		return (unsigned char *)"";
	}
	
	Accounts* Acc = _server->getAccount();
	account_item user = Acc->FindAccountByLogin(pcUserName);

	return (unsigned char *)user.pcPassword;
};
//******************************************************************************//



//******************************************************************************//
unsigned int uiGetRandom (unsigned int uiInitValue)
{
//	Random handle:
	static unsigned int uiNextRand = 1;
	if (uiInitValue) uiNextRand = uiInitValue;
	uiNextRand = uiNextRand * 1103515245 + 12345;
	return uiNextRand;
};


void vGetRandomBytesSequence        (unsigned char* pcDst, unsigned short usDstSize)
{
	while (usDstSize) pcDst[-- usDstSize] = uiGetRandom(0);
};
//******************************************************************************//


str_SGP2_ML_ServerCB xServer  =
{
    &vServerClientRequestMessage,
    &ucServerGetClientUserPasword,
    &vGetRandomBytesSequence
};
//******************************************************************************//


//Constructor
vsServer::vsServer(int port) : port(port), running(false) {
	FSessionDS = new Session;
}

// Destructor
vsServer::~vsServer() {
	delete FSessionDS;
    stop();
//    close(serverSocket);
}

void vsServer::StartListening() 
{
	if (iPortSetStackSize(REQUIRED_MIN_STACK_SIZE) != 0)                 if (!isDeamonMode) {
		Logger::log(Logger::LogLevel::log_ERROR, "WARNING: Unable to set min stack size 0x%X", REQUIRED_MIN_STACK_SIZE);
	}
	if (iPortSetThreadsLimit(REQUIRED_THREADS_COUNT, isDeamonMode) != 0) if (!isDeamonMode) {
		Logger::log(Logger::LogLevel::log_ERROR, "WARNING: Unable to threads count limit 0x%X", REQUIRED_THREADS_COUNT);	
	}
	
	// Init TCP Server:
	if (iPortInitTCP_Server() != 0) {
		Logger::log(Logger::LogLevel::log_ERROR, "Can't run TCP server");
		return;
	}

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        Logger::log(Logger::LogLevel::log_ERROR, "Can't create socket server");
        exit(EXIT_FAILURE);
    }	
    
    start();
}



void vsServer::start() {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

	Logger::log(Logger::LogLevel::log_INFO, "1. Bind");
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
    	Logger::log(Logger::LogLevel::log_ERROR, "bind");
        perror("bind");
        exit(EXIT_FAILURE);
    }

	Logger::log(Logger::LogLevel::log_INFO, "2. Listen");	
    if (listen(serverSocket, 5) == -1) {
    	Logger::log(Logger::LogLevel::log_ERROR, "Listen");
        perror("listen");
        exit(EXIT_FAILURE);
    }

    running = true;
	Logger::log(Logger::LogLevel::log_INFO, "Server started on port %d", port);
	
    acceptConnections();    
}

void vsServer::stop() {
    running = false;
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join(); 
        }
    }
}

void vsServer::acceptConnections() {
    while (running) {
        socklen_t addrLen = sizeof(xClientAddr);
        clientSocket = accept(serverSocket, (xPortTypeClientSocketAddr*)(&xClientAddr), &addrLen);
        
        if (clientSocket < 0) {
            Logger::log(Logger::LogLevel::log_ERROR, "NO_SOCKET! sock - %d ", clientSocket);
            continue;
        }

        try {
            Logger::log(Logger::LogLevel::log_INFO, "3. Accept sock - %d ", clientSocket);
            vPortSetSocketRecvTimeout(clientSocket, DEFALUT_SOCKET_SEND_TIMEOUT_MS);
            vPortSetSocketSendTimeout(clientSocket, DEFALUT_SOCKET_SEND_TIMEOUT_MS);
        } catch (const std::exception& e) {
            Logger::log(Logger::LogLevel::log_INFO, "Accept sock - %d, error: %s", clientSocket, e.what());
//            close(clientSocket); // Закрыть сокет в случае ошибки
            continue;
        }

        if (clientSocket >= 0) {
            try {
                std::lock_guard<std::mutex> lock(mtx);
                Logger::log(Logger::LogLevel::log_INFO, "4. CONNECT! sock - %d ", clientSocket);
                threads.emplace_back(&vsServer::handleClient, this, clientSocket);
                Logger::log(Logger::LogLevel::log_INFO, "5. EXIT! sock - %d ", clientSocket);
            } catch (const std::exception& e) {
                Logger::log(Logger::LogLevel::log_ERROR, "emplace_back! sock - %d, error: %s", clientSocket, e.what());
//                close(clientSocket); // Закрыть сокет в случае ошибки
            }
        } else {
            Logger::log(Logger::LogLevel::log_ERROR, "Fatal sock - %d ", clientSocket);
        }
    }
    
    Logger::log(Logger::LogLevel::log_ERROR, "Error Exit");
//    close(clientSocket); // Закрыть сокет в случае ошибки
}



void vsServer::handleClient(int clientSocket) {
    #define SERVER_ADDRESS 0

    unsigned char* pcReceiveBuffer = (unsigned char*)malloc(CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE);
    unsigned char* pcSendBuffer = (unsigned char*)malloc(CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE);
    if (!pcReceiveBuffer || !pcSendBuffer) {
        Logger::log(Logger::LogLevel::log_WARNING, "(Socket %d) Failed to allocate memory", clientSocket);
        if (pcReceiveBuffer) free(pcReceiveBuffer);
        iPortCloseServerSocket(clientSocket);
        return;
    }
    
    str_SGP2_ML_ServerHandle xServerHandle;

    for (;;) {
        try {
            Logger::log(Logger::LogLevel::log_INFO, "Init server handle, socket %d", clientSocket);
            vSGP2_ML_InitServerHandle(&xServerHandle, &xServer, CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE, SERVER_ADDRESS, MY_SERVER_ID);
			Logger::log(Logger::LogLevel::log_INFO, "Init step 2, socket %d", clientSocket);
            int k = 0;
            
			pcSGP_ClientStack* _clStack = new pcSGP_ClientStack();
            
            if (_clStack == nullptr) { 
            	Logger::log(Logger::LogLevel::log_ERROR, "ERROR CREATE CLIENT STACK, socket %d", clientSocket);	
				continue;
			}
			
			/*
				Запуск дополнительного потока, который будет получать информацию 
				из разых источников 
				
			*/
			_clStack->cxManagerData->threadInfo->thStart();
//		    _clStack->cxManagerData->threadInfo->thStart();
			
			
	
			// -------------------------------------------------------------------------------
			
			Logger::log(Logger::LogLevel::log_INFO, "Init OK, socket %d", clientSocket);
            for (;;) {
                try {
                    if (k >= CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE) break;
                    int i = iPortSocketRead(clientSocket, &pcReceiveBuffer[k], (CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE - k));
                    if (i <= 0) break;
                    k += i;

                    ServerMutex.lock();
                    _clStack->xServer    = this;
                    _clStack->xSessionDS = FSessionDS;
                    _clStack->iSocket    = clientSocket;
                    ServerMutex.unlock();

                    int result = usSGP2_ML_ServerProcessRequest(&xServerHandle, _clStack, pcReceiveBuffer, k, pcSendBuffer, CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE);
                    if (result > 0) {
                        if (iPortSocketWrite(clientSocket, pcSendBuffer, result) != result) {
                            Logger::log(Logger::LogLevel::log_WARNING, "(Socket %d) Socket Write break", clientSocket);
                            break;
                        }
                    }
					
                    k = 0;
                 
                    
                } catch (...) {
                    Logger::log(Logger::LogLevel::log_ERROR, "Error processing request for socket %d", clientSocket);
                }
            }
            
	            iPortCloseServerSocket(clientSocket);
	            _clStack->cxManagerData->threadInfo->thStop();
            break;
        } catch (...) {
            Logger::log(Logger::LogLevel::log_ERROR, "Error initializing server handle for socket %d", clientSocket);
        }
    }

	try { 
		Logger::log(Logger::LogLevel::log_INFO, "Close socket %d", clientSocket);
		free(pcReceiveBuffer);
		free(pcSendBuffer);
	} catch(...) {
		Logger::log(Logger::LogLevel::log_ERROR, "Error free socket %d", clientSocket);	
	}

}



