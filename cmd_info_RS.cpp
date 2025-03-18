#include "cmd_info_RS.h"


/*
 ���������� ������ �� 1 �������� (�� ID)
*/
bool cmd_info_RS::bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage) 
{
	std::unique_lock<std::mutex> lock(cmdMutex); 
	
	pxDstMessage->usCmd = 0x7001;
	pxDstMessage->ucPar = 0x01;
	
	usSGP2_ML_MessagePutInt(pxDstMessage,   TerminalID	); // Start ID
	usSGP2_ML_MessagePutInt(pxDstMessage,   TerminalID	); // FID
	usSGP2_ML_MessagePutInt(pxDstMessage,   0xFFFFFFFF	); // FIDMSK
	usSGP2_ML_MessagePutInt(pxDstMessage,   0xFFFFFFFE	); // FGID
	usSGP2_ML_MessagePutShort(pxDstMessage, 1 			); // Count   CNT	
	
	return true;			
}


/*
	��������� ���������� � ���������
*/
bool cmd_info_RS::vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage)
{
	std::unique_lock<std::mutex> lock(cmdMutex); 

	if(pxSrcMessage->usCmd == 0x7001) { 
		if (answer == nullptr) {
            answer = new uint8_t[TERMINAL_INFO_SIZE];
        }

//		if(pxSrcMessage->usDataSize >= TERMINAL_INFO_SIZE) {
		// �� �������� ����� �������									
		msgLength = pxSrcMessage->usDataSize - SERVER_TIMESTAMP_SIZE;// ������������ 
		
		std::memcpy(answer, pxSrcMessage->pcData+SERVER_TIMESTAMP_SIZE, msgLength); 	
//		}       
	} 
	
	return true;
}
