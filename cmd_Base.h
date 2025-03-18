#ifndef CMD_BASE_H
#define CMD_BASE_H

#include "SGP2/SGP2.h"

#include "assert.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <mutex>

//-----------CMD------------------------------------------------------------------
//#define  CMD_PING  		 0x0000                                                 // 
//#define  CMD_INFO  		 0x0001                                                 // 
//#define  CMD_STATE  	 0x7000                                                 // 
//#define  CMD_TERM_LIST   0x7001                                                 // RS - terminal list 
//#define  CMD_DIR         0x5000                                                 // File Manager
//#define  CMD_CAN         0x8100                                                 // CAN List
//#define  CMD_LOG         0x8110                                                 // Log list
//#define  CMD_SET         0x81F0                                                 // Can Settings
//#define  CMD_SRH         0x81E0                                                 // Data search
//#define  CMD_SYS         0x1000                                                 // System
//--------------------------------------------------------------------------------

//--- format Response task ---
struct TaskResponse {
	 unsigned char* ucData;
	 uint32_t       uiLength;	
};

class AbstractCommand
{
	protected:
		std::mutex cmdMutex;
		str_SGP2_ML_Message xMessage;
//		TManager_ClientData*  ManagerClientData 		= nullptr;	//Менеджер создается при подключении в стеке клиента, передается при запросе
	public:
		bool isCommand(const str_SGP2_ML_Message* pxSrcRequestMessage);
		bool isCmd(unsigned short _Cmd);
		bool isPar(unsigned char _Par);
		unsigned short getCmd();                                                 
		unsigned char  getPar();
		
		void setMessage(str_SGP2_ML_Message* pxDstMessage);
		virtual bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage)      = 0;
		virtual bool vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage) = 0; 
		
//		void setManager(TManager_ClientData* _manager);	
};

// base class for command for proxy connect
class AbstractCommandProxy: public AbstractCommand {
	protected:
		str_SGP2_ML_Message* ClientRequstMessage 	= nullptr;
		str_SGP2_ML_Message  ClientResponseMessage;
		uint8_t FCount = 0;		
	public:
	
	void init() { 
		std::unique_lock<std::mutex> lock(cmdMutex); 
		FCount = 0;
	}	
		
	void setClientMessage(str_SGP2_ML_Message* _clientMsg) {
		init();
		std::unique_lock<std::mutex> lock(cmdMutex); 
		if(_clientMsg == nullptr) {	return;	}
		ClientRequstMessage = _clientMsg;	
	}
	
	virtual str_SGP2_ML_Message* getClientResponseMsg()	= 0;
	virtual ~AbstractCommandProxy() = default; // Виртуальный деструктор
//	virtual TaskResponse ucTaskResponse() = 0;		
			
};

// Time 
class AbstractCommandTime: public AbstractCommand
{
	private:

	protected:
		uint8_t FTS[4]; 
	public:
		void      setTS(uint8_t _ts[4])  { memcpy(FTS, _ts, sizeof(FTS));};    
		uint32_t  getTimeUnix();                                                
		std::string getTimeStr(const std::string& _format="%d.%m.%y %H:%M:%S"); 
};




#endif
