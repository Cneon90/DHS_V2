#ifndef TERMINAL_LIST_H
#define TERMINAL_LIST_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <mutex>

#define TERMINAL_ITEM_SIZE     		144
#define TERMINAL_NAME_LENGTH   		64
#define TERMINAL_STATUS_LENGTH 		24
#define TERMINAL_INFO_LENGTH   		48
#define TERMINAL_RS_INFO_LENGTH   	66

/*
	Список терминалов с дополнительной информацией 
*/


// Элемент списка
class Terminal_Info_Item 
{ 
	public:
	    uint32_t 	ID;							 
		uint32_t 	EXTDBID;				
		char  	 	TNAME[TERMINAL_NAME_LENGTH];	 	
		uint16_t 	RALG;		
		char     	STATUS[TERMINAL_STATUS_LENGTH];
		char     	SW_INFO[TERMINAL_INFO_LENGTH];	
		
		unsigned char	rs_info[TERMINAL_RS_INFO_LENGTH];
		uint32_t		rs_infoLen = 0;

	    // Метод добавления данных в rs_info
	    void addRsInfo(const unsigned char* new_data, size_t data_length);
};

// Список терминалов
class Terminal_Info_List
{
	protected: 
		std::vector<Terminal_Info_Item>* terminalInfoList = nullptr;
		std::mutex dataMutex;
	public:
		Terminal_Info_List();//constructor
		void Add(Terminal_Info_Item& newItem);// Add or update
		void Clear();
		int Count();
		
		Terminal_Info_Item& getItem(int index);
		std::vector<Terminal_Info_Item>* getList();
		 	
};



#endif
