#ifndef CMD_Task_H
#define CMD_Task_H

#include <stdio.h>
#include "cmd_Base.h"

#include <cstring>
#include "Logger.h"

class cmdTask: public AbstractCommandProxy 
{
	public:
	   // Sending the client message to the server
		
	   bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage) {
	   		std::unique_lock<std::mutex> lock(cmdMutex); 
	   		if(ClientRequstMessage == nullptr) { 
	   			return false;
			}
			
 			try {
    			pxDstMessage->usCmd = ClientRequstMessage->usCmd;
				pxDstMessage->ucPar = ClientRequstMessage->ucPar;
			
				usSGP2_ML_MessagePutData(pxDstMessage, ClientRequstMessage->pcData, ClientRequstMessage->usDataSize);	
			} catch(...) { 
				Logger::log(Logger::LogLevel::log_ERROR, "Invalid bGetCurrentRequest");	
			}
			
	   		return true;
	   }
	   
	   bool vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage) {
	   		std::unique_lock<std::mutex> lock(cmdMutex); 
			if (pxSrcMessage == nullptr || pxSrcMessage->pcData == nullptr) {
				Logger::log(Logger::LogLevel::log_ERROR, "Invalid input message");
				return false;
			}
			
	   		ClientResponseMessage = *pxSrcMessage;
//			Logger::log(Logger::LogLevel::log_DEBUG, " + Resp, cmd - 0x%x, pr - %hhu , data -- %s, size - %d, max - %d",  pxSrcMessage->usCmd, pxSrcMessage->ucPar, pxSrcMessage->pcData, pxSrcMessage->usDataSize, pxSrcMessage->usDataMaxSize );
	
			//==== Emergency exit =====
			if(FCount > 10) {
				FCount = 0; 
				Logger::log(Logger::LogLevel::log_ERROR, "Exit TASK cmd - 0x%x, pr - %hhu", pxSrcMessage->usCmd, pxSrcMessage->ucPar);

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
	   			return true;
			}
			
			return false;
	   }
	   
	   str_SGP2_ML_Message* getClientResponseMsg() { 
	   		std::unique_lock<std::mutex> lock(cmdMutex); 
	   		return &ClientResponseMessage;
	   }
	   
	   str_SGP2_ML_Message* getClientRequstMsg() { 
	   		std::unique_lock<std::mutex> lock(cmdMutex); 
	   		return ClientRequstMessage;
	   }

};

#endif
