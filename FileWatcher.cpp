#include "FileWatcher.h"

TQuestList::TQuestList()
{
//	FilesList = new std::vector<int>; 
}

TQuestList::~TQuestList()
{
//	delete FilesList;
}

void TQuestList::setPath(std::string _path)
{
	FPath = _path;	
}



// Fabric method get instance
void TQuestList::setPlatform(bool isWindows) 
{
    // Динамическое создание объекта производного класса в зависимости от платформы
    if (getFiles != nullptr) {
        delete getFiles; // Очистка предыдущего объекта
    }
    if (isWindows) {
        getFiles = new GetFilesWin(); // Создание объекта для Windows
    } else {
        getFiles = new GetFilesLinux(); // Создание объекта для Linux
    }
}
	
// Добавление диалогового файла в список
// Valid! 	
void TQuestList::Add(std::string _filename)
{
	if( !QuestList) { return; }
	for(int i = 0; i < QuestList->size(); i++) { 
		QeustItem quest = QuestList->at(i);
		if(quest.fileName == _filename) {
			return;	
		}
	}
	// Получаем первый вопрос из файла 
	QeustItem _item = Dialog->getFirstQuest(FPath, _filename, SDIT_QUESTION);
	// Если получилось, добавляем в список
	if(_item.isValid) {// Add only valid 
//		printf("+ file:%s, size=%d, valid=%d, Text=%s \n",  _item.fileName.c_str(), _item.size, _item.isValid, _item.FirstQuest.c_str() );
		QuestList->push_back(_item);
	}
}	

// Run scan
void TQuestList::executeScan() 
{
	if( !QuestList) { return; }
    if (getFiles != nullptr) {
    	// сканируем директорию. Find all files of directory
    	getFiles->Scan(FPath);  // Вызов метода Scan() для соответствующего класса (Windows or Linux)
    	FilesList = getFiles->getFilesList(); // Забираем список файлов
	
		for(int i = 0; i < FilesList->size(); i++) {
    		std::string FileName = FilesList->at(i);
    		Add(FileName);
		}
    }
    
	// При удалении из директории
	// Если список файлов в директории изменился, ищем, какой файл был удален
	int FileListCount = FilesList->size();
	int QuestListCount = QuestList->size(); 
	
	if( FileListCount < QuestListCount ) {
//		QuestList->clear();
		for(int i=0;i < QuestListCount; i++ ) {
			QeustItem quest = QuestList->at(i);	
			
			bool find = false;
			for(int y = 0; y < FileListCount; y++) {
				std::string FileName = FilesList->at(y); 
				
				if(quest.fileName == FileName) { 
					find = true;
					break;
				}					
			}
			
			if(!find) {
//				printf("- %d. file:%s, size=%d, valid=%d, Text=%s \n", i, quest.fileName.c_str(), quest.size, quest.isValid, quest.FirstQuest.c_str() );	
				 // Удаляем элемент из QuestList
			    QuestList->erase(QuestList->begin() + i);
			    
			    // После удаления элемента, нужно уменьшить счетчик и продолжить проверку
			    --i;  // Уменьшаем i, так как элементы сдвигаются влево
			    --QuestListCount; // Уменьшаем количество элементов в QuestList
			}
		}
	}

// Fast method
//	int FileListCount = FilesList->size();
//	int QuestListCount = QuestList->size(); 
//	
//	// Создаём unordered_set для быстрого поиска имен файлов
//	std::unordered_set<std::string> fileNamesSet;
//	for (int i = 0; i < FileListCount; i++) {
//	    fileNamesSet.insert(FilesList->at(i));
//	}
//	
//	if (FileListCount < QuestListCount) {
//	    for (auto it = QuestList->begin(); it != QuestList->end(); ) {
//	        QeustItem quest = *it;
//	
//	        // Проверка, если файл не найден в списке файлов
//	        if (fileNamesSet.find(quest.fileName) == fileNamesSet.end()) {
////	            printf("Removing quest: %s \n", quest.fileName.c_str());	
//				printf("- file:%s, size=%d, valid=%d, Text=%s \n",  quest.fileName.c_str(), quest.size, quest.isValid, quest.FirstQuest.c_str() );
//	            // Удаляем элемент из QuestList
//	            it = QuestList->erase(it);  // erase возвращает итератор на следующий элемент
//	        } else {
//	            ++it;  // Если элемент не удаляется, просто двигаем итератор дальше
//	        }
//	    }
//	}
    
}


void TQuestList::StartExecute()
{
	while(!Terminated)
	{
		vPortSleepMs(PollingTime); /*20*/ //PollingTime
		std::lock_guard<std::mutex> lock(mtxFileWatcher);
		executeScan();
	}
}


int TQuestList::Count() { 
	std::lock_guard<std::mutex> lock(mtxFileWatcher);
	return QuestList->size();
}


QeustItem TQuestList::getItem(int index) { 
	std::lock_guard<std::mutex> lock(mtxFileWatcher); 
	
	int count = QuestList->size();
	
	// Проверка индекса
	if (index < 0 || index >= count) { 
	    return QeustItem();  // Возвращаем объект с дефолтными значениями
	}

	return QuestList->at(index);
}


