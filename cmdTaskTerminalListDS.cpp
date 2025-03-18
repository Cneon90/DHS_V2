#include "cmdTaskTerminalListDS.h"

bool cmdTaskTerminalListDS::bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage) 
{
	if(ClientRequstMessage == nullptr) { 
	return false;
}
	std::unique_lock<std::mutex> lock(cmdMutex); 
try {
	pxDstMessage->usCmd = ClientRequstMessage->usCmd;
	pxDstMessage->ucPar = ClientRequstMessage->ucPar;

	usSGP2_ML_MessagePutData(pxDstMessage, ClientRequstMessage->pcData, ClientRequstMessage->usDataSize);	
} catch(...) { 
	Logger::log(Logger::LogLevel::log_ERROR, "Invalid bGetCurrentRequest");	
}

//	Logger::log(Logger::LogLevel::log_DEBUG, " - Reqest, cmd - 0x%x, pr - %hhu , data -- %s, size - %d, max - %d",  pxDstMessage->usCmd, pxDstMessage->ucPar, pxDstMessage->pcData, pxDstMessage->usDataSize, pxDstMessage->usDataMaxSize );


	return true;
}

bool cmdTaskTerminalListDS::vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage) 
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	if (pxSrcMessage == nullptr || pxSrcMessage->pcData == nullptr) {
		Logger::log(Logger::LogLevel::log_ERROR, "Invalid input message");
		return false;
	}

	ClientResponseMessage = *pxSrcMessage;
//	Logger::log(Logger::LogLevel::log_DEBUG, " + Resp, cmd - 0x%x, pr - %hhu , data -- %s, size - %d, max - %d",  pxSrcMessage->usCmd, pxSrcMessage->ucPar, pxSrcMessage->pcData, pxSrcMessage->usDataSize, pxSrcMessage->usDataMaxSize );

//==== Emergency exit =====
	if(FCount > 10) {
		FCount = 0; 
		Logger::log(Logger::LogLevel::log_ERROR, "Exit TASK cmd - 0x%x, pr - %hhu", pxSrcMessage->usCmd, pxSrcMessage->ucPar);
	
	//			ClientResponseMessage.ucPar  		= 0xEE;
		ClientResponseMessage.pcData 		= new unsigned char[1]; 
		ClientResponseMessage.usDataSize    = 0; 
		ClientResponseMessage.usDataMaxSize = 5;  				
		uint8_t _mass[5] = ERROR_NOT_FILE;
		usSGP2_ML_MessagePutData(&ClientResponseMessage, _mass, sizeof(_mass)); 
		
		return true;
	} else { 
		FCount++;
	}
//  ======================= 

	if(pxSrcMessage->usDataSize > 0) { 
		// ----- PARSE --------------- 
		int MsgLength = pxSrcMessage->usDataSize - 4;
		if(MsgLength < TERMINAL_ITEM_SIZE) {
			return true;
		}

		int TerminalsCount = trunc(MsgLength / TERMINAL_ITEM_SIZE); // Количество терминалов

		uint32_t Skip = 4;
		for (int i = 0; i < TerminalsCount; i++) {
			Terminal_Info_Item terminal_item_ds;
			// ID
			terminal_item_ds.ID = *(uint32_t *)(pxSrcMessage->pcData + Skip);
			Skip += 4;
			// EXTDBID
			terminal_item_ds.EXTDBID = *(uint32_t *)(pxSrcMessage->pcData + Skip);
			Skip += 4;
	
			// Name
			strncpy(terminal_item_ds.TNAME, reinterpret_cast<const char*>(pxSrcMessage->pcData + Skip), TERMINAL_NAME_LENGTH);
			terminal_item_ds.TNAME[TERMINAL_NAME_LENGTH - 1] = '\0'; // ???????????? ????-?????????? ? ?????
			Skip += TERMINAL_NAME_LENGTH;
	
			// Status
			strncpy(terminal_item_ds.STATUS, reinterpret_cast<const char*>(pxSrcMessage->pcData + Skip), TERMINAL_STATUS_LENGTH);
			terminal_item_ds.STATUS[TERMINAL_STATUS_LENGTH - 1] = '\0'; // ???????????? ????-?????????? ? ?????
			Skip += TERMINAL_STATUS_LENGTH;
	
	        // SW
			strncpy(terminal_item_ds.SW_INFO, reinterpret_cast<const char*>(pxSrcMessage->pcData + Skip), TERMINAL_INFO_LENGTH);
			terminal_item_ds.SW_INFO[TERMINAL_INFO_LENGTH - 1] = '\0'; // ???????????? ????-?????????? ? ?????
			Skip += TERMINAL_INFO_LENGTH;
			
			// Добавление терминала 
			if(termList != nullptr) {
				termList->Add(terminal_item_ds);
			}	
		}

		return true;
	}
	
	return false;
}

// указываем список терминалов, куда будем добавлять полученные терминалы
void cmdTaskTerminalListDS::setTerminalList(Terminal_Info_List* _termList) {
	cmdMutex.lock();
	termList = _termList;	
	cmdMutex.unlock();	
}

//void cmdTaskTerminalListDS::SetTerminalInfoList(Manager_ClientData* _md) 
//{ 
//	if(_md != nullptr) 
//	TerminalInfoList = _md->GetTerminalInfoList(); // Пытаемся получить экземпляр	
//}




