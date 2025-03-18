#ifndef CMDTASKTERMINALLISTDS_H
#define CMDTASKTERMINALLISTDS_H
/*
	ѕолучение списка терминалов с DS сервера
*/
#include "Default.h"
#include "Logger.h"
#include "cmd_Base.h"

#include "Manager_ClientData.h"

// ѕолучение списка терминалов с DS сервера
class cmdTaskTerminalListDS: public AbstractCommandProxy 
{
	protected:
		
		//------------------------------------------------
//		TManager_ClientData*  ManagerClientData = nullptr;	//ћенеджер создаетс€ при подключении в стеке клиента, передаетс€ при запросе
		Terminal_Info_List* termList = nullptr;
	public:
		
		cmdTaskTerminalListDS() {}		
		~cmdTaskTerminalListDS() {}	
		
	
		bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage);
		bool vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage);
		   
		str_SGP2_ML_Message* getClientResponseMsg() 
		{	
			std::unique_lock<std::mutex> lock(cmdMutex); 
			return &ClientResponseMessage;  
		}
		str_SGP2_ML_Message* getClientRequstMsg() 	
		{	
			std::unique_lock<std::mutex> lock(cmdMutex); 	
			return ClientRequstMessage; 	
		}	
		
//		void setManager(TManager_ClientData* _manager) 
//		{	
//			cmdMutex.lock();
//			if(_manager != nullptr)	
//			ManagerClientData = _manager;	
//			cmdMutex.unlock();
//		} 

		// указываем список терминалов, куда будем добавл€ть полученные терминалы
		void setTerminalList(Terminal_Info_List* _termList);
};

#endif
