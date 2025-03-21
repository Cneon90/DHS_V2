#ifndef THREQUESTINFO_H
#define THREQUESTINFO_H

#include <mutex>
#include <thread>
#include "Logger.h"
#include "thClient.h"
#include "Terminal_Info_List.h"
#include "Session.h"

#include "cmd_info_RS.h"
/*
	Поток
	Опрос серверов, для получения информации 
*/
class ThRequestInfo
{
	private:
		std::thread thRequest;
		std::mutex InfoMutex;
		
		Terminal_Info_List*	TerminalInfoList 	= nullptr;	
		Session*			xSessionRS			= nullptr;
			
		// --- Th ---
		void StartExecute();
		bool Terminated = false;
		thClientConnect* ClientRS = nullptr; // Клиент для подключения 
		
		cmd_info_RS* Term_rs_info;
		uint8_t FTHID = 0x00;
	public:
		
		ThRequestInfo();
		~ThRequestInfo();
		void thStart();
		void thStop(); 
		
		void thWaitForCompletion();	
		//-- -- -- -- ---- -- -- -- ---- -- -- -- --
		void setTerminalList(Terminal_Info_List* _list) 
		{ 
			std::lock_guard<std::mutex> lock(InfoMutex); 
			TerminalInfoList = _list;
		}
		
		void setCurrentSessionRS(Session* _session) 
		{ 
			std::lock_guard<std::mutex> lock(InfoMutex);
            if(_session != nullptr)
			xSessionRS = _session;
		}
		
//		Terminal_Info_List* getTerminalList() 
//		{ 
//			std::lock_guard<std::mutex> lock(InfoMutex); 
//			return TerminalInfoList; 
//		}
		
			
};

#endif
