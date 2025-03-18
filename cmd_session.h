#ifndef CMD_SESSION_H
#define CMD_SESSION_H

#include "cmd_Base.h"
#include "Logger.h"


class cmdPingProxy: public AbstractCommandProxy 
{
	private:
		str_SGP2_ML_Message* FMessageResponse;
		bool auth = false;
//		uint8_t FCount = 0;
//		std::mutex cmdMutex;    
		
	public:
		cmdPingProxy() {
			std::unique_lock<std::mutex> lock(cmdMutex); 
			xMessage.usCmd = CMD_PING;
			xMessage.ucPar = 0x00;
			
			FMessageResponse = new str_SGP2_ML_Message;// = *FTaskCMD->getClientResponseMsg();
	   };
	   
	   ~cmdPingProxy() {
	   		delete FMessageResponse;
	   }
	   

	   
	   bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage) {
	   		std::unique_lock<std::mutex> lock(cmdMutex); 
//	   		std::lock_guard<std::mutex> lock(cmdMutex); 
	   		
	   		pxDstMessage->usCmd = 0x0000;
			pxDstMessage->ucPar = 0;	   		
	   		
	   		usSGP2_ML_MessagePutByte(pxDstMessage, 0xAB);	   		
//	   		Logger::log(Logger::LogLevel::log_DEBUG, " - Reqst, cmd - %hu, pr - %hhu , data -- %s, size - %d, max - %d",  pxDstMessage->usCmd, pxDstMessage->ucPar, pxDstMessage->pcData, pxDstMessage->usDataSize, pxDstMessage->usDataMaxSize );
	
	
	   		return true;
	   }
	   
	   bool vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage) 
	   {
//	   		std::lock_guard<std::mutex> lock(cmdMutex);
			std::unique_lock<std::mutex> lock(cmdMutex); 
	   		
		  	if(pxSrcMessage->pcData[0] == 0xAB) {
		  		auth = true;	
	   	   		return true;		
			} 
//			ClientResponseMessage = *pxSrcMessage;
//			Logger::log(Logger::LogLevel::log_DEBUG, " + Resp, cmd - %hu, pr - %hhu , data -- %s, size - %d, max - %d",  pxSrcMessage->usCmd, pxSrcMessage->ucPar, pxSrcMessage->pcData, pxSrcMessage->usDataSize, pxSrcMessage->usDataMaxSize );
		   
		    FCount++;
			if(FCount > 20) { 
				FCount = 0;
				Logger::log(Logger::LogLevel::log_ERROR, "Exit TASK cmd - 0x%x, pr - %hhu", pxSrcMessage->usCmd, pxSrcMessage->ucPar);
				auth = false;
				return true;
			}
		   
	   	    return false;	 		
	   }
	   
	   str_SGP2_ML_Message* getClientResponseMsg() {
//	   		std::lock_guard<std::mutex> lock(cmdMutex);
			std::unique_lock<std::mutex> lock(cmdMutex); 
//			str_SGP2_ML_Message* _msg = new str_SGP2_ML_Message;// = *FTaskCMD->getClientResponseMsg();
			
			FMessageResponse->pcData = new unsigned char[6]; 
//	   		FMessageResponse->usCmd = 0x7700;
	   		
			FMessageResponse->usDataSize = 0; 
			FMessageResponse->usDataMaxSize = 6;    
	   		
	   		usSGP2_ML_MessagePutInt(FMessageResponse, time(NULL));
	   		if (auth) { 
	   			FMessageResponse->ucPar = 0;
				usSGP2_ML_MessagePutByte(FMessageResponse, 0xFE); // Start session OK
			} else { 
				FMessageResponse->ucPar = 1;
				usSGP2_ML_MessagePutByte(FMessageResponse, 0xEE); // ERROR session OK
			}
		
//			delete FMessageResponse->pcData;		
			return FMessageResponse;
	   }

};


#endif
