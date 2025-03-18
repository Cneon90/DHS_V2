#ifndef _PORT_LINUX_H
#define _PORT_LINUX_H
/*******************************************************************************
                                   INCLUDES
*******************************************************************************/
#include <sys/sem.h>
#include <sys/resource.h>
#include <mysql/mysql.h>


#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <errno.h>
#include <fcntl.h>
/*******************************************************************************
                                   DEFINES
*******************************************************************************/
#define iPortSetStackSize          iLinuxSetMinStackSize
#define iPortSetThreadsLimit       iLinuxSetThreadsLimit

#define PORT_CATALOG_DEVIDER_SYMBOL  '/'
#define vPortCreateDirectory(DIR_NAME)   mkdir(DIR_NAME, 0700)

#define iPortPrintConsoleSymbolCode  iLinuxPrintSymbolCodeInUTF_8

#define xPortTypeSigAtomic    int
#define xPortTypeSemaphore    int

#define SD_BOTH               0
// gcc main.c -lmysqlclient
// Semaphores: https://www.prodevelopertutorial.com/system-v-semaphores-in-c-using-semget-semctl-semop-system-v-system-calls-in-linux/
// Install mysql:  $ apt-get install libmysqlclient-dev
// Include mysql: https://stackoverflow.com/questions/14604228/mysql-h-file-cant-be-found
// To run: sudo apt install libmysqlclient21

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf sm_wait = {0, -1, SEM_UNDO};
struct sembuf sm_post = {0, +1, SEM_UNDO};




#define vPortPressAnyKey()         getchar()

#define vPortSleepMs(TIME_MS)      usleep(TIME_MS*1000);

#define iPortSemaphoreCreate(SEM)  iLinuxPortCreateSemaphore(&SEM)
#define iPortSemaphoreDestroy(SEM) iLinuxPortDestroySemaphore(SEM)
#define vPortSemaphorePost(SEM)    semop(SEM, &sm_post, 1)
#define vPortSemaphoreWait(SEM)    semop(SEM, &sm_wait, 1)


int iLinuxSetMinStackSize (unsigned int uiSize)
{
	const rlim_t kStackSize = uiSize;  
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
        }
    }
    return result;
};

int iLinuxSetThreadsLimit (unsigned int uiNewLimit, unsigned char bHidePrintResult)
{
	struct rlimit old_lim, lim, new_lim;
  
    // Get old limits
    if( getrlimit(RLIMIT_NOFILE, &old_lim) == 0)
        if (!bHidePrintResult) printf("Old limits -> soft limit= %ld \t"
           " hard limit= %ld \n", old_lim.rlim_cur, 
                                 old_lim.rlim_max);
    else
        if (!bHidePrintResult) fprintf(stderr, "%s\n", strerror(errno));
      
    // Set new value
    lim.rlim_cur = uiNewLimit;
    lim.rlim_max = old_lim.rlim_max;
  
    // Set limits
    if(setrlimit(RLIMIT_NOFILE, &lim) == -1)
        if (!bHidePrintResult) fprintf(stderr, "%s\n", strerror(errno));
      
    // Get new limits
    if( getrlimit(RLIMIT_NOFILE, &new_lim) == 0)
        if (!bHidePrintResult) printf("New limits -> soft limit= %ld "
         " hard limit= %ld \n", new_lim.rlim_cur, 
                                  new_lim.rlim_max);
    else
        if (!bHidePrintResult) fprintf(stderr, "%s\n", strerror(errno));
    return 0;
};

int iLinuxPrintSymbolCodeInUTF_8 (char* pcDstArray, unsigned char ucSrcSymCode)
{
	if (pcDstArray == 0) return 0;
	if (ucSrcSymCode >= 0xC0)
	{
		if (ucSrcSymCode >= 0xF0)
		{
			pcDstArray[0] = 0xD1;
			pcDstArray[1] = ucSrcSymCode - 0x70;
		}
		else
		{
			pcDstArray[0] = 0xD0;
			pcDstArray[1] = ucSrcSymCode - 0x30;
		};
		pcDstArray[2] = 0;
		return 2;
	}
	else if (ucSrcSymCode == 0xA8) 
	{
		pcDstArray[0] = 0xD0;
		pcDstArray[1] = 0x81;
		pcDstArray[2] = 0;
		return 2;
	}
	else if (ucSrcSymCode == 0xB8)
	{
		pcDstArray[0] = 0xD1;
		pcDstArray[1] = 0x91;
		pcDstArray[2] = 0;
		return 2;
	}
	pcDstArray[0] = (char)(ucSrcSymCode);
	pcDstArray[1] = 0;
	return 1;
};

void vLinuxPortPostSemaphore (int iSem)
{
};

int iLinuxPortDestroySemaphore (int iSem)
{
	union semun arg;
	if (semctl(iSem, 0, IPC_RMID, arg) == -1) return -1;
	return 0;
};

int iLinuxPortCreateSemaphore (int* piSem)
{
	int key;
	static int iFtokID = 0;
	int semid;
	union semun arg;
	// Create unique key:
	iFtokID ++;
	if ((key = ftok(".", iFtokID)) == -1) return -1;
	//printf ("%i\n", key);
	// Create semaphore:
	if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) return -2;
	//printf ("%i\n", semid);
	// Initialize semaphore (set semaphore in ready state):
	//arg.val = 1;
	//if (semctl(semid, 0, SETVAL, arg) == -1) return -3;
	// Success:
	*piSem = semid;
	return 0;
	//*piSem = semget(5555, 1, 0666 | IPC_CREAT);
};

#define xPortTypeSocket             int
#define xPortTypeServerSocketAddr   struct sockaddr_in
#define xPortTypeClientSocketAddr   sockaddr


#define iPortInitTCP_Server()                 iLinuxPortInitTCP_Server()
#define iPortConfigServerSocket               iLinuxConfigServerSocket

//#define vPortSetSocketTimeout                 vLinuxSetSocketTimeout 
#define vPortSetSocketRecvTimeout vLinuxSetSocketRecvTimeout
#define vPortSetSocketSendTimeout vLinuxSetSocketSendTimeout

#define iPortSocketRead(SOCK, BUF, SIZE)      recv(SOCK, BUF, SIZE, 0)
#define iPortSocketWrite(SOCK, BUF, SIZE)     send(SOCK, BUF, SIZE, 0)
#define iPortCloseServerSocket                iLinuxCloseServerSocket
int iLinuxPortInitTCP_Server(void) {return 0;};

int iLinuxConfigServerSocket (int iSocket)
{
	int iFlag = 1;
	setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &iFlag, sizeof(iFlag));
	return 0;
};

void vLinuxSetSocketRecvTimeout (int iSocket, int iRcvTimeout_ms)
{
	int iSeconds;
	struct timeval xTimeoutSet;
	
	iSeconds = 0;
	iRcvTimeout_ms = iRcvTimeout_ms * 1000; // Convert to us
	while (iRcvTimeout_ms >= 1000000) {iRcvTimeout_ms -= 1000000; iSeconds ++;};
	xTimeoutSet.tv_sec = iSeconds;
	xTimeoutSet.tv_usec = iRcvTimeout_ms;
	setsockopt(iSocket, SOL_SOCKET, SO_RCVTIMEO, &xTimeoutSet, sizeof(xTimeoutSet));	
}

void vLinuxSetSocketSendTimeout (int iSocket, int iSndTimeout_ms)
{
	int iSeconds;
	struct timeval xTimeoutSet;
	
	iSndTimeout_ms = iSndTimeout_ms * 1000; // Convert to us
	while (iSndTimeout_ms >= 1000000) {iSndTimeout_ms -= 1000000; iSeconds ++;};
	xTimeoutSet.tv_sec = iSeconds;
	xTimeoutSet.tv_usec = iSndTimeout_ms;
	setsockopt(iSocket, SOL_SOCKET, SO_SNDTIMEO, &xTimeoutSet, sizeof(xTimeoutSet));	
}

//int iLinuxSocketRead  (int iSocket, unsigned char* pcData, int iDataLenght)    {return read  (iSocket, pcData, iDataLenght);}
//int iLinuxSocketWrite (int iSocket, unsigned char* pcData, int iDataLenght)    {return write (iSocket, pcData, iDataLenght);}
int iLinuxCloseServerSocket (int iSocket)    {shutdown(iSocket, SD_BOTH); close(iSocket); return 0;}

#endif
