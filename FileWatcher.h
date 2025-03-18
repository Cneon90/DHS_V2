#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <unordered_set>

#include "Logger.h"
#include "ThreadBase.h"
#include "GetFilesPort.h"

//#include "SgDialog\SgDialog.h"
#include "DialogAPI.h"

/*
	Получение списка файлов из директории "Questions"
	Создание общего списка вопросов 
	
	табличка вида:
	Filename   |   isValid    |   ferstQuest  | 
*/

class TQuestList: public ThreadBase
{
	private:
		void StartExecute() override; 
		std::mutex 	mtxFileWatcher;
		std::string FPath;
		GetFilesAbstract* getFiles = nullptr; 
		
		DialogAPI* Dialog = new DialogAPI();
		std::vector<std::string>* 	FilesList = nullptr; 
		std::vector<QeustItem>* 	QuestList = new std::vector<QeustItem>();
	public:
		TQuestList();
		~TQuestList();
		//
		void setPath(std::string _path);
		//set Win or Linux
		void setPlatform(bool isWindows);
		// Run scan
	    void executeScan();	
	    // Добавляем только валидные файлы
		void Add(std::string _filename);	
		// Отдать список 
		std::vector<QeustItem>* getQuestList() { 
			std::lock_guard<std::mutex> lock(mtxFileWatcher);
			return QuestList;
		}
		

		int Count();
		QeustItem getItem(int index);
		
		std::string getCurrentFile() { 
//			std::lock_guard<std::mutex> lock(mtxFileWatcher);
			QeustItem questOne = getItem(0);
			
			return questOne.fileName;	
		}
};




#endif
