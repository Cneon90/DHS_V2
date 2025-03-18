#ifndef MANAGER_CLIENTDATA_H
#define MANAGER_CLIENTDATA_H

/*
	Менеджер управления данными клиента 
	- реализация клиентского стека 
	
	
	- Заносить данные через менеджера
	- Выносить данные через менеджера
	
	Основной клиентский стек данных
*/
#include "Terminal_Info_List.h"
#include "ThRequestInfo.h"


class TManager_ClientData
{
	protected:
		std::mutex dataMutex;	
		Terminal_Info_List* TerminalInfoList = nullptr;
		
		
		
	public:
		TManager_ClientData();
		~TManager_ClientData();

		ThRequestInfo* threadInfo; // Thread get terminal info  
		
		Terminal_Info_List* GetTerminalInfoList(); //  (get or create) Получения экземпляра списка терминалов
		
		
		
};



#endif
