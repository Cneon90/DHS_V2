#include "thClient.h"

//===========================================================================================================================================================
void            vClientSendData            (void* pvCB_Arg, const unsigned char* pcSrcData, unsigned short usSrcSize);
unsigned short  usClientReceiveData        (void* pvCB_Arg,       unsigned char* pcDstData, unsigned short usDstSize, unsigned short usTimeout_ms) ;
char*           pcClientGetServerLogin     (void* pvCB_Arg, unsigned int uiServerID);
char*           pcClientGetServerPassword  (void* pvCB_Arg, unsigned int uiServerID);

str_SGP2_ML_ClientCB xClientCB =
{
	&vClientSendData,
	&usClientReceiveData,
	&pcClientGetServerLogin,
	&pcClientGetServerPassword,
};

//===========================================================================================================================================================

// Constructor
thClientConnect::thClientConnect()
{
	std::unique_lock<std::mutex> lock(mtx);
//	Logger::log(Logger::LogLevel::log_DEBUG, "Client | Create thClientConnect");
//	mtx.lock(); 	
	try{
	
		ConnectHandler = new str_ConnectionHandle;// (str_ConnectionHandle*)malloc(sizeof(str_ConnectionHandle));
		if(ConnectHandler == nullptr) {
			Logger::log(Logger::LogLevel::log_ERROR, "Client | Create ConnectHandler");	 
			return;
		}
		ConnectHandler->pxConnectionAttr = new str_ClientConnectionAttributes;// (str_ClientConnectionAttributes*)malloc(sizeof(str_ClientConnectionAttributes));	
		if(ConnectHandler == nullptr) {
			Logger::log(Logger::LogLevel::log_ERROR, "Client | Create ConnectHandler->pxConnectionAttr");	 
			return;
		}
		
		Terminated = false;
		FResponseMessage = new str_SGP2_ML_Message;
		
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client | SIZE: %d", sizeof(*FResponseMessage) );
		
		if(FResponseMessage == nullptr) {
			Logger::log(Logger::LogLevel::log_ERROR, "Client | Create FResponseMessage");	 
			return;
		}
		
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client | Create OK");
	} catch(...) { 
		Logger::log(Logger::LogLevel::log_ERROR, "Client | Constructor Error");
	}
//	mtx.unlock();
}

// Destructor
thClientConnect::~thClientConnect()
{
//	mtx.lock(); 
	delete ConnectHandler->pxConnectionAttr;
	delete ConnectHandler;
	delete FResponseMessage;
//	mtx.unlock();	
} 


void vClientSendData (void* pvCB_Arg, const unsigned char* pcSrcData, unsigned short usSrcSize)
{
    int i;
    xPortTypeSocket* pxSocket;
    str_ConnectionHandle* connection = (str_ConnectionHandle*)pvCB_Arg;    
    pxSocket = (xPortTypeSocket*)connection->pxTCP_Connection;

    if (pxSocket == NULL) return;
    i = iPortSocketWrite(*pxSocket, pcSrcData, usSrcSize);
    if (i <= 0) {
        pxSocket = NULL;
    }
}


unsigned short  usClientReceiveData (
    void* pvCB_Arg,
	unsigned char* pcDstData,
	unsigned short usDstSize,
	unsigned short usTimeout_ms)
{
	int i;
	str_ConnectionHandle* connection = (str_ConnectionHandle*)pvCB_Arg;

	xPortTypeSocket* pxSocket;

	pxSocket = (xPortTypeSocket*) connection->pxTCP_Connection;

	if (pxSocket == 0) return 0;
	i = iPortSocketRead(*pxSocket, pcDstData, usDstSize);
	if (i < 0) {pxSocket = 0; return 0;};

	return (unsigned short)(i);	
}


char* pcClientGetServerLogin    (void* pvCB_Arg, unsigned int uiServerID)
{
	return (*(str_ConnectionHandle**)pvCB_Arg)->pxConnectionAttr->cUser;
};

char* pcClientGetServerPassword (void* pvCB_Arg, unsigned int uiServerID)
{
	return (*(str_ConnectionHandle**)pvCB_Arg)->pxConnectionAttr->cPass;
};

void __fastcall vSendServerOperationErrorString(unsigned char ucSGP2_ML_ErrorCode)
{
    switch (ucSGP2_ML_ErrorCode) {
        case SGP2_ML_RESULT_CONNECTION_LOST:
            Logger::log(Logger::LogLevel::log_ERROR, "No response from Server. Connection lost.");
            break;
        case SGP2_ML_RESULT_RESPONSE_DATA_FALUT:
            Logger::log(Logger::LogLevel::log_ERROR, "Server response data error.");
            break;
        case SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR:
            Logger::log(Logger::LogLevel::log_ERROR, "Server response data format error.");
            break;
        case SGP2_ML_RESULT_SERVER_EXCEPTION:
            Logger::log(Logger::LogLevel::log_ERROR, "Server exception.");
            break;
        case SGP2_ML_RESULT_AUTH_FAILURE:
            Logger::log(Logger::LogLevel::log_ERROR, "Server Auth failure.");
            break;
        case SGP2_ML_RESULT_AUTH_REQUIRED:
            Logger::log(Logger::LogLevel::log_ERROR, "Server Auth required.");
            break;
        default:
            Logger::log(Logger::LogLevel::log_ERROR, "Unknown error code: %u", ucSGP2_ML_ErrorCode);
            break;
    }    
};


void thClientConnect::Execute()
{
	while (! Terminated) {
//		mtx.lock();
        std::lock_guard<std::mutex> lock(mtx);  // Автоматическая блокировка и разблокировка мьютекса
		StateConnect = -1;
		xPortTypeServerSocketAddr server_addr;
	
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Start Init ", index);
		// Init 
		vSGP2_ML_InitClientHandle(
	        &xClientHandle,
	        &xClientCB,
	        ucClientDataBuf,
	        CLIENT_BUFF_SIZE,
	        SGP2_ML_CT_GLOBAL,
	        ConnectHandler->pxConnectionAttr->uiAddress,
	        FTimeOut
		);

//		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Init - OK ", index);
		_reconnect:
			
		if(Terminated == true) { 
//			Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Connect close", index);
			break;	
		}	

		ConnectHandler->pxTCP_Connection = socket(AF_INET, SOCK_STREAM, 0);
//		Logger::log(Logger::LogLevel::log_INFO, "Client %d, Connect create socket %d", index, ConnectHandler->pxTCP_Connection);
		
//		if (((int)(ConnectHandler->pxTCP_Connection)) < 0) {
		if (((unsigned int)(ConnectHandler->pxTCP_Connection)) == INVALID_SOCKET) {
//			std::cerr << "Error create socket \n";
			Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Connect create socket", index);
			StateConnect = SGP2_ML_RESULT_ERROR_SOCKET;	
			Terminated = true;
//			goto _reconnect;
		}	
		vPortSetSocketRecvTimeout(ConnectHandler->pxTCP_Connection, DEFALUT_SOCKET_SEND_TIMEOUT_MS);
		
		memset((char*)(&server_addr), 0, sizeof(server_addr));
		
		
		//==== only IP ====
//		server_addr.sin_family = AF_INET;
//		server_addr.sin_port = htons(ConnectHandler->pxConnectionAttr->usPort); // htons(7000);// 
//		server_addr.sin_addr.s_addr = inet_addr(ConnectHandler->pxConnectionAttr->cHost); //
		//=================================

		//===== Resolve hostname ========
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // IPv4
        hints.ai_socktype = SOCK_STREAM;


        if (getaddrinfo(ConnectHandler->pxConnectionAttr->cHost, nullptr, &hints, &res) != 0) {
//            std::cerr << "Error resolving host: " << ConnectHandler->pxConnectionAttr->cHost << std::endl;
//            Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Connect resolving host: %s", index, ConnectHandler->pxConnectionAttr->cHost);
            StateConnect = SGP2_ML_RESULT_RESOLVING_HOST;
            Terminated = true;
            closesocket(ConnectHandler->pxTCP_Connection);
            goto _reconnect;
        }

        memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
        server_addr.sin_port = htons(ConnectHandler->pxConnectionAttr->usPort);
        freeaddrinfo(res);
        //=================================


		if (connect(ConnectHandler->pxTCP_Connection, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
			Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Connection failed", index);
			StateConnect = SGP2_ML_RESULT_CONNECTION_LOST;
//			ClientResponseMessage.usCmd;
//			throw std::runtime_error("Connection failed");
			Terminated = true;
					
			closesocket(ConnectHandler->pxTCP_Connection);
			goto _reconnect;
		}
	
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Start of authorization", index);
		//Authorization on the server 
		for (;;)
		{
			StateConnect = ucSGP2_ML_InitClientRequest(&xClientHandle, &ConnectHandler, &pxMessage);
			if (StateConnect)
			{
				vSendServerOperationErrorString(StateConnect);
//				if (StateConnect == SGP2_ML_RESULT_CONNECTION_LOST) {
				if (StateConnect != SGP2_ML_RESULT_SUCCESS) {	
				   Terminated = true; //goto _reconnect;
//				   printf("\n ERROR _CLOSE_ \n");
				   Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Connection close", index);
//				   break;
				   closesocket(ConnectHandler->pxTCP_Connection);	
				   goto _reconnect;
				}

			};
			
			pxMessage->usCmd = 0x0001;
	        pxMessage->ucPar = 0x00;
	        
			StateConnect = ucSGP2_ML_ExecuteClientRequest(&xClientHandle, &ConnectHandler, &pxMessage);
	        if (StateConnect)
	        {
	            vSendServerOperationErrorString(StateConnect);
	            if (StateConnect == SGP2_ML_RESULT_CONNECTION_LOST) goto _reconnect;
	            break;
	        };
	
	
			if (pxMessage->ucPar & 0x80)
			{
				Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Data %s", index, pxMessage->pcData);	
				goto _reconnect;
			};
	

			break;
		}

		
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Authorization - OK", index);
		// Main loop 
		for (;;)
		{
			vPortSleepMs(10);
		
			// Init	
			StateConnect = ucSGP2_ML_InitClientRequest(&xClientHandle, &ConnectHandler, &pxMessage);
	        if (StateConnect)
	        {
	            vSendServerOperationErrorString(StateConnect);
	            if (StateConnect == SGP2_ML_RESULT_CONNECTION_LOST) goto _reconnect;
	            if (StateConnect == SGP2_ML_RESULT_RESPONSE_DATA_FALUT) goto _reconnect;
	            std::cerr << "Server auth failure \n";
	            Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Server auth failure ", index);
	     		goto _reconnect;
	        };
	        
			if(FCmd == nullptr) { continue;	}
			
			// Prepare 
			try{
				FCmd->setMessage(pxMessage);
		    	bSyncRequestFlag =  FCmd->bGetCurrentRequest(pxMessage);  // bGetCurrentRequest(pxMessage); 
	    	} catch(...) { 
	    		Logger::log(Logger::LogLevel::log_ERROR, "Client %d, While the command is running ", index);	
			}
			
	        if (!bSyncRequestFlag)
	        {
	            pxMessage->usCmd = 0x00;
	            pxMessage->ucPar = 0x00;
	        }
	        
//	        if(index == 100) { 
//	        	int r = 11;
//			}
	        
	        //Request
	        StateConnect = ucSGP2_ML_ExecuteClientRequest(&xClientHandle, &ConnectHandler, &pxMessage);
	        if (StateConnect)
	        {
	            bSyncRequestFlag = false;
	            vSendServerOperationErrorString(StateConnect);
	            if (StateConnect == SGP2_ML_RESULT_CONNECTION_LOST) goto _reconnect;
	            if (StateConnect == SGP2_ML_RESULT_RESPONSE_DATA_FALUT) goto _reconnect;
	            break;
	        };
	        
	        if (bSyncRequestFlag)
	        {
	            bSyncRequestFlag = false;
	            if(FCmd == nullptr) { continue;	}
//				vResponseReceived(index, pxMessage);
				Terminated = FCmd->vResponseReceived(pxMessage);
				
				if(Terminated == true) {
					break;
				}
							
	        };
			
			
		}	
		
		//=== Connection close =====
	  	if(Terminated == true) {
//	  		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Terminate - OK", index);
	  		
			break;
		}
	}
	
	try
	{
		closesocket(ConnectHandler->pxTCP_Connection);
//		Logger::log(Logger::LogLevel::log_DEBUG, "Client %d, Connect close socket", index);	
	} catch(...) {
		Logger::log(Logger::LogLevel::log_ERROR, "Client %d, Error: Connect close socket", index);		
	}
	
//	if (ConnectHandler->pxTCP_Connection != -1) { 
//            close(ConnectHandler->pxTCP_Connection); 
//            ConnectHandler->pxTCP_Connection = -1; 
//            printf("Th client close \n");
//    }
	
//   	mtx.unlock();
}


// Set connection soket
void thClientConnect::setSocket(const char* host, unsigned short port, unsigned int address) {
	std::lock_guard<std::mutex> lock(mtx); 	
	if(host == nullptr) { return; }
	if(port <= 0 ) { return; }
	
	
	try {
//	    Logger::log(Logger::LogLevel::log_INFO, "Setting socket with host: %s, port: %u, address: %u", host, port, address);
	    strncpy(ConnectHandler->pxConnectionAttr->cHost, host, SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1);
	    ConnectHandler->pxConnectionAttr->cHost[SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1] = '\0'; 
	    ConnectHandler->pxConnectionAttr->usPort    = port;
	    ConnectHandler->pxConnectionAttr->uiAddress = address;
	} catch (const std::exception& e) {
	    Logger::log(Logger::LogLevel::log_ERROR, "Exception occurred: %s", e.what());
	}
}
// 
void thClientConnect::setUser(const char* _user, const char* _pass){
    setUsermtx.lock();
    // Обнуляем массивы перед копированием
    std::memset(ConnectHandler->pxConnectionAttr->cUser, 0, SERVER_CONN_ATTR_TEXT_MAX_SIZE);
    std::memset(ConnectHandler->pxConnectionAttr->cPass, 0, SERVER_CONN_ATTR_TEXT_MAX_SIZE);

    // Копируем строки в массивы с учетом их длин и завершающего нуля
    size_t userLength = min(strlen(_user), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1));
    std::strncpy(ConnectHandler->pxConnectionAttr->cUser, _user, userLength);
    ConnectHandler->pxConnectionAttr->cUser[userLength] = '\0'; // Завершающий ноль

    size_t passLength = min(strlen(_pass), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1));
    std::strncpy(ConnectHandler->pxConnectionAttr->cPass, _pass, passLength);
    ConnectHandler->pxConnectionAttr->cPass[passLength] = '\0'; // Завершающий ноль

    setUsermtx.unlock();
}

// 
void thClientConnect::joinThread() { 
	joinmtx.lock(); 
    if (threadClientConnect.joinable()) {
        threadClientConnect.join();
    }
    joinmtx.unlock();
}


str_SGP2_ML_Message* thClientConnect::getMessageError() {
	msgmtx.lock();
	
	FResponseMessage->pcData = new unsigned char[1]; 
	FResponseMessage->usCmd = CMD_SES;
	FResponseMessage->ucPar = 1;
	FResponseMessage->usDataSize = 0; 
	FResponseMessage->usDataMaxSize = 1;    
	
	usSGP2_ML_MessagePutByte(FResponseMessage, StateConnect); // Start session OK
	msgmtx.unlock();	
	return FResponseMessage;	
}

// Session 

// Принимаем сразу указатель на сессию
void thClientConnect::setSession(Session* _session) 
{
////	mtx.lock();  // Блокируем мьютекс
    std::lock_guard<std::mutex> lock(mtx);  // Автоматическая блокировка и разблокировка мьютекса
//	try {
//        if (_session == nullptr) {
//            Logger::log(Logger::LogLevel::log_ERROR, "TASK_BASE | SESSION | nullptr");
//            return;
//        }
//
//        std::string _host    = _session->getHost();
//        unsigned short _port = _session->getPort();
//        unsigned int _id     = _session->getAddress();
//        std::string _user    = _session->getUser();
//        std::string _pass    = _session->getPass();
//
//        // Проверка на пустой хост
//        if (_host.empty()) {
//            Logger::log(Logger::LogLevel::log_ERROR, "Host cannot be empty");
//            return;
//        }
//
//        // Проверка на корректный диапазон порта
//        if (_port == 0 || _port > 65535) {
//            Logger::log(Logger::LogLevel::log_ERROR, "Port must be in the range 1-65535");
//            return;
//        }
//
//        // Проверка на пустые имя пользователя и пароль
//        if (_user.empty() || _pass.empty()) {
//            Logger::log(Logger::LogLevel::log_ERROR, "Username and password cannot be empty");
//            return;
//        }
////		mtx.unlock();
//        // Логика загрузки сессии
//        setSocket(_host.c_str(), _port, _id);
//        setUser(_user.c_str(), _pass.c_str());
//	}
//	catch (...) {
//    	Logger::log(Logger::LogLevel::log_ERROR, "An error occurred while setting the session.");
////    	mtx.unlock();
//	}
//
//	// Разблокируем мьютекс после обработки
////	mtx.unlock();

    try {
        if (_session == nullptr) {
            Logger::log(Logger::LogLevel::log_ERROR, "TASK_BASE | SESSION | nullptr");
            return;
        }

        std::string _host    = _session->getHost();
        unsigned short _port = _session->getPort();
        unsigned int _id     = _session->getAddress();
        std::string _user    = _session->getUser();
        std::string _pass    = _session->getPass();

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
        setSocket(_host.c_str(), _port, _id);
        setUser(_user.c_str(), _pass.c_str());
    }
    catch (const std::exception& e) {
//        Logger::log(Logger::LogLevel::log_ERROR, std::string("An error occurred while setting the session: ") + e.what());
    }
    catch (...) {
        Logger::log(Logger::LogLevel::log_ERROR, "An unknown error occurred while setting the session.");
    }

}


