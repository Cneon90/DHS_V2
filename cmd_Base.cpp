#include "Cmd_Base.h"


bool AbstractCommand::isCommand(const str_SGP2_ML_Message* pxSrcRequestMessage) 
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if ((pxSrcRequestMessage->usCmd == xMessage.usCmd) && (pxSrcRequestMessage->ucPar == xMessage.ucPar))  {
		return true;
	}

	return false;	
}


bool AbstractCommand::isCmd(unsigned short _Cmd)
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if (_Cmd == xMessage.usCmd)   {
		return true;
	}

	return false;		
}


bool AbstractCommand::isPar(unsigned char _Par)
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if (_Par == xMessage.usCmd)   {
		return true;
	}

	return false;
}



unsigned short AbstractCommand::getCmd() 
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if (this == nullptr){
		return 0; 
	}
	
	return xMessage.usCmd;
};

unsigned char AbstractCommand::getPar()
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if (this == nullptr){ 
		return 0; 
	}
	
	return xMessage.ucPar;
};



void AbstractCommand::setMessage(str_SGP2_ML_Message* pxDstMessage) 
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	pxDstMessage->usCmd = xMessage.usCmd;
	pxDstMessage->ucPar = xMessage.ucPar;
}


// =================== TIME ==================================

uint32_t AbstractCommandTime::getTimeUnix() {
	std::unique_lock<std::mutex> lock(cmdMutex); 
	return (static_cast<uint32_t>   (FTS[3]) << 24) |
			 (static_cast<uint32_t> (FTS[2]) << 16) |
			 (static_cast<uint32_t> (FTS[1]) << 8)  |
			  static_cast<uint32_t> (FTS[0]);
};


std::string AbstractCommandTime::getTimeStr(const std::string& _format)
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	uint32_t unixTime = getTimeUnix();
    time_t rawtime = static_cast<time_t>(unixTime);
    struct tm *timeinfo = std::gmtime(&rawtime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), _format.c_str(), timeinfo);

    return std::string(buffer);
};





