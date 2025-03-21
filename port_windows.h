#ifndef _PORT_WINDOWS_H
#define _PORT_WINDOWS_H
/*******************************************************************************
                                   INCLUDES
*******************************************************************************/
#include <stdio.h>
//#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <semaphore.h>
#include <direct.h>

	#include <winsock2.h>
	#include <windows.h>
	#include <ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")
/*******************************************************************************
                                   DEFINES
*******************************************************************************/
#define iPortSetStackSize          iWindowsSetMinStackSize
#define iPortSetThreadsLimit       iWindowsSetThreadsLimit

#define iPortPrintConsoleSymbolCode  iWindowsPrintSymbolCodeInAscII

#define PORT_CATALOG_DEVIDER_SYMBOL  '\\'

#define vPortCreateDirectory(DIR_NAME)   mkdir(DIR_NAME)

#define xPortTypeSigAtomic         sig_atomic_t
#define xPortTypeSemaphore         sem_t

#define vPortPressAnyKey()         scanf("%*s");//_getch();

#define vPortSleepMs(TIME_MS)      Sleep(TIME_MS);

#define iPortInitTCP_Server()      iWindowsPortInitTCP_Server()
#define iPortConfigServerSocket    iWindowsConfigServerSocket
#define iPortSemaphoreCreate(SEM)  sem_init(&SEM, 0, 0)
#define iPortSemaphoreDestroy(SEM) sem_destroy(&SEM)
#define vPortSemaphorePost(SEM)    sem_post(&SEM)
#define vPortSemaphoreWait(SEM)    sem_wait(&SEM)
#define vPortExecuteGarbageCollector vWindowsExecuteGarbageCollector

void vWindowsExecuteGarbageCollector (void);
//{
//	ULONG PID = GetCurrentProcessId();
//	HANDLE hOpenProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
//	SetProcessWorkingSetSize(hOpenProcess, -1, -1);
//	CloseHandle(hOpenProcess);
//};

int iWindowsSetMinStackSize (unsigned int uiSize);
//{
//	//#warning "Use \"-Wl,--stack,[STACK_SIZE_IN_BYTES]\" string in C-compiler parameters"
//	return 0;
//};
//
int iWindowsSetThreadsLimit (unsigned int uiNewLimit, unsigned char bHidePrintResultFlag);
//{
//	return 0;
//};
//
int iWindowsPrintSymbolCodeInAscII (char* pcDstArray, unsigned char ucSrcSymCode);
//{
//	if (pcDstArray == 0) return 0;
//	if (ucSrcSymCode >= 0xC0)
//	{
//		if (ucSrcSymCode <= 0xEF) {pcDstArray[0] = ucSrcSymCode - 0x40;} else pcDstArray[0] = ucSrcSymCode - 0x10;
//	}
//	else if (ucSrcSymCode == 0xA8) // ¨
//	{
//		pcDstArray[0] = 0xF0;
//	}
//	else if (ucSrcSymCode == 0xB8) // ¸
//	{
//		pcDstArray[0] = 0xF1;
//	}
//	else pcDstArray[0] = ucSrcSymCode;
//	pcDstArray[1] = 0;
//	return 1;
//};
//
int  iWindowsPortInitTCP_Server (void);
//{
//	WSADATA xWsaData;
//	if(WSAStartup(0x0202, &xWsaData))
//    {
//        printf("Error WSAStartup %d\n", WSAGetLastError());
//        return -1;
//	};
//	return 0;
//};
//
int iWindowsConfigServerSocket (SOCKET xSocket);
//{
//	int iFlag = 1;
//	setsockopt(xSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &iFlag, sizeof(iFlag));
//	return 0;
//};
#define xPortTypeSocket             SOCKET
#define xPortTypeServerSocketAddr   SOCKADDR_IN
#define xPortTypeClientSocketAddr   sockaddr

#ifndef SHUT_RD
#define SHUT_RD  SD_RECEIVE
#endif  
#ifndef SHUT_WR
#define SHUT_WR  SD_SEND
#endif  
#ifndef SHUT_RDWR
#define SHUT_RDWR  SD_BOTH
#endif 
 
#define vPortSetSocketRecvTimeout vWindowsSetSocketRecvTimeout
#define vPortSetSocketSendTimeout vWindowsSetSocketSendTimeout
#define iPortSocketRead(SOCK, BUF, SIZE)       recv(SOCK, reinterpret_cast<char*>(BUF), SIZE, 0)
//#define iPortSocketWrite(SOCK, BUF, SIZE)      send(SOCK, BUF, SIZE, 0)
#define iPortSocketWrite(SOCK, BUF, SIZE)      send(SOCK, reinterpret_cast<const char*>(BUF), SIZE, 0)
#define iPortCloseServerSocket                 iWindowsCloseServerSocket

// ===== Client ===============
#define vPortSetSocketTimeout vWindowsSetSocketTimeout 

void vWindowsSetSocketTimeout (SOCKET xSocket, int iRcvTimeout_ms, int iSndTimeout_ms);
// ============================

void vWindowsSetSocketRecvTimeout (SOCKET xSocket, int iRcvTimeout_ms);
//{
//	DWORD dwTimeout_ms;
//	dwTimeout_ms = (DWORD)(iRcvTimeout_ms);
//	setsockopt(xSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
//	return;
//}
//
void vWindowsSetSocketSendTimeout (SOCKET xSocket, int iSndTimeout_ms);
//{
//	DWORD dwTimeout_ms;
//	dwTimeout_ms = (DWORD)(iSndTimeout_ms);
//	setsockopt(xSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
//	return;
//}

////int iWindowsSocketRead  (SOCKET iSocket, unsigned char* pcData, int iDataLenght) {return recv  (iSocket, pcData, iDataLenght, 0);}// MSG_PEEK
////int iWindowsSocketWrite (SOCKET iSocket, unsigned char* pcData, int iDataLenght) {return send  (iSocket, pcData, iDataLenght, 0);}
int iWindowsCloseServerSocket (SOCKET xSocket); 
//   {shutdown(xSocket, SD_BOTH); closesocket(xSocket); return 0;}

#endif

