#ifndef THSERVER2_H
#define THSERVER2_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <string.h>
#include <dirent.h>
#include <sched.h>
#include <time.h>
#include <iostream>

//#include <fstream> //exi

#include "SGP2/SGP2.h"
#include "TaskList.h"
#include "Accounts.h"
#include "thClient.h"
#include "Task_Connect.h"
#include "TaskRS_GetGroups.h"
#include "cmd_session.h"
#include "Monitoring.h"
#include "help_functions.h"
#include "Logger.h"
#include "Default.h"
#include "RelayServers.h"
#include "Session.h"
#include "cmd_session.h"
#include "cmdTaskTerminalListDS.h"
#include "help_files.h"

//#include "FileWatcher.h"

//#include "SgDialog\SgDialog.h"
//#include "SgDialog/SgDialogLib.h"

#include "DialogAPI.h"

#if _WIN32
	#include "port_windows.h"	
#elif __linux__
	#include "port_linux.h"	
#else 
	#error "UNKNOWN TARGET OS"
#endif




#define REQUIRED_MIN_STACK_SIZE            			0x40000000  // 1 GBytes (actual for linux port only)
#define REQUIRED_THREADS_COUNT             			65536
#define CONTROL_CONNECTION_IO_DATA_BUFFER_SIZE  	32768 
//#define SGP_CLIENT_STACK_TEXT_BUFF_SIZE 			1024
#define MY_SERVER_ID                                0xDEADBEEB
#define MY_SERVER_TYPE                     			0x70000000

#define CONFIG_SERVER_NAME_MAX_SIZE        			128

#define NO_SOCKET                          			0

/*
	Основной поток выполнения. 
	после запуска, сервер ждёт подключения
	после подключения, ждём команд 
	

*/

class vsServer;
class Session;


/* 
	Клиентский стэк   
	client_stack 
	
	Этот стек передается подключившимуся клиенту, в нем передаем все необходимые экземпляры,
	что бы не создавать их в процессе выполнения	
*/
typedef struct
{
	vsServer*       		xServer    		= nullptr;
	Session*        		xSessionDS 		= nullptr;
	Session*        		xSessionRS 		= nullptr;
	TaskConnect*    		xTaskDS    		= nullptr;
	MonitoringBase* 		xMonitoring		= new MonitoringWin(); 	
	cmdTask*        		xCmdTask    	= new cmdTask();
	cmdTaskTerminalListDS*	xCmdTaskDSList	= new cmdTaskTerminalListDS();
	cmdPingProxy* 			xCmdPing		= new cmdPingProxy();
	int 		    		iSocket    		= 0;
	
	DialogAPI*				QuestAPI		= new DialogAPI();
	//--- Terminal list --- 
	TManager_ClientData* 	cxManagerData	= new TManager_ClientData(); // Module data Client stack
	
	
	/*
	 Модуль клиенских данных (тут агрегируются персональные данные подключенного клиента)
			cxManagerData   	
			     |
			 threadInfo -- Поток опроса
			     |
		     GetTerminalInfoList - список с данными о терминале (DS + RS) серверов
	
	*/
} pcSGP_ClientStack;


class vsServer: public TaskListAttachBase {
	private:
		void StartListening();
		void acceptConnections();
    	void handleClient(int clientSocket);
		
    	int clientSocket;	
    	int serverSocket;
    	int Index;
    	int port;
    	bool running;
    	bool isDeamonMode;
    	
//    	char* Name; 
		std::string Name;
		
    	Accounts* FAccounts;
    	Session* FSessionDS;
    	std::mutex mtx; 
    	std::thread thServer;    	
    	std::vector<std::thread> threads;
		struct xPortTypeClientSocketAddr xClientAddr;
		int clHandler;
	
		uint8_t app_version_H;
		uint8_t app_version_L;
		uint8_t app_date_D;
		uint8_t app_date_M;
		uint8_t app_date_Y;		
//		TQuestList* FQuestList = nullptr;
		std::string FQuestPath = "";
	public:
		
		// Принимаем список вопросов
//		void setQuestList(TQuestList* _questsList) { 
//			FQuestList = _questsList; 	
//		}
		
//		TQuestList* getQuestList() { 
//			return FQuestList;
//		}

		void setQuestPath(std::string _path) {
			FQuestPath = _path; 	
		}
		
		std::string getQuestPath() { 
			return FQuestPath;
		}
		
		std::mutex tasksMutex; 	
		std::mutex thMutex; 
		std::mutex PasswordMutex;
		std::mutex xMessageMutex;
		std::mutex StateMutex;
		std::mutex ServerMutex;
		std::mutex MessageMutex;
		
		void thStartListening()  { thServer = std::thread(&vsServer::StartListening, this); }	
		
	    vsServer(int port);
	    ~vsServer();
	    
	    void start();
	    void stop();
	    
	    void setVer(uint8_t vH, uint8_t vL) 
		{ 
	    	app_version_H = vH;
			app_version_L = vL;	
		}
		
		void setDateRelease(uint8_t _d, uint8_t _m, uint8_t _y ) 
		{
			app_date_D = _d;
			app_date_M = _m;
			app_date_Y = _y;	
		}
		
		uint8_t getVerH()  { return app_version_H;	}
		uint8_t getVerL()  { return app_version_L;	}
		uint8_t getDateD() { return app_date_D;		}
		uint8_t getDateM() { return app_date_M;		}
		uint8_t getDateY() { return app_date_Y;		}		
	    
		void setIndex(int _index)  {	Index = _index;     	}	
		void setMode(bool _mode)   {	isDeamonMode = _mode;   }
		void setName(const char* newName) {	if (newName) {	Name = newName;	}}	
		
		const char* getName() const {
			return Name.c_str();  // Возвращаем C-style строку
		}
		
		void setAccounts(Accounts* _accounts) { FAccounts = _accounts;}		
		Accounts* getAccount() { return FAccounts;}
		
		int getClientSocket() { 
			return clientSocket;
		}
		
		void setClientHandler(int _handler) { 
			clHandler = _handler;
		}
		
		int getClientHandler() { 
			return clHandler;
		}
		
	
	
};

#endif
