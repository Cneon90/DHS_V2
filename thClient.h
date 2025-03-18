/**
	Class connection client
		
	класс осуществляет подключение к серверу, в качестве параметра 
	принимает реквизиты для подключения (хост, порт, логин, пароль) 
	и класс команду ( setCommand(AbstractCommand* _cmd) )  
	
	Выполняет команду, и возвращает ответ. 
		
	230724 @kk
*/
/*******************************************************************************
                                   INCLUDES
*******************************************************************************/
#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>

#include <thread>
#include <mutex>

#include <string.h>
#include <dirent.h>
//#include <pthread.h>
#include <sched.h>
#include <time.h>



#include <iostream>

#include "SGP2/SGP2.h"
#include "Default.h"


#if _WIN32
	#include "port_windows.h"	
#elif __linux__
	#include "port_linux.h"	
#else 
	#error "UNKNOWN TARGET OS"
#endif

#include "cmd_Base.h"


#define DEFALUT_SOCKET_SEND_TIMEOUT_MS     			6000
#define CLIENT_BUFF_SIZE 				 			32768
//#define CLIENT_BUFF_SIZE 							52769
#define SERVER_CONN_ATTR_TEXT_MAX_SIZE   			256

#define CONNCET_TIMEOUT_MS  						2000
#define THREAD_RESTART_FLAG bCheckThreadRestartFlag(pxTerminal)
#define CHECK_RECONNECT_MACROS() if (THREAD_RESTART_FLAG) {iPortCloseClientSocket(xConnectionSocket);goto __restart_terminal_connection_thread;};
#define RECONNECT_MACROS() {iPortCloseClientSocket(xConnectionSocket); goto __reconnect;};
#define DATALOG_SYNCHRO_HANDLE_REWRITE_PERIOD       64
#define OBJECT_MONITORING_STATE_SAVE_PERIOD         64
#define DEFAULT_THREAD_STACK_SIZE          			0x4000      // 16 kBytes (Don't touch!!!!)

#define SGP2_ML_RESULT_RESOLVING_HOST          		7
#define SGP2_ML_RESULT_ERROR_SOCKET					8

#include "Logger.h"
#include "Session.h"

/*******************************************************************************
                                   Structs
*******************************************************************************/
typedef struct
{
	char cName[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
	char cUser[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
	char cPass[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
	char cHost[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
	unsigned short usPort;
	unsigned int   uiAddress;
} str_ClientConnectionAttributes;
//--------------------------------------------------------------------------------
typedef struct
{
	xPortTypeSocket pxTCP_Connection;
	str_ClientConnectionAttributes* pxConnectionAttr;
} str_ConnectionHandle;
//--------------------------------------------------------------------------------

/*******************************************************************************
                                   
*******************************************************************************/
class thClientConnect 
{
	private:
		str_ConnectionHandle* ConnectHandler; 

		bool Terminated       = false;
		bool bSyncRequestFlag = false;	
		uint16_t index;
		
		std::mutex mtx;
		std::mutex msgmtx;
		std::mutex joinmtx;
		std::mutex setUsermtx;
		std::thread threadClientConnect;
		
		//--Conn--//
		str_SGP2_ML_ClientHandle xClientHandle;
		str_SGP2_ML_Message* FResponseMessage;
		unsigned char    	 ucClientDataBuf[CLIENT_BUFF_SIZE];
		unsigned short   	 FTimeOut 	     = 5000;
		unsigned int     	 uiServerAddress = 0;
		str_SGP2_ML_Message* pxMessage = nullptr;
		AbstractCommand*     FCmd = nullptr;
		unsigned char 		 StateConnect = 0;
	public:
		// Constructor
		thClientConnect();
		~thClientConnect();
		
//		void setLastError(char* _lastError) { 
//			strncpy(LastError, _lastError, sizeof(LastError) - 1);
//        	LastError[sizeof(LastError) - 1] = '\0'; 
//		}

		str_SGP2_ML_Message* getMessageError(); 
		
		const unsigned char getLastError() const {
        	return StateConnect;
    	}
    	
    	void init() { 
//			ConnectHandler = nullptr; 
			Terminated 		 = false;
			bSyncRequestFlag = false ;	
			index			 = 0;
			//--Conn-//
//			str_SGP2_ML_ClientHandle xClientHandle;
//			FResponseMessage = nullptr;
//			unsigned char    	 ucClientDataBuf[CLIENT_BUFF_SIZE];
//			unsigned short   	 FTimeOut 	     = 5000;
//			unsigned int     	 uiServerAddress = 0;
//			pxMessage = nullptr;
//			FCmd = nullptr;
			StateConnect = 0;	
		}
		
		//Set connection soket
		void setSocket(const char* host, unsigned short port, unsigned int address);
		//Set Auth 
		void setUser(const char* _user, const char* _pass);		
		const char* getHost() {	return ConnectHandler->pxConnectionAttr->cHost;	}
		
		//Client - Index 
		void setIndex(uint16_t _index) { index = _index; }
		uint16_t getIndex() { return index;	}		
		
		// Thread
		void Execute();	
		void joinThread();
		void Start() {Terminated = true; }
		void Stop() { Terminated = false; }
		bool getState() {return Terminated;	}
		void startThread() { 
		    if (threadClientConnect.joinable()) {
            	threadClientConnect.join();  // Дожидаемся завершения потока
        	}
			threadClientConnect = std::thread(&thClientConnect::Execute, this); 
		}	
		bool isThread() { return Terminated; }    
		bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage); 

		void setCommand(AbstractCommand* _cmd) { 
			if(_cmd == nullptr) { return; }
			FCmd = _cmd; 
		}
		
		str_SGP2_ML_Message* getResponse() {
			return pxMessage;
		}
		
		void setSession(Session* _session); 
};



#endif
