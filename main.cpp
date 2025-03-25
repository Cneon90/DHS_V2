#include "main.h"




int main(int argc, char** argv) 
{
	int i;
	char cBuf[CMD_BUF_SIZE];

	//=== Logger =====
	try{
		Logger::copyFormattedDateToArray(cLogDir, sizeof(cLogDir));
		vPortCreateDirectory(cLogDir);
		std::string filePath = std::string(cLogDir) + "\\Init.log";		
	
		if (!Logger::initialize(filePath)) {
        	std::cerr << "Failed to open log file" << std::endl;
        	return 1;
    	}
    	Logger::setLogLevel(Logger::LogLevel::log_DEBUG);
	}	
	catch (...) {
		printf("Error init logger \n");
	}
	
   
	argh::parser cmdl(argv);

 
	if (cmdl[{ "-d", "-D", "-deamon"}]) {
	 	Logger::ConsoleOutOff();
	 	bDaemonMode = true;
	} else {
		Logger::ConsoleOutOn();
		Logger::log(Logger::LogLevel::log_INFO, "Start DEBUG mode");
		bDaemonMode = false;	
	}
	
	
	if (cmdl[{ "-log_debug"}]) {
	 	Logger::setLogLevel(Logger::LogLevel::log_DEBUG);
	 	Logger::log(Logger::LogLevel::log_INFO, "LOG MODE: DEBUG");
	} 
	
	if (cmdl[{ "-log_info"}]) {
	 	Logger::setLogLevel(Logger::LogLevel::log_INFO);
	 	Logger::log(Logger::LogLevel::log_INFO, "LOG MODE: INFO");
	} 
	
	if (cmdl[{ "-log_warning"}]) {
	 	Logger::setLogLevel(Logger::LogLevel::log_WARNING);
	 	Logger::log(Logger::LogLevel::log_INFO, "LOG MODE: WARNING");
	} 
	
	if (cmdl[{ "-log_error"}]) {
	 	Logger::setLogLevel(Logger::LogLevel::log_ERROR);
	 	Logger::log(Logger::LogLevel::log_INFO, "LOG MODE: ERROR");
	} 

	Logger::log(Logger::LogLevel::log_INFO, "--------------Start--------------");
	// --- Parse ARG ------
	if (cmdl[{ "-v", "-version" }]) {
//		printf( "\n\n Version: %d.%02d (%02d.%02d.%d) \n\n", APP_VERSION_H, APP_VERSION_L, APP_DATE_D, APP_DATE_M, APP_DATE_Y );
		printf("\n+-----------------------+\n");
		printf("|   Version Info        |\n");
		printf("+-----------------------+\n");
		printf("|      Version: %d.%02d    |\n", APP_VERSION_H, APP_VERSION_L);
		printf("| Release Date: %02d/%02d/%d|\n", APP_DATE_D, APP_DATE_M, APP_DATE_Y);
		printf("+-----------------------+\n\n");

		return 0;	
	}
	
	if (cmdl[{ "-h", "-help" }]) {
		printf( "\n");
		printf( "  -d - Deamond mode on \n" );
		printf( " --p - set server port (--port=9999)\n" );	 /*DHS.exe -d -log_error --port=9999*/		

		return 0;	
	}
	
    ArgParser parser(argc, argv);
//    std::cout << "App Root Directory: " << parser.getAppRootDir() << std::endl;
//    std::cout << "Daemon Mode: " << (parser.isDaemonMode() ? "Enabled" : "Disabled") << std::endl;
    parser.runAsDaemon();
//  bDaemonMode = parser.isDaemonMode();
    
    // --- Get port ----- 
    int port = parser.getPort();
	if (port <= 0) port = DEFAULT_PORT;
//	printf(" Port %d \n", port);
	Logger::log(Logger::LogLevel::log_INFO, "App port-%d", port);
	//------------------------
	//---Add account --------------------------
		Accounts* AccountList = new Accounts();
		char accFileName[] = "Accounts.cfg";
		AccountList->ReadFileAccount(accFileName); 
//		AccountList->AddAccount("Kirill","123","A"); 
		int _counts  = AccountList->counts();
		Logger::log(Logger::LogLevel::log_INFO, "Load accounts: %d",_counts);
	// ---------------------------------------
	//---Add RS record  --------------------------
	std::vector<TRelayServer> records = RSHandler::readAllFromFile(RS_SERVERS_FILE_NAME);
	Logger::log(Logger::LogLevel::log_INFO, "Load accounts RS: %d", records.size());
	// ---------------------------------------
	// === INIT ====
	// Init SGP FS virtual disks (uninited):
	i = strlen(cConnectionsLogsDir);
	sprintf(&cConnectionsLogsDir[i], "%c%s", PORT_CATALOG_DEVIDER_SYMBOL, SGP2_FS_VIRTUAL_DIR_CONNECTIONS_LOGS); 
	vPortCreateDirectory(cConnectionsLogsDir);
	vPortCreateDirectory(QUESTIONS_CATALOG_NAME);
	pcVirtualDirRealLocation[SGP2_FS_VIRTUAL_DIR_CONNECTIONS_LOGS_INDEX] = cConnectionsLogsDir;
	// ---------------------------------------
//	printf("Start \r\n");
//	printf(" - Create catalog %s", cConnectionsLogsDir+i );
	Logger::log(Logger::LogLevel::log_INFO, "- Create catalog %s ", cConnectionsLogsDir);	
	
	uiStartTS = time(NULL); 
	if (!bDaemonMode) {
//		printf("\nTehnovizor Diagnostic_hub v%u.%02u\nRelease date: %02u.%02u.20%02u\n(c)Tehnovizor LTD, 20%02u\n----------------------------------------------------------------\n\n", APP_VERSION_H, APP_VERSION_L, APP_DATE_D, APP_DATE_M, APP_DATE_Y, APP_DATE_Y);
		Logger::log(Logger::LogLevel::log_INFO, "\nTehnovizor Diagnostic_hub v%u.%02u\nRelease date: %02u.%02u.20%02u\n(c)Tehnovizor LTD, 20%02u\n----------------------------------------------------------------\n\n", APP_VERSION_H, APP_VERSION_L, APP_DATE_D, APP_DATE_M, APP_DATE_Y, APP_DATE_Y);	
	}

// ----- Create task list ------------
	auto* _tasksList = new TasksList();
	auto* tListener = new TaskListener();
// -----------------------------------

//======== Dialogs =========================
 	// Устанавливаем кодировку консоли в UTF-8
    SetConsoleOutputCP(1251);
	/*Dialog directory scan*/
	TQuestList* QuestFileFind = new TQuestList();	
//	QuestFileFind->setPollingTime(QUESTIONS_POLLING_TIME);	
	QuestFileFind->setPath(QUESTIONS_CATALOG_NAME);
	QuestFileFind->setPlatform(isWindows);	
//	thFileWatcher->thStart();	
	QuestFileFind->executeScan();
	
	std::string QuestFilename = QuestFileFind->getCurrentFile(); // Берем первый файл из списка в директории
	Logger::log(Logger::LogLevel::log_INFO, "Quest file=%s ", QuestFilename.c_str());

    DialogAPI::FileQuestPath = std::string(QUESTIONS_CATALOG_NAME) + PORT_CATALOG_DEVIDER_SYMBOL + QuestFilename;

	delete QuestFileFind; 
//	 std::string QuestFilename = "123.dgp";
//==========================================	

//======== ======= =========================
//=== Server ====== 
	vsServer* Diag = new vsServer(port);
	Diag->setIndex(1);
	Diag->setMode(bDaemonMode);
	char serName[] = "Diagnostic hub";
	Diag->setName(serName);
	Diag->setVer(APP_VERSION_H, APP_VERSION_L);
	Diag->setDateRelease(APP_DATE_D, APP_DATE_M, APP_DATE_Y);
	Diag->setAccounts(AccountList);
	
	Diag->setTasksList(_tasksList);
//	Diag->setQuestList(thFileWatcher);
	std::string QuestPath = std::string(QUESTIONS_CATALOG_NAME) + "\\" + QuestFilename;
	Diag->setQuestPath(QuestPath);
	
	Diag->thStartListening();
	Logger::log(Logger::LogLevel::log_INFO, "Start %s", serName);
//=== Server ======

//--- Tasks Listing ------- 
	tListener->setTasksList(_tasksList);
	tListener->thStartListening();
	Logger::log(Logger::LogLevel::log_DEBUG, "Start Listing tasks");
//--- Tasks Listing ------- 


/*------------------------------------------------------------
  ======================== HTTP SERVER =======================
 ------------------------------------------------------------*/
    auto* serverhttp = new ServerHttp();
    serverhttp->setQuestPath(QuestPath);
    serverhttp->thStart();
//==========================================

		
	//=== Input command ============
	for (;;)
	{
 		scanf("%s", cBuf);
 		//=============================================================
		if ((cBuf[0] == 'V') || (cBuf[0] == 'v')) {  
			printf("\n+-----------------------+\n");
			printf("|   Version Info        |\n");
			printf("+-----------------------+\n");
			printf("|      Version: %d.%02d    |\n", APP_VERSION_H, APP_VERSION_L);
			printf("| Release Date: %02d/%02d/%d|\n", APP_DATE_D, APP_DATE_M, APP_DATE_Y);
			printf("+-----------------------+\n\n");
		}
		
		if (strcmp(cBuf, "Help") == 0 || cBuf[0] == 'H' || cBuf[0] == 'h') {  
			printf( "  -d - Deamond mode on \n" );
			printf( " --port=xxxx - set server port (--port=9999)\n" );	 /*DHS.exe -d -log_error --port=9999*/		
		}
		
		 if (strcmp(cBuf, "Exit") == 0 || cBuf[0] == 'e' || cBuf[0] == 'E') {
            printf("  Exit\n");

            return 0;
        }
		//============================================================= 
	}

//    http_thread.join();
	return 0;
}
