//---------------------------------------------------------------------------
#ifndef SgDialogH 
#define SgDialogH

//---------------------------------------------------------------------------
#include <vector>
#include <string>
//#include <cstring>
//#include <iostream>
//#include <stdexcept> 

#include "SgDialogLib.h"
#include "..\Logger.h"


//------------------------------------------------------------------------------

//unsigned char ucBufFileData[VIRTUAL_FILE_SIZE];
//int            iBufFileDataSize = 0;
//
//unsigned char ucReadFileData[VIRTUAL_FILE_SIZE];
//int            iReadFileDataSize = 0;
//
//unsigned char ucItemDataBuf[ITEM_DATA_BUF_SIZE];
int  iReadDescriptor     (void* pvCB_Arg,       unsigned char* pcDstData, int iOffset, int iSize);
int  iWriteDescriptor    (void* pvCB_Arg, const unsigned char* pcSrcData, int iOffset, int iSize);
//
static const strSgDialogCB xSgDialogCB = {iReadDescriptor, iWriteDescriptor};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Data class
class TSgDialogItem {
	public:
		std::string 			Id;
		std::string 			Text;
		enSgDialogItemType 		Type;

		unsigned int getID() { 
			unsigned int ui;
			if (sscanf(Id.c_str(), "%08X", &ui) != 1) return 0;
			
			return ui;
		}
		
};



// list
// All records file
class TSgDialogList {
	private:
		std::vector<TSgDialogItem>* FList = new std::vector<TSgDialogItem>;
	public:

		void Clear() { FList->clear(); }
		void Add(TSgDialogItem item) { FList->push_back(item); }
		void Add(uint16_t _id, std::string _text, enSgDialogItemType _type);

		TSgDialogItem getFirst() {
			if(!FList) {
				TSgDialogItem temp;
				temp.Id = "-1";
				return temp;
			}
			if (FList->size() != 0) {
				return FList->at(0);
			}
		}
		
		TSgDialogItem getDialogByIndex(unsigned int _index) {
		    if (!FList || _index >= FList->size()) {
		        TSgDialogItem temp;
		        temp.Id = "-1";
		        return temp;
		    }
		    
		    return FList->at(_index);
		}

         // Получаем ID первого вопроса (или -1 если список пуст)
//		int getFirstQuestID() {
//			TSgDialogItem temp = FList->at(0);
//			if (FList->size() != 0) {
//				return temp.Id;
//			} else {
//                return -1;
//            }
//        }

		std::vector<TSgDialogItem>* getList() { return FList; }
		
		int Count() { 
			return FList->size();
		}
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class TSgDialog {
	private:
		enSgDialogItemType eCurrentItemType;
		std::string asCurrentFileName = "";
		strSgDialogReadListHandle xReadHandle;

		void AddArray(strSgDialogItem* newItem);

		char cText[16];
		unsigned int ui;
		bool bFileChangesUnsaved = false;

		TSgDialogList* DialogList = new TSgDialogList();
//		int iGetContent(int iContentMaxSize, TMemo* ItemTextMemo);
//		int GetContent(int iContentMaxSize, char*  content, const std::vector<std::string>& lines);

//		std::vector<strSgDialogItem>* DialogList = new std::vector<strSgDialogItem>;
	public:
		strSgDialogItem getItemByID(std::string id);
		strSgDialogItem getItemByID(int id);

		//----Current-path-----
		void setCurrentPath(std::string _path) { 
			asCurrentFileName = _path; 
		}
		
		std::string getCurrentPath()           { return asCurrentFileName;  }
		//---------------------
		bool init();
		void ReadItems(enSgDialogItemType etype);  // Read file

		TSgDialogList* getItems(enSgDialogItemType etype);

		std::string getNameByID(std::string id); // Возвращаем имя по ID
        std::string getNameByID(int id);
		int getIntID(std::string id);
		enSgDialogItemType getTypeByID(std::string id);
		enSgDialogItemType getTypeByID(int id);

        TSgDialogList* getList() { return DialogList; }

//		strSgDialogItem* CreateItem(enSgDialogItemType _type, std::vector<std::string>* lines);
//		strSgDialogItem* CreateItem(enSgDialogItemType _type, std::string _text);
//
//		void AddItem(strSgDialogItem* xItem);
//		void AddItem(enSgDialogItemType _type, std::vector<std::string>* lines);
//		void AddItem(enSgDialogItemType _type, std::string _text);
//
//		void DeleteItem(std::string id);
//		void DeleteItem(int id);
//
//		void ModifyItem(std::string id, std::string text);
//        void ModifyItem(std::string id,void* ItemData);

//		void Save(); // Save file
//		bool isChange() { return bFileChangesUnsaved; }
//		void Clear();
//		~TSgDialog();

		std::string getTypeLabel(enSgDialogItemType _type); //Вернуть строковое название типа (по его ключу)
		std::string getTypeLabel(int id);                   // Вернуть название типа объекта по ID

		int getDialogByQustionID(int questionID);

		int getIDQestionByItem(strSgDialogItem& Item);
		int getIDQestionByIDItem(int id);
        std::string getNameByItem(strSgDialogItem& xItem);
};

#endif






