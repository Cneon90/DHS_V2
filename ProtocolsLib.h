#ifndef PROTOCOLS_LIB_H
#define PROTOCOLS_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
                          LIBRARY INCLUDES
*******************************************************************************/
//#include <unistd.h>
/*******************************************************************************
						  LIBRARY DEFINES
*******************************************************************************/
#define PROTOCOL_TYPE_MIDEP        1
#define PROTOCOL_TYPE_SGP          2


#define MIDEP_DATA_BLOCKS_MAX_COUNT  16

// Midep Connecting interface codes:
#define MIDEP_CFC_UNDEFINED   0
#define MIDEP_CIC_GSM         1
#define MIDEP_CIC_WIFI        2
#define MIDEP_CIC_LAN         3
#define MIDEP_CIC_SERIAL      4
#define MIDEP_CIC_RS232       5
#define MIDEP_CIC_RS485       6
#define MIDEP_CIC_IRDA        7
#define MIDEP_CIC_BT          8
#define MIDEP_CIC_ZIGBEE      9
#define MIDEP_CIC_GSM_2G     10
#define MIDEP_CIC_GSM_3G_LTE 11
#define MIDEP_CIC_TERM_MESH  12

typedef struct
{
	unsigned char  ucSize;
	unsigned char* pcData;
} str_MidepDataBlock;

typedef struct 
{
	// Stack data:
	unsigned char* ucMessageDataHeap;
	unsigned short usMessageDataHeapMaxSize;
	
	// Message attributes:
	unsigned int   uiServerID;
	unsigned char  ucCommand;
	unsigned char  ucSubCommand;
	unsigned char  ucTransactionIndex;
	unsigned char  ucResultCode;
	
	// Contain data
	unsigned char ucDataBlocksCount;
	str_MidepDataBlock xDataBlock[MIDEP_DATA_BLOCKS_MAX_COUNT];
} str_MidepMessage;

#define  HF_SGP_ADDR_TYPE_LOCAL    1
#define  HF_SGP_ADDR_TYPE_GLOBAL   2

// Codes of interfaces:
#define SGP_IFC_UNKNOWN            0
#define SGP_IFC_USB                1
#define SGP_IFC_SERIAL             2
#define SGP_IFC_RS_485             3
#define SGP_IFC_RS_232             4
#define SGP_IFC_LIN                5
#define SGP_IFC_IRDA               6
#define SGP_IFC_CAN                7
#define SGP_IFC_LAN_TCP_CLIENT     8
#define SGP_IFC_LAN_TCP_SERVER     9
#define SGP_IFC_LAN_UDP            10
#define SGP_IFC_GSM_TCP_CLIENT     11
#define SGP_IFC_GSM_TCP_SERVER     12
#define SGP_IFC_GSM_UDP            13
#define SGP_IFC_WFI_TCP_CLIENT     14
#define SGP_IFC_WFI_TCP_SERVER     15
#define SGP_IFC_WFI_UDP            16
#define SGP_IFC_BT                 17
#define SGP_IFC_ZIGBEE             18
#define SGP_IFC_RF                 19


typedef struct
{
	unsigned char* pcDataHeap;
	unsigned short usDataHeapSize;

	unsigned char  ucIndex;
	unsigned short usLoadIndex;

	unsigned short usTotalSize;
} str_SGP_Shuttle;
/*******************************************************************************
                          LIBRARY FUNCTIONS PROTOTYPES
*******************************************************************************/
void            vProtocolsLibMidepInitMessage        (str_MidepMessage* pxMessage, unsigned int uiServerID, unsigned char ucCommand, unsigned char ucSubCommand, unsigned char* pcStackData, unsigned short usStackDataSize);
unsigned short usProtocolsLibMidepCompileMessage     (str_MidepMessage* pxMessage);
unsigned int   uiProtocolsLibMidepDecodeMessage      (unsigned char* pcSrcData, unsigned short usSrcDataLenght, str_MidepMessage* pxDstMessage);
unsigned int   uiProtocolsLibMidepGetIntFormArray    (unsigned char *pcArray);

unsigned short usProtocolsLibSGP_CreateEstConShuttle (str_SGP_Shuttle *pxDstSPG_Shuttle, unsigned int uiServerID, unsigned short usInBuffSize, unsigned char* pcStackData, unsigned short usStackDataSize);
unsigned short usProtocolsLibSGP_CompileShuttle      (str_SGP_Shuttle *pxDstSPG_Shuttle);
unsigned short usProtocolsLibSGP_DecodeShuttle       (unsigned char* pcSrcData, unsigned short usSrcDataSize, str_SGP_Shuttle *pxDstSPG_Shuttle);
unsigned char  ucProtocolsLibSGP_ExractAddress       (str_SGP_Shuttle *pxSrcSPG_Shuttle, unsigned int* piDstAddress);
#ifdef __cplusplus
}
#endif
#endif  // PROTOCOLS_LIB_H
