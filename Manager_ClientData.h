#ifndef MANAGER_CLIENTDATA_H
#define MANAGER_CLIENTDATA_H

/*
	�������� ���������� ������� ������� 
	- ���������� ����������� ����� 
	
	
	- �������� ������ ����� ���������
	- �������� ������ ����� ���������
	
	�������� ���������� ���� ������
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
		
		Terminal_Info_List* GetTerminalInfoList(); //  (get or create) ��������� ���������� ������ ����������
		
		
		
};



#endif
