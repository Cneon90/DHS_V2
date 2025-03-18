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
    // ������������ �������� ������� ������������ ������ � ����������� �� ���������
    if (getFiles != nullptr) {
        delete getFiles; // ������� ����������� �������
    }
    if (isWindows) {
        getFiles = new GetFilesWin(); // �������� ������� ��� Windows
    } else {
        getFiles = new GetFilesLinux(); // �������� ������� ��� Linux
    }
}
	
// ���������� ����������� ����� � ������
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
	// �������� ������ ������ �� ����� 
	QeustItem _item = Dialog->getFirstQuest(FPath, _filename, SDIT_QUESTION);
	// ���� ����������, ��������� � ������
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
    	// ��������� ����������. Find all files of directory
    	getFiles->Scan(FPath);  // ����� ������ Scan() ��� ���������������� ������ (Windows or Linux)
    	FilesList = getFiles->getFilesList(); // �������� ������ ������
	
		for(int i = 0; i < FilesList->size(); i++) {
    		std::string FileName = FilesList->at(i);
    		Add(FileName);
		}
    }
    
	// ��� �������� �� ����������
	// ���� ������ ������ � ���������� ���������, ����, ����� ���� ��� ������
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
				 // ������� ������� �� QuestList
			    QuestList->erase(QuestList->begin() + i);
			    
			    // ����� �������� ��������, ����� ��������� ������� � ���������� ��������
			    --i;  // ��������� i, ��� ��� �������� ���������� �����
			    --QuestListCount; // ��������� ���������� ��������� � QuestList
			}
		}
	}

// Fast method
//	int FileListCount = FilesList->size();
//	int QuestListCount = QuestList->size(); 
//	
//	// ������ unordered_set ��� �������� ������ ���� ������
//	std::unordered_set<std::string> fileNamesSet;
//	for (int i = 0; i < FileListCount; i++) {
//	    fileNamesSet.insert(FilesList->at(i));
//	}
//	
//	if (FileListCount < QuestListCount) {
//	    for (auto it = QuestList->begin(); it != QuestList->end(); ) {
//	        QeustItem quest = *it;
//	
//	        // ��������, ���� ���� �� ������ � ������ ������
//	        if (fileNamesSet.find(quest.fileName) == fileNamesSet.end()) {
////	            printf("Removing quest: %s \n", quest.fileName.c_str());	
//				printf("- file:%s, size=%d, valid=%d, Text=%s \n",  quest.fileName.c_str(), quest.size, quest.isValid, quest.FirstQuest.c_str() );
//	            // ������� ������� �� QuestList
//	            it = QuestList->erase(it);  // erase ���������� �������� �� ��������� �������
//	        } else {
//	            ++it;  // ���� ������� �� ���������, ������ ������� �������� ������
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
	
	// �������� �������
	if (index < 0 || index >= count) { 
	    return QeustItem();  // ���������� ������ � ���������� ����������
	}

	return QuestList->at(index);
}


