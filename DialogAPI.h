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
	// Преобразование в формат удобный для отправки
	TDialogDTO getDialogDTO(strSgDialogItem& dialogItem);
	// Ищем диалог по ID
	strSgDialogItem getDialogItemByID(unsigned int _id);
	// InitData
	void DialogDTOItemClear(TDialogDTO& dialogDTO);	
	// Ищем ID первого диалога  
	unsigned int getQuestID_First();
	// Получаем текст решения по ID 
	std::string getSolutionText(unsigned int DialogId);
};



#endif
