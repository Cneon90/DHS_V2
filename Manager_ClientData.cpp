#include "Manager_ClientData.h"

//constructor
TManager_ClientData::TManager_ClientData()
{
	threadInfo = new ThRequestInfo();
//	threadInfo->setTerminalList(TerminalInfoList);
}

//destructor
TManager_ClientData::~TManager_ClientData()
{
	
}

// Получения экземпляра списка  терминалов
Terminal_Info_List* TManager_ClientData::GetTerminalInfoList() 
{ 
	std::lock_guard<std::mutex> lock(dataMutex); 
	
	Terminal_Info_List* _trmList = nullptr;
	if(TerminalInfoList == nullptr) { 
		_trmList = new Terminal_Info_List();
		TerminalInfoList = _trmList;
		threadInfo->setTerminalList(TerminalInfoList);	
	} else { 
		_trmList = TerminalInfoList;
	}
	
	return _trmList;	
}



