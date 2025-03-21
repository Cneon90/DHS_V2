#ifndef GETFILESPORT_H
#define GETFILESPORT_H

#include <iostream>
#include <direct.h>
#include <cstring>
#include <vector>

#include "Logger.h"

#if _WIN32
	#include <Windows.h>
#else 
	
#endif	

class GetFilesAbstract {	
	protected:
		std::vector<std::string>* fsList = new std::vector<std::string>();
	
	public:
		virtual void Scan(std::string FPath) = 0;
		
		std::vector<std::string>* getFilesList() {
			return fsList; 	
		}	
};

class GetFilesWin: public GetFilesAbstract { 
	void Scan(std::string FPath) override;		
}; 

class GetFilesLinux: public GetFilesAbstract { 
	void Scan(std::string FPath) override;		
}; 

#endif
