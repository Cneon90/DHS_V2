#ifndef SGP2_H
#define SGP2_H

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
											INCLUDES
***************************************************************************************************/
#include <stdint.h>
#include <stddef.h>
/***************************************************************************************************
										SGP2 PACKET MANAGEMENT
***************************************************************************************************/

#define SGP2_PL_RESPONSE_ERROR_PACKET_DATA_FORMAT_ERROR   0x1
#define SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE               0x2
#define SGP2_PL_RESPONSE_ERROR_CODE_OPERATION_PROHIBITED  0xF

typedef enum {SGP2_PL_PD_REQUEST = 0, SGP2_PL_PD_RESPONSE = 1} en_SGP2_PL_PacketDirection;
typedef enum {SGP2_PL_CT_LOCAL = 0, SGP2_PL_CT_GLOBAL = 1} en_SGP2_PL_ConnectionType;
typedef enum {SGP2_PL_PT_HANDSHAKE = 0, SGP2_PL_PT_AUTH1 = 1, SGP2_PL_PT_AUTH2 = 2,  SGP2_PL_PT_MESSAGE = 3, SGP2_PL_PT_PING = 15} en_SGP2_PL_PacketType;

typedef struct
{
	unsigned short usSize;
	unsigned char* pcData;
} str_SGP2_PL_DataHeap;

#define str_SGP2_PL_PacketData str_SGP2_PL_DataHeap

typedef struct
{
	en_SGP2_PL_PacketDirection eDirection;
	en_SGP2_PL_ConnectionType  eConnectionType;
	unsigned int           uiServerAddress;
	en_SGP2_PL_PacketType      eType;
	unsigned char          ucResposeErrorCode;
} str_SGP2_PL_PacketAttr;

typedef struct
{
	unsigned short        usSize;
	str_SGP2_PL_DataHeap      xData;
} str_SGP2_PL_PacketHandle;


unsigned short usSGP2_PL_ValidatePacket          (unsigned char* pcSrcData, unsigned short usSrcDataSize);
unsigned short usSGP2_PL_ValidateRequestPacket   (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstAddr);
unsigned short usSGP2_PL_ValidateResponsePacket  (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstAddr);
unsigned short usSGP2_PL_ExtractResponseHS_Data  (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstServerID, unsigned short* psDstBuffSize);
unsigned short usSGP2_PL_CreateGlobalPingRequest (unsigned int   uiAddr, unsigned char* pcSrcPingData, unsigned short usSrcPingDataSize, unsigned char* pcDstDataArray, unsigned short usDstDataArraySize);

/***************************************************************************************************
											SGP2 ML COMMON
***************************************************************************************************/

#define AES_BLOCKLEN                          16            // Block length in bytes - AES is 128b block only
#define AES_keyExpSize                        176
struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
};

#define SGP2_ML_ENCRYPTION_BLOCK_SIZE         AES_BLOCKLEN
#define SGP2_ML_USERNAME_MAX_LEN              16


typedef struct
{
	unsigned short usCmd;
	unsigned char  ucPar;
	unsigned char* pcData;
	unsigned short usDataSize;
	unsigned short usDataMaxSize;
} str_SGP2_ML_Message;


unsigned short usSGP2_ML_MessagePutData          (str_SGP2_ML_Message* pxMessage, void* pvSrcData, unsigned short usDataSize);
unsigned short usSGP2_ML_MessagePutString        (str_SGP2_ML_Message* pxMessage, char* pcString);
unsigned short usSGP2_ML_MessagePutByte          (str_SGP2_ML_Message* pxMessage, unsigned char  ucByte);
unsigned short usSGP2_ML_MessagePutShort         (str_SGP2_ML_Message* pxMessage, unsigned short usShort);
unsigned short usSGP2_ML_MessagePutInt           (str_SGP2_ML_Message* pxMessage, unsigned int   uiInt);

unsigned short usSGP2_GetShortFromArray          (const unsigned char* pcSrcArray);
unsigned int   uiSGP2_GetIntFromArray            (const unsigned char* pcSrcArray);

void            vSGP2_PutIntToArray              (unsigned int uiVal, unsigned char* pcDstArray);
void            vSGP2_PutShortToArray            (unsigned short usVal, unsigned char* pcDstArray);
/***************************************************************************************************
											SGP2 ML CLIENT
***************************************************************************************************/
#define SGP2_ML_RESULT_SUCCESS                0
#define SGP2_ML_RESULT_CONNECTION_LOST        1
#define SGP2_ML_RESULT_RESPONSE_DATA_FALUT    2
#define SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR  3
#define SGP2_ML_RESULT_SERVER_EXCEPTION       4
#define SGP2_ML_RESULT_AUTH_FAILURE           5
#define SGP2_ML_RESULT_AUTH_REQUIRED          6

#define SGP2_ML_RESULT_HANDLE_ERROR           0xFF

typedef enum {SGP2_ML_CT_LOCAL = 0, SGP2_ML_CT_GLOBAL = 1} en_SGP2_ML_ConnectionType;

typedef struct
{
	void            (*pvSendData)           (void* pvCB_Arg, const unsigned char* pcSrcData, unsigned short  usSrcSize);
	unsigned short  (*psReceiveData)        (void* pvCB_Arg,       unsigned char* pcDstData, unsigned short  usDstSize, unsigned short usTimeout_ms);
	char*           (*ppcGetServerLogin)    (void* pvCB_Arg, unsigned int uiServerID);
	char*           (*ppcGetServerPassword) (void* pvCB_Arg, unsigned int uiServerID);
} str_SGP2_ML_ClientCB;


typedef struct
{
	// Config:
	str_SGP2_ML_ClientCB*     pxClientCB;
	unsigned char*            pcIO_Buf;
	unsigned short            usIO_BufSize;
	en_SGP2_ML_ConnectionType  eConnectionType;
	unsigned int              uiServerAddress;
	unsigned short            usRespTimeout_ms;

	// Stack data:
	unsigned short            usRemoteServerInBufSize;
	unsigned int              uiRemoteServerID;
	unsigned char             ucEncryptionBuf[SGP2_ML_ENCRYPTION_BLOCK_SIZE];
	struct  AES_ctx           xAES_Handle;
	str_SGP2_ML_Message       xMessage;
	str_SGP2_PL_PacketHandle  xPL_Packet;
	unsigned char             bIsAuthed;
} str_SGP2_ML_ClientHandle;

void vSGP2_ML_InitClientHandle(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	str_SGP2_ML_ClientCB*     pxClientCB,
	unsigned char*            pcIO_Buf,
	unsigned short            usIO_BufSize,
	en_SGP2_ML_ConnectionType  eConnectionType,
	unsigned int              uiServerAddress,
	unsigned short            usRespTimeout_ms);

unsigned char ucSGP2_ML_InitClientRequest(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	void*                     pvCB_Arg,
	str_SGP2_ML_Message**    ppxDstRequestMessage);

unsigned char ucSGP2_ML_ExecuteClientRequest(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	void*                     pvCB_Arg,
	str_SGP2_ML_Message**    ppxDstResponseMessage);

/***************************************************************************************************
											SGP2 ML SERVER
***************************************************************************************************/
typedef struct
{
	void           (*pvProcessClientRequestMessage) (void* pvCB_Arg, char* pcUserName, const str_SGP2_ML_Message* pxSrcRequestMessage, str_SGP2_ML_Message* pxDstResponseMessage);
	unsigned char* (*ppcGetClientUserPasword)       (void* pvCB_Arg, const char* pcUserName);
	void           (*pvGetRandomByteSequence)       (unsigned char* pcDst, unsigned short usDstSize);
} str_SGP2_ML_ServerCB;

typedef struct
{
	// Config:
	str_SGP2_ML_ServerCB*     pxServerCB;
	unsigned short            usInBufSize;
	unsigned int              uiAddress;
	unsigned int              uiID;

	// Stack data:
	unsigned short            usRemoteClientInBufSize;
	unsigned int              uiRemoteClientID;
	unsigned char             ucEncryptionBuf[SGP2_ML_ENCRYPTION_BLOCK_SIZE];
	struct  AES_ctx           xAES_Handle;
	unsigned char             bIsAuthed;
	char                       cCurrentUser[SGP2_ML_USERNAME_MAX_LEN + 1];

} str_SGP2_ML_ServerHandle;

void vSGP2_ML_InitServerHandle(
	str_SGP2_ML_ServerHandle* pxServerHandle,
	str_SGP2_ML_ServerCB*     pxServerCB,
	unsigned int              uiInBuffSize,
	unsigned int              uiAddress,
	unsigned int              uiID);

unsigned short usSGP2_ML_ServerProcessRequest (
	str_SGP2_ML_ServerHandle* pxServerHandle,
	void*                     pvCB_Arg,
	unsigned char*            pcSrcInData,
	unsigned short            usSrcInDataSize,
	unsigned char*            pcDstOutData,
	unsigned short            usDstOutDataMaxSize);

#ifdef __cplusplus
}
#endif
#endif  // SGP2_H
