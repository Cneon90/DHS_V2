#ifndef DEFAULT_H
#define DEFAULT_H

#define RS_SERVERS_FILE_NAME						"RelayServers.cfg"

#define KEY_DELETE_CMD {0xFF,0xFE,0x10,0xAA, 0xAB}
#define ERROR_NOT_FILE {0xFF,0xEE,0x40,0x04, 0xEE}

// ---- Do not edit  ----------------------------------------------------------------------------
#define  CMD_PING  		 		0x0000                                                 // PING
#define  CMD_INFO  		 		0x0001                                                 // SERVER INFO
#define  CMD_STATE  	 		0x7000                                                 // SERVER STATE
#define  CMD_TERM_LIST_RS  		0x7001                                                 // List of terminals on RS servers
#define  CMD_TERM_LIST_DHS 		0x7901                                                 // List of terminals on DHS servers


#define  CMD_GROUP_LIST  		0x7002                                                 // List of Groups 
#define  CMD_TERM_LIST_DS       0x8000                                                 // List of terminals on DS servers

#define  CMD_DIR         		0x5000                                                 // File Manager

#define  CMD_SYNC  		 		0x8001                                                 // Синхронизация терминала
#define  CMD_CAN         		0x8100                                                 // CAN List
#define  CMD_LOG         		0x8110                                                 // Log list
#define  CMD_SET         		0x81F0                                                 // Can Settings
#define  CMD_SRH         		0x81E0                                                 // Data search
#define  CMD_SYS         		0x1000                                                 // System
//----- MY ---------------------------------------------------------------------
#define  CMD_SES         		0x7700                                                 // Session DS on Diagnostic_hub_server
#define  CMD_RS          		0x7701                                                 // Relay Server (CRUD)
#define  CMD_DIALOG        		0x9900                                                 // Dialog help desk

#define  CMD_ADMIN       		0xA000                                                 // Admin Panel
#define  CMD_REQRIG      		0xA001                                                 // Запрос права
#define  CMD_CARD_TABLE  		0x81A0    
// ---- Do not edit  ----------------------------------------------------------------------------


#endif
