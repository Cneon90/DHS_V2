#ifndef CMD_INFO_RS_H
#define CMD_INFO_RS_H

#include "Default.h"
#include "Logger.h"
#include "cmd_Base.h"


#define TERMINAL_INFO_SIZE		70
#define SERVER_TIMESTAMP_SIZE	4
#define TERMINAL_ID_SIZE		4

/*
	Получение информации о терминале с RS сервера
*/
class cmd_info_RS: public AbstractCommand
{
	private:
		uint32_t TerminalID; // ID терминала 
		unsigned char* answer = nullptr; 
		uint32_t msgLength	= 0;

	public:
		cmd_info_RS() {}		
		~cmd_info_RS() {}
		
		void setID(uint32_t _id) { 
			TerminalID = _id;
		}
		
		bool bGetCurrentRequest(str_SGP2_ML_Message* pxDstMessage);		
		bool vResponseReceived(const str_SGP2_ML_Message* pxSrcMessage);
		
		
		const unsigned char* getAnswer() const {
			return answer;
		}
		
		uint32_t getAnswerLen() { 
			return msgLength;
		} 
		

};

#endif
