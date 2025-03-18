#include "Terminal_Info_List.h"

// ����� ���������� ������ � rs_info
void Terminal_Info_Item::addRsInfo(const unsigned char* new_data, size_t data_length) {
    // ���������, ����� ����� ����� ������ �� ��������� ������������ ����� �������
    if (data_length > TERMINAL_RS_INFO_LENGTH) {
        // ���� ������ ������� �������, �������� ��
        data_length = TERMINAL_RS_INFO_LENGTH;
    }

    // �������� ������ � rs_info
    std::memcpy(rs_info, new_data, data_length);
}		



// Constructor
Terminal_Info_List::Terminal_Info_List() 
{
	dataMutex.lock();	
	terminalInfoList = new std::vector<Terminal_Info_Item>;	
	dataMutex.unlock();
} 

// Add or update
void Terminal_Info_List::Add(Terminal_Info_Item& newItem) { 
    // ���������� ������
	dataMutex.lock();
    for (auto& item : *terminalInfoList) {
        if (item.ID == newItem.ID) {
            
            // update
            item.EXTDBID = newItem.EXTDBID;
            std::strcpy(item.TNAME, newItem.TNAME);
            item.RALG = newItem.RALG;
            std::strcpy(item.STATUS, newItem.STATUS);
            std::strcpy(item.SW_INFO, newItem.SW_INFO);
            dataMutex.unlock();
            
            return; // ������� ��������, ������� �� �������
        }
    }

	// add	
	terminalInfoList -> push_back(newItem);	
	dataMutex.unlock();
}

// Clear list
void Terminal_Info_List::Clear() 
{
	dataMutex.lock();		
	terminalInfoList->clear();
	dataMutex.unlock();
}

// ���������� ������� � ������
int Terminal_Info_List::Count() 
{ 
	std::lock_guard<std::mutex> lock(dataMutex); 
	return terminalInfoList->size();
}

Terminal_Info_Item& Terminal_Info_List::getItem(int index) 
{ 
	std::lock_guard<std::mutex> lock(dataMutex); 
	if(index >= 0 && index < terminalInfoList->size()) { 
		return terminalInfoList->at(index);	
	}
}
 
std::vector<Terminal_Info_Item>* Terminal_Info_List::getList() { 
	std::lock_guard<std::mutex> lock(dataMutex); 
	return terminalInfoList;
}



