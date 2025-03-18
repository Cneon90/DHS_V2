#include "port_windows.h"

//void vWindowsExecuteGarbageCollector(void)
//{
//    ULONG PID = GetCurrentProcessId();
//    HANDLE hOpenProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
//    SetProcessWorkingSetSize(hOpenProcess, -1, -1);
//    CloseHandle(hOpenProcess);
//}

//int iWindowsSetMinStackSize(unsigned int uiSize)
//{
//    //#warning "Use \"-Wl,--stack,[STACK_SIZE_IN_BYTES]\" string in C-compiler parameters"
//    return 0;
//}




void vWindowsExecuteGarbageCollector (void)
{
	ULONG PID = GetCurrentProcessId();
	HANDLE hOpenProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	SetProcessWorkingSetSize(hOpenProcess, -1, -1);
	CloseHandle(hOpenProcess);
};

int iWindowsSetMinStackSize (unsigned int uiSize)
{
	//#warning "Use \"-Wl,--stack,[STACK_SIZE_IN_BYTES]\" string in C-compiler parameters"
	return 0;
};

int iWindowsSetThreadsLimit (unsigned int uiNewLimit, unsigned char bHidePrintResultFlag)
{
	return 0;
};

int iWindowsPrintSymbolCodeInAscII (char* pcDstArray, unsigned char ucSrcSymCode)
{
	if (pcDstArray == 0) return 0;
	if (ucSrcSymCode >= 0xC0)
	{
		if (ucSrcSymCode <= 0xEF) {pcDstArray[0] = ucSrcSymCode - 0x40;} else pcDstArray[0] = ucSrcSymCode - 0x10;
	}
	else if (ucSrcSymCode == 0xA8) // ?
	{
		pcDstArray[0] = 0xF0;
	}
	else if (ucSrcSymCode == 0xB8) // ?
	{
		pcDstArray[0] = 0xF1;
	}
	else pcDstArray[0] = ucSrcSymCode;
	pcDstArray[1] = 0;
	return 1;
};

int  iWindowsPortInitTCP_Server (void)
{
	WSADATA xWsaData;
	if(WSAStartup(0x0202, &xWsaData))
    {
        printf("Error WSAStartup %d\n", WSAGetLastError());
        return -1;
	};
	return 0;
};

int iWindowsConfigServerSocket (SOCKET xSocket)
{
	int iFlag = 1;
	setsockopt(xSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &iFlag, sizeof(iFlag));
	return 0;
};


void vWindowsSetSocketRecvTimeout (SOCKET xSocket, int iRcvTimeout_ms)
{
	DWORD dwTimeout_ms;
	dwTimeout_ms = (DWORD)(iRcvTimeout_ms);
	setsockopt(xSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
	return;
}

void vWindowsSetSocketSendTimeout (SOCKET xSocket, int iSndTimeout_ms)
{
	DWORD dwTimeout_ms;
	dwTimeout_ms = (DWORD)(iSndTimeout_ms);
	setsockopt(xSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
	return;
}


void vWindowsSetSocketTimeout (SOCKET xSocket, int iRcvTimeout_ms, int iSndTimeout_ms)
{
	DWORD dwTimeout_ms;
	dwTimeout_ms = (DWORD)(iRcvTimeout_ms);
	setsockopt(xSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
	dwTimeout_ms = (DWORD)(iSndTimeout_ms);
	setsockopt(xSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &dwTimeout_ms, sizeof(dwTimeout_ms));
	return;
}

//int iWindowsSocketRead  (SOCKET iSocket, unsigned char* pcData, int iDataLenght) {return recv  (iSocket, pcData, iDataLenght, 0);}// MSG_PEEK
//int iWindowsSocketWrite (SOCKET iSocket, unsigned char* pcData, int iDataLenght) {return send  (iSocket, pcData, iDataLenght, 0);}
int iWindowsCloseServerSocket (SOCKET xSocket)    {shutdown(xSocket, SD_BOTH); closesocket(xSocket); return 0;}




