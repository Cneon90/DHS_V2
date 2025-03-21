#include "ThRequestInfo.h"

// Constructor
ThRequestInfo::ThRequestInfo()
{
//	std::lock_guard<std::mutex> lock(InfoMutex); 	
	ClientRS = new thClientConnect();	
	Term_rs_info = new cmd_info_RS();
}

// Destructor
ThRequestInfo::~ThRequestInfo() 
{
//	std::lock_guard<std::mutex> lock(InfoMutex); 
	thWaitForCompletion();	
	delete ClientRS;
}
 
// Start thread
void ThRequestInfo::thStart()  
{
	std::lock_guard<std::mutex> lock(InfoMutex); 
	Terminated = false;	
	
	if(FTHID == 0xFE) { 
		Logger::log(Logger::LogLevel::log_DEBUG, "thRequestinfo |thStart | thread is run | return");
		return;	
	}
	FTHID = 0xFE;
	thRequest = std::thread(&ThRequestInfo::StartExecute, this); 	
}

void ThRequestInfo::thStop() 
{ 
	std::lock_guard<std::mutex> lock(InfoMutex); 
	Terminated = true;
	FTHID = 0x00;
}	

// Wait stop thread
void ThRequestInfo::thWaitForCompletion() {
	
	if (thRequest.joinable()) {
		thRequest.join(); 
	}
}		
//---------------------------------------------------


void ThRequestInfo::StartExecute()
{
    while(!Terminated)
    {
		vPortSleepMs(20); /*20*/
//		std::unique_lock<std::mutex> lock(InfoMutex); 
		std::lock_guard<std::mutex> lock(InfoMutex);
		
		
		
		if(TerminalInfoList == nullptr) { 
//			Logger::log(Logger::LogLevel::log_DEBUG, "List empty");
			continue;
		}
			
		if(xSessionRS == nullptr) { 
			Logger::log(Logger::LogLevel::log_DEBUG, "xSessionRS empty");
			continue;
		}
		
		if(ClientRS == nullptr) { 
			Logger::log(Logger::LogLevel::log_DEBUG, "ClientRS empty");
			continue;
		}
		
//		Logger::log(Logger::LogLevel::log_DEBUG, "List count: %d ", TerminalInfoList->Count() );

		
		int count = TerminalInfoList->Count();
		if(count == 0) { 
			Logger::log(Logger::LogLevel::log_DEBUG, " thread RequestInfo | List Count = 0 | return");
			continue;
		}
		
		for(int i=0; i < count; i++) {
				Terminal_Info_Item& Terminal = TerminalInfoList->getItem(i);
//				Logger::log(Logger::LogLevel::log_DEBUG, "ThRequest | thClient | Terminal ID: 0x%X ", Terminal.ID );	
				
				ClientRS->init();
				ClientRS->setIndex(100);
				if(xSessionRS != nullptr) ClientRS->setSession(xSessionRS);
				Term_rs_info->setID(Terminal.ID); 
				ClientRS->setCommand(Term_rs_info);			
				ClientRS->Execute();
			
				unsigned char state = ClientRS->getLastError();
				if (state != SGP2_ML_RESULT_SUCCESS) { 
					Logger::log(Logger::LogLevel::log_ERROR, "ThRequest | thClient | LAST ERROR: %d ", state);
                    xSessionRS->printProperties();
				} 
				
//				Terminal.addRsInfo(Term_rs_info->getAnswer(), 62);
			
//				// Копируем полученную с сервера информацию в свойство терминала
				std::memcpy(Terminal.rs_info, Term_rs_info->getAnswer(), Term_rs_info->getAnswerLen() /*TERMINAL_RS_INFO_LENGTH*/); // Замените на правильный размер	
				Terminal.rs_infoLen = Term_rs_info->getAnswerLen(); // Сохраняем длину сообщения


			 	
//				//****** TEMP *******
//				char name[32];
//				std::memset(name,0,32);
//				std::memcpy(name,Terminal.rs_info+4, 32); 
//			    name[32] = '\0';  
//			    Logger::log(Logger::LogLevel::log_DEBUG, "ThRequest | thClient | Term_name:%s ", name);  	
//			    //****** TEMP *******
		    
		} 


		

	}	


	
}
