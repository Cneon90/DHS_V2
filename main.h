//
// Created by Kirill on 18.03.2025.
//

#ifndef DHS_MAIN_H
#define DHS_MAIN_H

#endif //DHS_MAIN_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <dirent.h>
//#include <pthread.h>
//#include <sched.h>
#include <ctime>
#include <iostream>
#include <future>
#include <chrono>


//#include "SgDialog.h"

#if _WIN32
#include "port_windows.h"
#include <Windows.h>
#elif __linux__
#include "port_linux.h"
#else
	#error "UNKNOWN TARGET OS"
#endif

#include <vector>
#include "SGP2/SGP2.h"
#include "help_functions.h"
#include <iostream>
#include "ProtocolsLib.h"

#include "thClient.h"
//#include "thServer.h"
#include "thServer2.h"
#include "Accounts.h"
#include "Arg_parser.h"
#include "TaskList.h"
#include "TaskExecutor.h"
#include "Logger.h"
//ExtLib
#include "extLib\argh.h"
#include "RelayServers.h"

#include "SgListLib.h"
#include "SgQueue.h"
#include "SgDialog\SgDialog.h"

#include "FileWatcher.h"

/*******************************************************************************
                                   DEFINES
*******************************************************************************/
#define APP_VERSION_H                      			1
#define APP_VERSION_L                     			12

#define APP_DATE_D                         			28
#define APP_DATE_M                         			02
#define APP_DATE_Y                         			25

#define CONNECTIONS_LOGS_CATALOG_NAME      			"ConnectionsLog"
#define QUESTIONS_CATALOG_NAME						"Questions"
#define QUESTIONS_POLLING_TIME						2000

#define SGP2_FS_VIRTUAL_DIRS_COUNT                  1
#define SGP2_FS_VIRTUAL_DIR_CONNECTIONS_LOGS        CONNECTIONS_LOGS_CATALOG_NAME
#define SGP2_FS_VIRTUAL_DIR_CONNECTIONS_LOGS_INDEX  0



#define  CMD_BUF_SIZE  						  		1024
#define  DEFAULT_PORT								9999

#include "crow.h"
/*******************************************************************************


/*******************************************************************************
/*******************************************************************************
                                   VARIABLES
*******************************************************************************/

//unsigned int uiAccotuntsCount;
//str_AccountHandle xAccount[MAX_ACCOUNTS_COUNT];
unsigned int  uiStartTS;

unsigned char bDaemonMode = 0;
char cAppRootDir[512] = "";
char cLogDir[255]; // ?????? ??????? ??? ???????? ????
char cConnectionsLogsDir[512] = "";

char* pcVirtualDirRealLocation[SGP2_FS_VIRTUAL_DIRS_COUNT];

#if _WIN32
bool isWindows = true;
#else
bool isWindows = false;
#endif