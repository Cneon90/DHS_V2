#ifndef DIALOGAPI_H
#define DIALOGAPI_H

#include <iostream>
//#include <dirent.h>
#include <cstring>
#include <vector>

#include "SgDialog\SgDialog.h"
#include "Dialog_DTO.h"


class DialogAPI
{
private:
	void ConvertDialog(str_xDialogItemDataStruct& xDialogItemData, TDialogDTO& dialogDTO);  	    
protected:
	TSgDialog* TDialog = new TSgDialog();		
public:
	QeustItem getFirstQuest(std::string _path, std::string _name, enSgDialogItemType etype);
	bool init(std::string _path);
	// �������������� � ������ ������� ��� ��������
	TDialogDTO getDialogDTO(strSgDialogItem& dialogItem);
	// ���� ������ �� ID
	strSgDialogItem getDialogItemByID(unsigned int _id);
	// InitData
	void DialogDTOItemClear(TDialogDTO& dialogDTO);	
	// ���� ID ������� �������  
	unsigned int getQuestID_First();
	// �������� ����� ������� �� ID 
	std::string getSolutionText(unsigned int DialogId);
};



#endif
