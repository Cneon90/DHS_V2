#ifndef CMDTASKTERMINALLISTDS_H
#define CMDTASKTERMINALLISTDS_H
/*
	��������� ������ ���������� � DS �������
*/
#include "Default.h"
#include "Logger.h"
#include "cmd_Base.h"

#include "Manager_ClientData.h"

// ��������� ������ ���������� � DS �������
class cmdTaskTerminalListDS: public AbstractCommandProxy 
{
	protected:
		
		//------------------------------------------------
//		TManager_ClientData*  ManagerClientData = nullptr;	//�������� ��������� ��� ����������� � ����� �������, ���������� ��� �������
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

		// ��������� ������ ����������, ���� ����� ��������� ���������� ���������
		void setTerminalList(Terminal_Info_List* _termList);
};

#endif
