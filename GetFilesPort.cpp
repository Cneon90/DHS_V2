#include "GetFilesPort.h"

// windows
void GetFilesWin::Scan(std::string FPath) 
{
	if(FPath == "") {
		Logger::log(Logger::LogLevel::log_ERROR, "Error opening directory: Empty path ");	
		return;
	}
	
	std::string search_path = FPath + "\\*";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        Logger::log(Logger::LogLevel::log_ERROR, "Error opening directory %s", FPath.c_str());
        return;
    }

	fsList->clear();
    do {
//      adding "." and ".."    	
//      std::cout << find_file_data.cFileName << std::endl;
//		fsList->push_back(find_file_data.cFileName);

		// Исключаем "." и ".."
        if (find_file_data.cFileName[0] != '.' && find_file_data.cFileName[1] != '.') {
        	if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { // isFILE
            	fsList->push_back(find_file_data.cFileName);
            }
        }
    } while (FindNextFile(hFind, &find_file_data));

    FindClose(hFind);	
}

//Linux
void GetFilesLinux::Scan(std::string FPath)
{
    DIR* dir = opendir(FPath.c_str());
    if (dir == nullptr) {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return;
    }

    struct dirent* entry;
    fsList->clear();
    while ((entry = readdir(dir)) != nullptr) {
		fsList->push_back(entry->d_name);
    }

    closedir(dir);
}
