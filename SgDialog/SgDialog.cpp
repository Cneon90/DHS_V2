//---------------------------------------------------------------------------
#include "SgDialog.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

unsigned char ucBufFileData[VIRTUAL_FILE_SIZE];
int            iBufFileDataSize = 0;

unsigned char ucReadFileData[VIRTUAL_FILE_SIZE];
int            iReadFileDataSize = 0;
unsigned char ucItemDataBuf[ITEM_DATA_BUF_SIZE];

int iReadDescriptor   (void* pvCB_Arg,       unsigned char* pcDstData, int iOffset, int iSize)
{
    int i;
    if (pvCB_Arg)
    {
        if (iOffset > iBufFileDataSize) return 0;
        i = iBufFileDataSize - iOffset;
        if (iSize > i) iSize = i;
        for (i = 0; i < iSize; i ++) pcDstData[i] = ucBufFileData[iOffset + i];
        return iSize;
	};
    if (iOffset > iReadFileDataSize) return 0;
    i = iReadFileDataSize - iOffset;
    if (iSize > i) iSize = i;
    for (i = 0; i < iSize; i ++) pcDstData[i] = ucReadFileData[iOffset + i];
    return iSize;
};



//int iReadDescriptor   (void* pvCB_Arg,       unsigned char* pcDstData, int iOffset, int iSize)
//{
//    int i;
//    if (pvCB_Arg)
//    {
//        if (iOffset > GBuf.iBufFileDataSize) return 0;
//        i = GBuf.iBufFileDataSize - iOffset;
//        if (iSize > i) iSize = i;
//        for (i = 0; i < iSize; i ++) pcDstData[i] = ucBufFileData[iOffset + i];
//        return iSize;
//	};
//    if (iOffset > GBuf.iReadFileDataSize) return 0;
//    i = GBuf.iReadFileDataSize - iOffset;
//    if (iSize > i) iSize = i;
//    for (i = 0; i < iSize; i ++) pcDstData[i] = GBuf.ucReadFileData[iOffset + i];
//    return iSize;
//};

//---------------------------------------------------------------------------
int iWriteDescriptor  (void* pvCB_Arg, const unsigned char* pcSrcData, int iOffset, int iSize)
{
    int i;
    if (iOffset == 0) iBufFileDataSize = 0;
	if (iOffset != iBufFileDataSize) return 0;
	for (i = 0; i < iSize; i ++)
	{
		if ((iOffset + i) >= VIRTUAL_FILE_SIZE) break;
		ucBufFileData[iOffset + i] = pcSrcData[i];
	};
	iBufFileDataSize += i;
	return i;
};


// =============================== TSgDialog ===================================
bool TSgDialog::init()
{
	int i;
	iSgDialogCreateNewDialog (&xSgDialogCB, 0);
	for (i = 0; i < iBufFileDataSize; i ++)
	{
		if (i >= VIRTUAL_FILE_SIZE) break;
		ucReadFileData[i] = ucBufFileData[i];
	};
	iReadFileDataSize = i;

	FILE *pxFile;
	
//	printf(asCurrentFileName.c_str());
	
	pxFile = fopen(asCurrentFileName.c_str(), "rb");

	if (pxFile == NULL)	{
//		throw std::runtime_error("open");
//		printf("ERROR Open!");
		Logger::log(Logger::LogLevel::log_ERROR, "Error open file %s", asCurrentFileName.c_str());
		return false;
	};

	iBufFileDataSize = fread(ucBufFileData, sizeof(unsigned char), VIRTUAL_FILE_SIZE, pxFile);
	fclose(pxFile);

	if (iSgDialogReadItemsInit(&xSgDialogCB, (void*)1, &xReadHandle) < 0) {
//		throw std::runtime_error("read");
//		printf("ERROR Read!");
		Logger::log(Logger::LogLevel::log_ERROR, "Error read file %s", asCurrentFileName.c_str());
		return false;
	};

	for (i = 0; i < iBufFileDataSize; i ++)	{
		if (i >= VIRTUAL_FILE_SIZE) break;
		ucReadFileData[i] = ucBufFileData[i];
	};
	
	iReadFileDataSize = i;
	bFileChangesUnsaved = false;
	
	return true;
}

//===Прочитать записи по типу=====
void TSgDialog::ReadItems(enSgDialogItemType etype)
{
	strSgDialogItem xItem;
	DialogList->Clear();
	while (iSgDialogReadNextItem     (&xSgDialogCB, 0, &xReadHandle, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) > 0) {
		if (xItem.eType != etype) continue;
		TSgDialogItem dgItem;
//		dgItem.Id   = Format( "%08X", xItem.uiID);
		char buffer[50];
		sprintf(buffer, "%08X", xItem.uiID);  // Форматируем в строку в шестнадцатеричной форме
		
		dgItem.Id   = buffer;
		dgItem.Type = etype;
		ui = xItem.uiSize;
		if (ui > (ITEM_DATA_BUF_SIZE - 1)) ui = (ITEM_DATA_BUF_SIZE - 1);
		ucItemDataBuf[ui] = 0;

		dgItem.Text =  (std::string)((char*)xItem.pvItemData);
		DialogList->Add(dgItem);
	};
}

// ======================== Создание записи ====================================
//strSgDialogItem* TSgDialog::CreateItem(enSgDialogItemType _type, std::vector<std::string>* lines)
//{
//	char buffer[ITEM_DATA_BUF_SIZE];  // Буфер для данных
//	int contentSize = GetContent(ITEM_DATA_BUF_SIZE, buffer, *lines);
//
//	strSgDialogItem* Item = new strSgDialogItem;
//
//	Item->uiID 		 = 0;
//	Item->eType		 = _type;
//	Item->uiSize     = (unsigned int) contentSize;
//	Item->pvItemData = buffer;
//
//	return Item;
//}

////strSgDialogItem* TSgDialog::CreateItem(enSgDialogItemType _type, std::string _text)
////{
////	strSgDialogItem* Item = new strSgDialogItem;
////
////	Item->uiID 		 = 1;
////	Item->eType 	 = _type;
////	Item->uiSize 	 = _text.length();  // Размер строки в символах
////	Item->pvItemData = (void*)_text.c_str();  // Указываем на C-строку
////
////	return Item;
////}
//// =============================================================================
//
//// =================== Добавление Записи =======================================
//void TSgDialog::AddItem(strSgDialogItem* xItem)
//{
//	AddArray(xItem);
//	delete xItem;
//	bFileChangesUnsaved = true;
//}
//
//void TSgDialog::AddItem(enSgDialogItemType _type, std::vector<std::string>* lines)
//{
//	strSgDialogItem* _item = CreateItem(_type, lines);
//	if(_item == nullptr) { return; }
//	AddItem(_item);
//}
//
//void TSgDialog::AddItem(enSgDialogItemType _type, std::string _text)
//{
//	strSgDialogItem* _item = CreateItem(_type, _text);
//	if(_item == nullptr) { return; }
//	AddItem(_item);
//}
// ===================! Добавление Записи ======================================

// Add data to array
void TSgDialog::AddArray(strSgDialogItem* newItem)
{
	int i = 0;
	if (iSgDialogAddItem (&xSgDialogCB, 0, newItem)){
		for (i = 0; i < iBufFileDataSize; i ++)	{
			if (i >= VIRTUAL_FILE_SIZE) break;
			ucReadFileData[i] = ucBufFileData[i];
		};
		iReadFileDataSize = i;

		ui = newItem->uiSize;
		if (ui > (ITEM_DATA_BUF_SIZE - 1)) ui = (ITEM_DATA_BUF_SIZE - 1);
		ucItemDataBuf[ui] = 0;
	};

	bFileChangesUnsaved = true;
}


// Save to file
//void TSgDialog::Save()
//{
//	FILE *pxFile;
//	int i;
//
//	pxFile = fopen(asCurrentFileName.c_str(), "wb");
//	if (pxFile == NULL)	{
//		throw std::runtime_error("open");
//		return;
//	};
//
//	i = fwrite(ucReadFileData, sizeof(unsigned char), iReadFileDataSize, pxFile);
//	fclose(pxFile);
//	if (i != iReadFileDataSize)	{
//		throw std::runtime_error("write");
//		return;
//	};
//
//	bFileChangesUnsaved = false;
//}

// Получить текст из TMemo
//int TSgDialog::iGetContent(int iContentMaxSize, TMemo* ItemTextMemo)
//{
//	int iSize = 0;
//	int i, k, iLine;
//
//	if (ucItemDataBuf == 0) return -1;
//	for (iLine = 0; iLine < ItemTextMemo->Lines->Count; iLine ++)
//	{
//		if (iSize >= iContentMaxSize) break;
//		k = ItemTextMemo->Lines->Strings[iLine].Length();
//		if (k > (iContentMaxSize - iSize)) k = (iContentMaxSize - iSize);
//		for (i = 0; i < k; i ++) ucItemDataBuf[iSize + i] = *(ItemTextMemo->Lines->Strings[iLine].c_str() + i);
//		iSize += k;
//		if ((iSize + 3) > iContentMaxSize) break;
//		if (iLine >= (ItemTextMemo->Lines->Count - 1)) break;
//
//		ucItemDataBuf[iSize ++] = 0x0D;
//		ucItemDataBuf[iSize ++] = 0x0A;
//	};
//
//	return iSize;
//};

// Получить текст из vectors
//int TSgDialog::GetContent(int iContentMaxSize, char* content, const std::vector<std::string>& lines)
//{
//	int iSize = 0;
//	int i, k, iLine;
//	if (content == nullptr || lines.empty()) return -1;
//
//	for (iLine = 0; iLine < lines.size(); iLine++) {
//		if (iSize >= iContentMaxSize) break;
//
//		// Получаем длину строки
//		k = lines[iLine].length();
//
//		// Если текущая строка слишком длинная для оставшегося места, обрезаем
//		if (k > (iContentMaxSize - iSize)) k = (iContentMaxSize - iSize);
//
//		// Копируем строку в content
//		for (i = 0; i < k; i++)
//		{
//			content[iSize + i] = lines[iLine][i];
//		}
//		iSize += k;
//
//		// Если есть место для символов новой строки, добавляем их
//		if ((iSize + 2) <= iContentMaxSize)  // 2 байта для новой строки
//		{
//			content[iSize++] = 0x0D;  // Carriage Return
//			content[iSize++] = 0x0A;  // Line Feed
//		}
//	}
//
//	return iSize;
//}

//TSgDialog::~TSgDialog()
//{
//	delete Item;
//}

// Очистить все данные
//void TSgDialog::Clear() {
//	int i;
//	iSgDialogCreateNewDialog (&xSgDialogCB, 0);
//	for (i = 0; i < iBufFileDataSize; i ++) {
//		if (i >= VIRTUAL_FILE_SIZE) break;
//		ucReadFileData[i] = ucBufFileData[i];
//	};
//	iReadFileDataSize = i;
//}

//TODO Перенести вывод в файл формы
//void TSgDialog::FillTable(enSgDialogItemType etype, TStringGrid* table)
//{
//	if(table == nullptr) { return; }
//	strSgDialogItem xItem;
//
//	int i = 0;
//
//	table->RowCount=0;
//	if (iSgDialogReadItemsInit(&xSgDialogCB, 0, &xReadHandle) < 0) return;
//	while (iSgDialogReadNextItem     (&xSgDialogCB, 0, &xReadHandle, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) > 0)
//	{
//		table->RowCount = i+1;
//		if (xItem.eType != etype) continue;
//		sprintf(cText, "%08X", xItem.uiID);
//		table->Cells[0][i] =  cText;
//
//
//		ui = xItem.uiSize;
//		if (ui > (ITEM_DATA_BUF_SIZE - 1)) ui = (ITEM_DATA_BUF_SIZE - 1);
//		ucItemDataBuf[ui] = 0;
//		table->Cells[1][i] =  (std::string)((char*)xItem.pvItemData);
//
//		i++;
//	};
//
//
//	if(table->RowCount > 18) {
//		table->RowCount = i;
//	} else {
//	   table->RowCount = 18;
//	}
//}

// Получение списка по типу
TSgDialogList* TSgDialog::getItems(enSgDialogItemType etype)
{
	if (iSgDialogReadItemsInit(&xSgDialogCB, 0, &xReadHandle) < 0) return nullptr;
	DialogList->Clear();
	strSgDialogItem xItem;
	int i = 0;
	while (iSgDialogReadNextItem     (&xSgDialogCB, 0, &xReadHandle, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) > 0)
	{
		if (xItem.eType != etype) continue;

		ui = xItem.uiSize;
		if (ui > (ITEM_DATA_BUF_SIZE - 1)) ui = (ITEM_DATA_BUF_SIZE - 1);
		ucItemDataBuf[ui] = 0;

		TSgDialogItem dgItem;
		dgItem.Type = etype;
		sprintf(cText, "%08X", xItem.uiID);
//        dgItem.Id   = Format("%08X", xItem.uiID);
		dgItem.Id = cText;
		if(etype == SDIT_DIALOG) {
			int questID = getIDQestionByItem(xItem);
//			dgItem.Id   = Format("%08X", questID);
			dgItem.Text = getNameByID(questID);
		} else {

			dgItem.Text = (std::string)((char*)xItem.pvItemData);
		}

		DialogList->Add(dgItem);
		i++;
	};

	return DialogList;
}

// Поиск записи по ID
strSgDialogItem TSgDialog::getItemByID(std::string id)
{
	strSgDialogItem xItem;
	xItem.eType  = SDIT_EMPTY;
	xItem.uiSize = 0;
	xItem.uiID   = 0;
	xItem.pvItemData = nullptr;

	if (sscanf(id.c_str(), "%08X", &ui) != 1) return xItem;
	if (iSgDialogReadItem(&xSgDialogCB, 0, ui, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) != 1) xItem.uiSize = 0;

	return xItem;
}

// Поиск записи по ID (INT)
strSgDialogItem TSgDialog::getItemByID(int id)
{
//   return getItemByID(Format("%08X",id));
	char buffer[50];  // Буфер для строки
    sprintf(buffer, "%08X", id);  // Форматируем id в строку с ведущими нулями в шестнадцатеричной системе
    return getItemByID(buffer);  // Передаем отформатированную строку в getItemByID
}

enSgDialogItemType TSgDialog::getTypeByID(std::string id)
{
	strSgDialogItem xItem;
	xItem.eType  = SDIT_EMPTY;
	xItem.uiSize = 0;
	xItem.uiID   = 0;
	xItem.pvItemData = nullptr;

	if (sscanf(id.c_str(), "%08X", &ui) != 1) return SDIT_EMPTY;
	if (iSgDialogReadItem(&xSgDialogCB, 0, ui, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) != 1) xItem.uiSize = 0;

	return xItem.eType;
}

enSgDialogItemType TSgDialog::getTypeByID(int id)
{
//	return getTypeByID(Format("%08X",id));
	char buffer[50];  // Буфер для строки
    sprintf(buffer, "%08X", id);  // Форматируем id в строку с ведущими нулями в шестнадцатеричной системе
    return getTypeByID(buffer);  // Передаем отформатированную строку в getItemByID
}

std::string TSgDialog::getTypeLabel(int id)
{
//   return getTypeLabel(getTypeByID(id));
}

// Возвращаем имя по ID
std::string TSgDialog::getNameByID(std::string id)
{
	strSgDialogItem xItem;
	xItem.eType  = SDIT_EMPTY;
	xItem.uiSize = 0;
	xItem.uiID   = 0;
	xItem.pvItemData = nullptr;

	if (sscanf(id.c_str(), "%08X", &ui) != 1) return "-";
	if (iSgDialogReadItem(&xSgDialogCB, 0, ui, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) != 1) xItem.uiSize = 0;

	std::string result = "";


//	result = (std::string)((char*)xItem.pvItemData);
//	if (xItem.uiSize > 0 && result.length() > xItem.uiSize) {
//		result = result.SubString(1, xItem.uiSize);  // Обрезаем строку по размеру
//	}

	result = (std::string)((char*)xItem.pvItemData);
	if (xItem.uiSize > 0 && result.length() > xItem.uiSize) {
	    result = result.substr(0, xItem.uiSize);  
	}


	if(xItem.eType == SDIT_DIALOG) {
		int QuestID = getIDQestionByIDItem(xItem.uiID);
		result = getNameByID(QuestID);
	}

	return result;
}

std::string TSgDialog::getNameByItem(strSgDialogItem& xItem)
{

	if (iSgDialogReadItem(&xSgDialogCB, 0, ui, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) != 1) xItem.uiSize = 0;


//	std::string result = "";
//	result = (std::string)((char*)xItem.pvItemData);
//	if (xItem.uiSize > 0 && result.length() > xItem.uiSize) {
//		result = result.SubString(1, xItem.uiSize);  // Обрезаем строку по размеру
//	}

	std::string result = "";
	result = (std::string)((char*)xItem.pvItemData);
	if (xItem.uiSize > 0 && result.length() > xItem.uiSize) {
	    result = result.substr(0, xItem.uiSize);  // Обрезаем строку по размеру
	}


	if (xItem.eType == SDIT_DIALOG) {
		int QuestID = getIDQestionByIDItem(xItem.uiID);
		result = getNameByID(QuestID);
	}

	return result;
}

// Возвращаем имя по ID (Int)
std::string TSgDialog::getNameByID(int id)
{
	if(id < 0 ) {
		std::cout << "Error get name by id:" << id <<std::endl;
		return "";
	}
	
//	return getNameByID(Format("%08X", id));
    char buffer[50];  // Буфер для строки
    sprintf(buffer, "%08X", id);  // Форматируем id в строку с ведущими нулями в шестнадцатеричной системе
    return getNameByID(buffer);  // Передаем отформатированную строку в getItemByID	
}

int TSgDialog::getIntID(std::string id)
{
	strSgDialogItem xItem;
	xItem.eType  = SDIT_EMPTY;
	xItem.uiSize = 0;
	xItem.uiID   = 0;
	xItem.pvItemData = nullptr;

	if (sscanf(id.c_str(), "%08X", &ui) != 1) return -1;
	if (iSgDialogReadItem(&xSgDialogCB, 0, ui, &xItem, ucItemDataBuf, ITEM_DATA_BUF_SIZE) != 1) xItem.uiSize = 0;

	return xItem.uiID;
}

// Удаление записи по ID
//void TSgDialog::DeleteItem(std::string id)
//{
//	strSgDialogItem xItem = getItemByID(id);
//	if (iSgDialogRemoveModifyItem (&xSgDialogCB, 0, xItem.uiID, 0) != 1) return;
//	int i;
//	for (i = 0; i < iBufFileDataSize; i ++) {
//		if (i >= VIRTUAL_FILE_SIZE) break;
//		ucReadFileData[i] = ucBufFileData[i];
//	};
//	iReadFileDataSize = i;
//	bFileChangesUnsaved = true;
//}

//void TSgDialog::DeleteItem(int id)
//{
////	DeleteItem(Format("%08X",id));
//	char buffer[50];  // Буфер для строки
//    sprintf(buffer, "%08X", id);  // Форматируем id в строку с ведущими нулями в шестнадцатеричной системе
//    DeleteItem(buffer);  // Передаем отформатированную строку в getItemByID
//}

// Изменение записи
//void TSgDialog::ModifyItem(std::string id, std::string _text)
//{
//	strSgDialogItem xItem = getItemByID(id);
//
//	xItem.uiSize	 = _text.length();
//	xItem.pvItemData = (void*)_text.c_str();
//
//	if (iSgDialogRemoveModifyItem (&xSgDialogCB, 0, xItem.uiID, &xItem) != 1) return;
//	if (xItem.uiSize > (ITEM_DATA_BUF_SIZE - 1)) xItem.uiSize = (ITEM_DATA_BUF_SIZE - 1);
//	ucItemDataBuf[xItem.uiSize] = 0;
//
//    int i;
//	for (i = 0; i < iBufFileDataSize; i ++)	{
//		if (i >= VIRTUAL_FILE_SIZE) break;
//		ucReadFileData[i] = ucBufFileData[i];
//	};
//
//	iReadFileDataSize 	= i;
//	bFileChangesUnsaved = true;
//}

//void TSgDialog::ModifyItem(std::string id,void* ItemData)
//{
//	strSgDialogItem xItem = getItemByID(id);
//
//	xItem.uiSize	 = sizeof(str_xDialogItemDataStruct);
//	xItem.pvItemData = ItemData;
//
//	if (iSgDialogRemoveModifyItem (&xSgDialogCB, 0, xItem.uiID, &xItem) != 1) return;
//	if (xItem.uiSize > (ITEM_DATA_BUF_SIZE - 1)) xItem.uiSize = (ITEM_DATA_BUF_SIZE - 1);
//	ucItemDataBuf[xItem.uiSize] = 0;
//
//    int i;
//	for (i = 0; i < iBufFileDataSize; i ++)	{
//		if (i >= VIRTUAL_FILE_SIZE) break;
//		ucReadFileData[i] = ucBufFileData[i];
//	};
//
//	iReadFileDataSize 	= i;
//	bFileChangesUnsaved = true;
//}


std::string TSgDialog::getTypeLabel(enSgDialogItemType _type)
{
	switch(_type) {
		case  SDIT_ANY : {
			return "ANY";
			break;
		}

		case  SDIT_QUESTION : {
			return "QUESTION";
			break;
		}

		case  SDIT_ANSWER : {
			return "ANSWER";
			break;
		}

		case  SDIT_SOLUTION : {
			return "SOLUTION";
			break;
		}

		case  SDIT_DIALOG : {
			return "DIALOG";
			break;
		}

		case  SDIT_MAX_VALUE : {
			return "MAX_VALUE";
			break;
		}

		case  SDIT_EMPTY : {
			return "EMPTY";
			break;
		}

	}

}


int TSgDialog::getDialogByQustionID(int questionID)
{

}

// Получаем ID вопроса из Item
int TSgDialog::getIDQestionByItem(strSgDialogItem& Item)
{
	strSgDialogItem dialogDetails = getItemByID(Item.uiID); // Получаем детали диалога
	str_xDialogItemDataStruct xDialogItemData = *(str_xDialogItemDataStruct*) dialogDetails.pvItemData;
	return  xDialogItemData.iQuestion;
}

int TSgDialog::getIDQestionByIDItem(int id)
{
	strSgDialogItem dialogDetails = getItemByID(id); // Получаем детали диалога
	str_xDialogItemDataStruct xDialogItemData = *(str_xDialogItemDataStruct*) dialogDetails.pvItemData;
	return  xDialogItemData.iQuestion;
}

//std::string TSgDialog::getLabelQest



// Получаем из диалога название его вопроса
//std::string getDialogDetailsQuestLabel(int id)
//{
//  return getNameByID(id);
//}


//================================! TSgDialog ==================================

//==================================== List ====================================

void TSgDialogList::Add(uint16_t _id, std::string _text, enSgDialogItemType _type) 
{
	TSgDialogItem item;
	item.Id   = _id;
	item.Text = _text;
	item.Type = _type;

	FList->push_back(item);
}
