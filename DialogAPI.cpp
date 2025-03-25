#include "DialogAPI.h"

std::string DialogAPI::FileQuestPath = ""; // Инициализация статического члена

QeustItem DialogAPI::getFirstQuest(std::string _path, std::string _name, enSgDialogItemType etype) {
	QeustItem _item; 
	_item.fileName 		= "";
	_item.isValid  		= false;
	_item.size			= 0;
	_item.FirstQuest  	= "";
				
	if(_path == "") { return _item;	}
	if(_name == "") { return _item;	}			
	if(!TDialog	  ) { return _item;	}
	
//	TDialog->setCurrentPath(_path + "\\" + _name); 
//	bool isVailid = TDialog->init(); 
	bool isVailid = init(_path + "\\" + _name);
	if(!isVailid) {
		return	_item;	
	}
	
	TSgDialogList* DialogList  = TDialog->getItems(etype); /*SDIT_DIALOG*/	
	TSgDialogItem  quest 	   = DialogList->getFirst();
	_item.fileName 	 = _name;
	_item.isValid 	 = isVailid;
	_item.size 		 = 20;
	_item.FirstQuest = quest.Text; 	


	return _item;		
}


// Init file
bool DialogAPI::init(std::string _path)
{
	if(_path == "") { return false;	}
	if(!TDialog	  ) { return false;	}
	
	TDialog->setCurrentPath(_path); 
	return TDialog->init(); 	
}



// Преобразование в формат удобный для отправки
TDialogDTO DialogAPI::getDialogDTO(strSgDialogItem& dialogItem) 
{ 
	TDialogDTO dialogDTO;
	DialogDTOItemClear(dialogDTO);

	// Если диалог, переводим в структуру диалога
	if(dialogItem.eType != SDIT_DIALOG ) {
		// ERROR
		return dialogDTO;
	}		
	
	str_xDialogItemDataStruct xDialogItemData = *(str_xDialogItemDataStruct*) dialogItem.pvItemData; 	
	ConvertDialog(xDialogItemData, dialogDTO);

	return dialogDTO;
}

strSgDialogItem DialogAPI::getDialogItemByID(unsigned int _id) 
{
	return TDialog->getItemByID(_id); // Находим данные по ID записи
}

// InitData
void DialogAPI::DialogDTOItemClear(TDialogDTO& dialogDTO)	
{
	dialogDTO.QuestText = "";
	for(int i=0; i < DIALOG_MAX_ANSWERS_COUNT; i++) {
		dialogDTO.AnswerText[i] = "";
		dialogDTO.NextActID[i] 	= 0;	
	}	
}	


// Сборка диалога 
void DialogAPI::ConvertDialog(str_xDialogItemDataStruct& xDialogItemData, TDialogDTO& dialogDTO)  
{
	try {
		dialogDTO.QuestText   = TDialog->getNameByID(xDialogItemData.iQuestion); // Получаем текст вопроса
		dialogDTO.AnswerCount = xDialogItemData.ucAnswersCount;
		Logger::log(Logger::LogLevel::log_DEBUG, "DIALOG API | QUEST TEXT %s| ", dialogDTO.QuestText.c_str());	
		//Ответы
		for(int i=0; i < xDialogItemData.ucAnswersCount; i++) { 
			dialogDTO.AnswerText[i] = TDialog->getNameByID(xDialogItemData.iAnswer[i]);	
			dialogDTO.NextActID[i]  = xDialogItemData.iNextStep[i];				
			Logger::log(Logger::LogLevel::log_DEBUG, "DIALOG API | ANSWER TEXT %s| NEXT ID:%d ", dialogDTO.AnswerText[i].c_str(), dialogDTO.NextActID[i]);			
		}			
				
	} catch(...) { 
		Logger::log(Logger::LogLevel::log_ERROR, "DIALOG API | GET DATA | ERROR");		
	}
}


// Ищем ID первого диалога  
unsigned int DialogAPI::getQuestID_First() 
{ 
	// Получаем все диалоги
	TSgDialogList* Dialog_list = TDialog->getItems(SDIT_DIALOG);
	//Получить первый диалог
	TSgDialogItem FirstDialog = Dialog_list->getFirst();

	return FirstDialog.getID();
}


std::string DialogAPI::getSolutionText(unsigned int DialogId) { 
	return TDialog->getNameByID(DialogId);	
}	



