/***************************************************************************************************
										INCLUDES
***************************************************************************************************/
#include "SGP2.h"
/***************************************************************************************************
										DEFINES
***************************************************************************************************/
#define LOCAL_ADDR_REQUEST_SYNC    0x55
#define LOCAL_ADDR_RESPONSE_SYNC   0xAA
#define GLOBAL_ADDR_REQUEST_SYNC   0x77
#define GLOBAL_ADDR_RESPONSE_SYNC  0x88

#define SGP2_ML_MIN_DATA_BUF_SIZE       16
void vConvertPasswordToKey            (const char* pcSrcPassword, unsigned char* pcDstKey, unsigned short usKeySize);
unsigned short usCheckMessageFreeSize (str_SGP2_ML_Message* pxMessage);
void           AES_init_ctx           (struct AES_ctx* ctx, const uint8_t* key);
void           AES_ECB_encrypt        (const struct AES_ctx* ctx, uint8_t* buf);
void           AES_ECB_decrypt        (const struct AES_ctx* ctx, uint8_t* buf);

unsigned short usGetCRC16 (unsigned char *pcSrcArray, unsigned short usArraySize);

void            vSGP2_PL_InitPacket             (str_SGP2_PL_PacketHandle* pxHandle, str_SGP2_PL_PacketAttr* pxSrcAttr, unsigned char* pcDstDataArray, unsigned short usDstDataArraySize);
void            vSGP2_PL_PacketSetRespErrorCode (str_SGP2_PL_PacketHandle* pxHandle, unsigned char ucResponseErrorCode);
unsigned short usSGP2_PL_PacketGetDataPointer   (str_SGP2_PL_PacketHandle* pxHandle, unsigned char** ppcDstPointer);
unsigned short usSGP2_PL_PacketPutData          (str_SGP2_PL_PacketHandle* pxHandle, void* pvSrcData, unsigned short usDataSize);
unsigned short usSGP2_PL_PacketPutIdleData      (str_SGP2_PL_PacketHandle* pxHandle, unsigned short usDataSize);
unsigned short usSGP2_PL_PacketPutString        (str_SGP2_PL_PacketHandle* pxHandle, char* pcString);
unsigned short usSGP2_PL_PacketPutByte          (str_SGP2_PL_PacketHandle* pxHandle, unsigned char  ucByte);
unsigned short usSGP2_PL_PacketPutShort         (str_SGP2_PL_PacketHandle* pxHandle, unsigned short usShort);
unsigned short usSGP2_PL_PacketPutInt           (str_SGP2_PL_PacketHandle* pxHandle, unsigned int    uiInt);
unsigned short usSGP2_PL_AssemblePacket         (str_SGP2_PL_PacketHandle* pxHandle);

unsigned short usSGP2_PL_DecodePacket           (unsigned char* pcSrcData, unsigned short usSrcDataSize, str_SGP2_PL_PacketAttr* pxDstAttr, str_SGP2_PL_PacketData* pxDstPacketData);
/***************************************************************************************************
* Function:    usSGP2_GetShortFromArray
* Description:
****************************************************************************************************/
unsigned short usSGP2_GetShortFromArray (const unsigned char* pcSrcArray)
{
	unsigned short usResult;
	usResult  = pcSrcArray[1];
	usResult  = usResult << 8;
	usResult += pcSrcArray[0];
	return usResult;
};
/***************************************************************************************************
* Function:    uiSGP2_GetIntFromArray
* Description:
****************************************************************************************************/
unsigned int uiSGP2_GetIntFromArray   (const unsigned char* pcSrcArray)
{
	unsigned int uiResult;
	uiResult  = pcSrcArray[3];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[2];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[1];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[0];
	return uiResult;
};
/***************************************************************************************************
* Function:    vSGP2_PutIntToArray
* Description:
****************************************************************************************************/
void            vSGP2_PutIntToArray              (unsigned int uiVal, unsigned char* pcDstArray)
{
	pcDstArray[0] = (unsigned char)(uiVal >> 0);
	pcDstArray[1] = (unsigned char)(uiVal >> 8);
	pcDstArray[2] = (unsigned char)(uiVal >> 16);
	pcDstArray[3] = (unsigned char)(uiVal >> 24);
};
/***************************************************************************************************
* Function:    vSGP2_PutShortToArray
* Description:
****************************************************************************************************/
void            vSGP2_PutShortToArray            (unsigned short usVal, unsigned char* pcDstArray)
{
	pcDstArray[0] = (unsigned char)(usVal >> 0);
	pcDstArray[1] = (unsigned char)(usVal >> 8);
};
/***************************************************************************************************
* Function:    usSGP2_ML_MessagePutData
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_MessagePutData          (str_SGP2_ML_Message* pxMessage, void* pvSrcData, unsigned short usDataSize)
{
	unsigned short s;
	s = usCheckMessageFreeSize(pxMessage);
	if (usDataSize > s) usDataSize = s;
	for (s = 0; s < usDataSize; s ++) pxMessage->pcData[pxMessage->usDataSize + s] = *(((unsigned char*)(pvSrcData)) + s);
	pxMessage->usDataSize += s;
	return s;
};
/***************************************************************************************************
* Function:    usSGP2_ML_MessagePutString
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_MessagePutString        (str_SGP2_ML_Message* pxMessage, char* pcString)
{
	unsigned short s, usFree;
	usFree = usCheckMessageFreeSize(pxMessage);
	if (usFree < 1) return 0;
	for (s = 0; s < (usFree - 1); s ++)
	{
		if (pcString[s] == 0) break;
		pxMessage->pcData[pxMessage->usDataSize + s] = (unsigned char)(pcString[s]);
	};
	pxMessage->pcData[pxMessage->usDataSize + s] = 0;
	pxMessage->usDataSize += (s + 1);
	return (s + 1);
};
/***************************************************************************************************
* Function:    usSGP2_ML_MessagePutByte
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_MessagePutByte          (str_SGP2_ML_Message* pxMessage, unsigned char  ucByte)
{
	if (usCheckMessageFreeSize(pxMessage) < 1) return 0;
	pxMessage->pcData[pxMessage->usDataSize ++] = ucByte;
	return 1;
};
/***************************************************************************************************
* Function:    usSGP2_ML_MessagePutShort
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_MessagePutShort         (str_SGP2_ML_Message* pxMessage, unsigned short usShort)
{
	if (usCheckMessageFreeSize(pxMessage) < 2) return 0;
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(usShort >> 0);
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(usShort >> 8);
	return 2;
};
/***************************************************************************************************
* Function:    usSGP2_ML_MessagePutInt
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_MessagePutInt           (str_SGP2_ML_Message* pxMessage, unsigned int   uiInt)
{
	if (usCheckMessageFreeSize(pxMessage) < 4) return 0;
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(uiInt >> 0);
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(uiInt >> 8);
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(uiInt >> 16);
	pxMessage->pcData[pxMessage->usDataSize ++] = (unsigned char)(uiInt >> 24);
	return 4;
};
/***************************************************************************************************
* Function:    vSGP2_ML_InitClientHandle
* Description:
****************************************************************************************************/
void vSGP2_ML_InitClientHandle(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	str_SGP2_ML_ClientCB*     pxClientCB,
	unsigned char*            pcIO_Buf,
	unsigned short            usIO_BufSize,
	en_SGP2_ML_ConnectionType  eConnectionType,
	unsigned int              uiServerAddress,
	unsigned short            usRespTimeout_ms)
{
	if (pxClientHandle == 0) return;
	pxClientHandle->pxClientCB = pxClientCB;
	pxClientHandle->pcIO_Buf = pcIO_Buf;
	pxClientHandle->usIO_BufSize = usIO_BufSize;
	pxClientHandle->eConnectionType = eConnectionType;
	pxClientHandle->uiServerAddress = uiServerAddress;
	pxClientHandle->usRespTimeout_ms = usRespTimeout_ms;

	// Reset stack data:
	pxClientHandle->usRemoteServerInBufSize = 0;
	pxClientHandle->uiRemoteServerID        = 0;
	pxClientHandle->bIsAuthed               = 0;
};

/***************************************************************************************************
* Function:    ucSGP2_ML_InitClientRequest
* Description:
****************************************************************************************************/
unsigned char ucSGP2_ML_InitClientRequest(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	void*                     pvCB_Arg,
	str_SGP2_ML_Message**    ppxDstRequestMessage)
{
	unsigned short s, p;
	str_SGP2_PL_PacketData   xClientPacketData;
	str_SGP2_PL_PacketAttr   xClientPacketAttr;
	char* pc;

	if (pxClientHandle == 0)                                      return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pcIO_Buf == 0)                            return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->usIO_BufSize < SGP2_ML_MIN_DATA_BUF_SIZE) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB == 0)                          return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB->pvSendData == 0)              return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB->psReceiveData == 0)           return SGP2_ML_RESULT_HANDLE_ERROR;

	if ((!pxClientHandle->usRemoteServerInBufSize) || (!pxClientHandle->bIsAuthed))
	{

		// Handshake request:
		xClientPacketAttr.eDirection = SGP2_PL_PD_REQUEST;
		xClientPacketAttr.eConnectionType = SGP2_PL_CT_LOCAL; if (pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) xClientPacketAttr.eConnectionType = SGP2_PL_CT_GLOBAL;
		xClientPacketAttr.uiServerAddress = pxClientHandle->uiServerAddress;
		xClientPacketAttr.ucResposeErrorCode = 0;
		xClientPacketAttr.eType = SGP2_PL_PT_HANDSHAKE;
		vSGP2_PL_InitPacket (&pxClientHandle->xPL_Packet, &xClientPacketAttr, pxClientHandle->pcIO_Buf, pxClientHandle->usIO_BufSize);
		usSGP2_PL_PacketPutShort(&pxClientHandle->xPL_Packet, pxClientHandle->usIO_BufSize);
		usSGP2_PL_PacketPutInt  (&pxClientHandle->xPL_Packet, 0); // Client ID

		s = usSGP2_PL_AssemblePacket(&pxClientHandle->xPL_Packet);
		pxClientHandle->pxClientCB->pvSendData(pvCB_Arg, pxClientHandle->pcIO_Buf, s);
		// Receive response:
		p = 0;
		for (;;)
		{
			if (p > pxClientHandle->usIO_BufSize) p = pxClientHandle->usIO_BufSize;
			s = pxClientHandle->pxClientCB->psReceiveData(pvCB_Arg, &pxClientHandle->pcIO_Buf[p], (pxClientHandle->usIO_BufSize - p), pxClientHandle->usRespTimeout_ms);
			p += s;
			if (s == 0) { pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_CONNECTION_LOST;};
			// Decode response:
			s = usSGP2_PL_DecodePacket    (pxClientHandle->pcIO_Buf, p, &xClientPacketAttr, &xClientPacketData);
			// Check decode success (parsed size != 0)
			if (s) break;
		};
		if (!s) return SGP2_ML_RESULT_RESPONSE_DATA_FALUT;
		// Check packet direction:
		if (xClientPacketAttr.eDirection != SGP2_PL_PD_RESPONSE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check packet address:
		if (pxClientHandle->uiServerAddress)
		{
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_LOCAL)  && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0x000000FF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0xFFFFFFFF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		};
		// Check data type:
		if (xClientPacketAttr.eType != SGP2_PL_PT_HANDSHAKE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check error code:
		if (xClientPacketAttr.ucResposeErrorCode) return SGP2_ML_RESULT_SERVER_EXCEPTION;
		// Check data format:
		if (xClientPacketData.usSize < 6) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Fix Remote Server attributes:
		pxClientHandle->usRemoteServerInBufSize = usSGP2_GetShortFromArray(&xClientPacketData.pcData[0]);
		pxClientHandle->uiRemoteServerID = uiSGP2_GetIntFromArray(&xClientPacketData.pcData[2]);
		// Calc fact out buf size:
		if (pxClientHandle->usRemoteServerInBufSize > pxClientHandle->usIO_BufSize) pxClientHandle->usRemoteServerInBufSize = pxClientHandle->usIO_BufSize;

		// Auth1 request:
		if (!pxClientHandle->pxClientCB->ppcGetServerLogin)    return SGP2_ML_RESULT_AUTH_FAILURE;
		xClientPacketAttr.eDirection = SGP2_PL_PD_REQUEST;
		xClientPacketAttr.eConnectionType = SGP2_PL_CT_LOCAL; if (pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) xClientPacketAttr.eConnectionType = SGP2_PL_CT_GLOBAL;
		xClientPacketAttr.uiServerAddress = pxClientHandle->uiServerAddress;
		xClientPacketAttr.ucResposeErrorCode = 0;
		xClientPacketAttr.eType = SGP2_PL_PT_AUTH1;
		vSGP2_PL_InitPacket (&pxClientHandle->xPL_Packet, &xClientPacketAttr, pxClientHandle->pcIO_Buf, pxClientHandle->usRemoteServerInBufSize);
		usSGP2_PL_PacketPutString(&pxClientHandle->xPL_Packet, pxClientHandle->pxClientCB->ppcGetServerLogin(pvCB_Arg, pxClientHandle->uiRemoteServerID));
		s = usSGP2_PL_AssemblePacket(&pxClientHandle->xPL_Packet);
		pxClientHandle->pxClientCB->pvSendData(pvCB_Arg, pxClientHandle->pcIO_Buf, s);
		// Receive response:
		p = 0;
		for (;;)
		{
			if (p > pxClientHandle->usIO_BufSize) p = pxClientHandle->usIO_BufSize;
			s = pxClientHandle->pxClientCB->psReceiveData(pvCB_Arg, &pxClientHandle->pcIO_Buf[p], (pxClientHandle->usIO_BufSize - p), pxClientHandle->usRespTimeout_ms);
			p += s;
			if (s == 0) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_CONNECTION_LOST;};
			// Decode response:
			s = usSGP2_PL_DecodePacket    (pxClientHandle->pcIO_Buf, p, &xClientPacketAttr, &xClientPacketData);
			// Check decode success (parsed size != 0)
			if (s) break;
		};
		if (!s) return SGP2_ML_RESULT_RESPONSE_DATA_FALUT;
		// Decode response:
		s = usSGP2_PL_DecodePacket    (pxClientHandle->pcIO_Buf, s, &xClientPacketAttr, &xClientPacketData);
		// Check decode success (parsed size != 0)
		if (!s) return SGP2_ML_RESULT_RESPONSE_DATA_FALUT;
		// Check packet direction:
		if (xClientPacketAttr.eDirection != SGP2_PL_PD_RESPONSE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check packet address:
		if (pxClientHandle->uiServerAddress)
		{
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_LOCAL)  && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0x000000FF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0xFFFFFFFF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		};
		// Check data type:
		if (xClientPacketAttr.eType != SGP2_PL_PT_AUTH1) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check error code:
		if (xClientPacketAttr.ucResposeErrorCode) return SGP2_ML_RESULT_AUTH_FAILURE;
		// Check data format:
		if (xClientPacketData.usSize < SGP2_ML_ENCRYPTION_BLOCK_SIZE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		for (s = 0; s < SGP2_ML_ENCRYPTION_BLOCK_SIZE; s ++) pxClientHandle->ucEncryptionBuf[s] = xClientPacketData.pcData[s];
		// Create aes key as cycled password (use Packet data as a buf):
		if (!pxClientHandle->pxClientCB->ppcGetServerPassword) return SGP2_ML_RESULT_AUTH_FAILURE;
		pc = pxClientHandle->pxClientCB->ppcGetServerPassword (pvCB_Arg, pxClientHandle->uiRemoteServerID);
		if (!pc)  return SGP2_ML_RESULT_AUTH_FAILURE;
		vConvertPasswordToKey(pc, xClientPacketData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
		AES_init_ctx(&pxClientHandle->xAES_Handle, xClientPacketData.pcData);
		AES_ECB_encrypt(&pxClientHandle->xAES_Handle, pxClientHandle->ucEncryptionBuf);

		// Auth2 request:
		xClientPacketAttr.eDirection = SGP2_PL_PD_REQUEST;
		xClientPacketAttr.eConnectionType = SGP2_PL_CT_LOCAL; if (pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) xClientPacketAttr.eConnectionType = SGP2_PL_CT_GLOBAL;
		xClientPacketAttr.uiServerAddress = pxClientHandle->uiServerAddress;
		xClientPacketAttr.ucResposeErrorCode = 0;
		xClientPacketAttr.eType = SGP2_PL_PT_AUTH2;
		vSGP2_PL_InitPacket (&pxClientHandle->xPL_Packet, &xClientPacketAttr, pxClientHandle->pcIO_Buf, pxClientHandle->usRemoteServerInBufSize);
		usSGP2_PL_PacketPutData(&pxClientHandle->xPL_Packet, pxClientHandle->ucEncryptionBuf, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
		s = usSGP2_PL_AssemblePacket(&pxClientHandle->xPL_Packet);
		pxClientHandle->pxClientCB->pvSendData(pvCB_Arg, pxClientHandle->pcIO_Buf, s);
		// Receive response:
		p = 0;
		for (;;)
		{
			if (p > pxClientHandle->usIO_BufSize) p = pxClientHandle->usIO_BufSize;
			s = pxClientHandle->pxClientCB->psReceiveData(pvCB_Arg, &pxClientHandle->pcIO_Buf[p], (pxClientHandle->usIO_BufSize - p), pxClientHandle->usRespTimeout_ms);
			p += s;
			if (s == 0) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_CONNECTION_LOST;};
			// Decode response:
			s = usSGP2_PL_DecodePacket    (pxClientHandle->pcIO_Buf, p, &xClientPacketAttr, &xClientPacketData);
			// Check decode success (parsed size != 0)
			if (s) break;
		};
		if (!s) return SGP2_ML_RESULT_RESPONSE_DATA_FALUT;
		// Check packet direction:
		if (xClientPacketAttr.eDirection != SGP2_PL_PD_RESPONSE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check packet address:
		if (pxClientHandle->uiServerAddress)
		{
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_LOCAL)  && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0x000000FF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
			if ((pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0xFFFFFFFF))) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		};
		// Check data type:
		if (xClientPacketAttr.eType != SGP2_PL_PT_AUTH2) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		// Check error code:
		if (xClientPacketAttr.ucResposeErrorCode) return SGP2_ML_RESULT_AUTH_FAILURE;
		// Check data format:
		if (xClientPacketData.usSize < SGP2_ML_ENCRYPTION_BLOCK_SIZE) return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;
		for (s = 0; s < SGP2_ML_ENCRYPTION_BLOCK_SIZE; s ++) pxClientHandle->ucEncryptionBuf[s] = xClientPacketData.pcData[s];
		// Create aes key as cycled password (use Packet data as a buf):
		if (!pxClientHandle->pxClientCB->ppcGetServerPassword) return SGP2_ML_RESULT_AUTH_FAILURE;
		pc = pxClientHandle->pxClientCB->ppcGetServerPassword (pvCB_Arg, pxClientHandle->uiRemoteServerID);
		if (!pc)  return SGP2_ML_RESULT_AUTH_FAILURE;
		vConvertPasswordToKey(pc, xClientPacketData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
		// Create session AES key:
		for (s = 0; s < SGP2_ML_ENCRYPTION_BLOCK_SIZE; s ++) xClientPacketData.pcData[s] ^= pxClientHandle->ucEncryptionBuf[s];
		AES_init_ctx(&pxClientHandle->xAES_Handle, xClientPacketData.pcData);
		pxClientHandle->bIsAuthed = 1;

	};

	xClientPacketAttr.eDirection = SGP2_PL_PD_REQUEST;
	xClientPacketAttr.eConnectionType = SGP2_PL_CT_LOCAL; if (pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) xClientPacketAttr.eConnectionType = SGP2_PL_CT_GLOBAL;
	xClientPacketAttr.uiServerAddress = pxClientHandle->uiServerAddress;
	xClientPacketAttr.ucResposeErrorCode = 0;
	xClientPacketAttr.eType = SGP2_PL_PT_MESSAGE;
	vSGP2_PL_InitPacket (&pxClientHandle->xPL_Packet, &xClientPacketAttr, pxClientHandle->pcIO_Buf, pxClientHandle->usRemoteServerInBufSize);
	s = usSGP2_PL_PacketGetDataPointer   (&pxClientHandle->xPL_Packet, &pxClientHandle->xMessage.pcData);
	if (s < 6) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (s < SGP2_ML_ENCRYPTION_BLOCK_SIZE) return SGP2_ML_RESULT_HANDLE_ERROR;
	s = s / SGP2_ML_ENCRYPTION_BLOCK_SIZE;
	s = s * SGP2_ML_ENCRYPTION_BLOCK_SIZE;
	pxClientHandle->xMessage.pcData += 5;
	pxClientHandle->xMessage.usDataMaxSize = s - 6;
	pxClientHandle->xMessage.usDataSize = 0;
	pxClientHandle->xMessage.usCmd = 0;
	pxClientHandle->xMessage.ucPar = 0;
	*ppxDstRequestMessage = &pxClientHandle->xMessage;
	return 0;
};
/***************************************************************************************************
* Function:    ucSGP2_ML_ExecuteClientRequest
* Description:
****************************************************************************************************/
unsigned char ucSGP2_ML_ExecuteClientRequest(
	str_SGP2_ML_ClientHandle* pxClientHandle,
	void*                     pvCB_Arg,
	str_SGP2_ML_Message**    ppxDstResponseMessage)
{
	str_SGP2_PL_PacketData   xClientPacketData;
	str_SGP2_PL_PacketAttr   xClientPacketAttr;
	unsigned char ucCS;
	unsigned char* pcMessageDataPointer;
	unsigned short s, p, usTotalSize;

	if (pxClientHandle == 0)                                      return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pcIO_Buf == 0)                            return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->usIO_BufSize < SGP2_ML_MIN_DATA_BUF_SIZE) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB == 0)                          return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB->pvSendData == 0)              return SGP2_ML_RESULT_HANDLE_ERROR;
	if (pxClientHandle->pxClientCB->psReceiveData == 0)           return SGP2_ML_RESULT_HANDLE_ERROR;
	if (!pxClientHandle->usRemoteServerInBufSize)                 return SGP2_ML_RESULT_AUTH_REQUIRED;

	if (usSGP2_PL_PacketGetDataPointer   (&pxClientHandle->xPL_Packet, &pcMessageDataPointer) == 0) return SGP2_ML_RESULT_HANDLE_ERROR;
	// Pack message in packet Data field:
	if (usSGP2_PL_PacketPutShort    (&pxClientHandle->xPL_Packet, pxClientHandle->xMessage.usCmd) != 2) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (usSGP2_PL_PacketPutByte     (&pxClientHandle->xPL_Packet, pxClientHandle->xMessage.ucPar) != 1) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (usSGP2_PL_PacketPutShort    (&pxClientHandle->xPL_Packet, pxClientHandle->xMessage.usDataSize) != 2) return SGP2_ML_RESULT_HANDLE_ERROR;
	if (usSGP2_PL_PacketPutIdleData (&pxClientHandle->xPL_Packet, pxClientHandle->xMessage.usDataSize) != pxClientHandle->xMessage.usDataSize) return SGP2_ML_RESULT_HANDLE_ERROR;
	// Add CS:
	ucCS = 0; for (s = 0; s < (pxClientHandle->xMessage.usDataSize + 5); s ++) ucCS += pcMessageDataPointer[s];
	if (usSGP2_PL_PacketPutByte     (&pxClientHandle->xPL_Packet, (0xFF - ucCS)) != 1) return SGP2_ML_RESULT_HANDLE_ERROR;
	// Extend
	usTotalSize = pxClientHandle->xMessage.usDataSize + 6;
	while (usTotalSize % SGP2_ML_ENCRYPTION_BLOCK_SIZE)
	{
		usSGP2_PL_PacketPutByte (&pxClientHandle->xPL_Packet, ucCS);
		usTotalSize ++;
	};
	// Encrypt message data:
	s = 0;
	while (usTotalSize >= SGP2_ML_ENCRYPTION_BLOCK_SIZE)
	{
		AES_ECB_encrypt(&pxClientHandle->xAES_Handle, &pcMessageDataPointer[s]);
		s += SGP2_ML_ENCRYPTION_BLOCK_SIZE;
		usTotalSize -= SGP2_ML_ENCRYPTION_BLOCK_SIZE;
	};
	// Send packet:
	s = usSGP2_PL_AssemblePacket(&pxClientHandle->xPL_Packet);
	pxClientHandle->pxClientCB->pvSendData(pvCB_Arg, pxClientHandle->pcIO_Buf, s);
	// Receive response:
	p = 0;
	for (;;)
	{
		if (p > pxClientHandle->usIO_BufSize) p = pxClientHandle->usIO_BufSize;
		s = pxClientHandle->pxClientCB->psReceiveData(pvCB_Arg, &pxClientHandle->pcIO_Buf[p], (pxClientHandle->usIO_BufSize - p), pxClientHandle->usRespTimeout_ms);
		p += s;
		if (s == 0) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_CONNECTION_LOST;};
		// Decode response:
		s = usSGP2_PL_DecodePacket    (pxClientHandle->pcIO_Buf, p, &xClientPacketAttr, &xClientPacketData);
		// Check decode success (parsed size != 0)
		if (s) break;
	};
	if (!s) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_DATA_FALUT;};
	// Check packet direction:
	if (xClientPacketAttr.eDirection != SGP2_PL_PD_RESPONSE) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};

	// Check packet address:
	if (pxClientHandle->uiServerAddress)
	{
		if ((pxClientHandle->eConnectionType == SGP2_ML_CT_LOCAL)  && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0x000000FF))) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
		if ((pxClientHandle->eConnectionType == SGP2_ML_CT_GLOBAL) && (xClientPacketAttr.uiServerAddress != (pxClientHandle->uiServerAddress & 0xFFFFFFFF))) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
	};
	// Check data type:
	if (xClientPacketAttr.eType != SGP2_PL_PT_MESSAGE) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
	// Check error code:
	if (xClientPacketAttr.ucResposeErrorCode) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_AUTH_FAILURE;};
	// Check data format:
	if (!((xClientPacketData.usSize >= SGP2_ML_ENCRYPTION_BLOCK_SIZE) && ((xClientPacketData.usSize % SGP2_ML_ENCRYPTION_BLOCK_SIZE) == 0))) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
	// Decrypt message:
	s = 0;
	while (s < xClientPacketData.usSize)
	{
		AES_ECB_decrypt(&pxClientHandle->xAES_Handle, &xClientPacketData.pcData[s]);
		s += SGP2_ML_ENCRYPTION_BLOCK_SIZE;
	};
	pxClientHandle->xMessage.usCmd = usSGP2_GetShortFromArray (&xClientPacketData.pcData[0]);
	pxClientHandle->xMessage.ucPar = xClientPacketData.pcData[2];
	pxClientHandle->xMessage.usDataSize = usSGP2_GetShortFromArray (&xClientPacketData.pcData[3]);
	pxClientHandle->xMessage.usDataMaxSize = pxClientHandle->xMessage.usDataSize;
	pxClientHandle->xMessage.pcData = &xClientPacketData.pcData[5];
	// Validate message (CS):
	if ((pxClientHandle->xMessage.usDataSize + 6) > xClientPacketData.usSize) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
	ucCS = 0;
	for (s = 0; s < pxClientHandle->xMessage.usDataSize + 5; s ++) ucCS += xClientPacketData.pcData[s];
	if (xClientPacketData.pcData[s] != (0xFF - ucCS)) {pxClientHandle->bIsAuthed = 0; return SGP2_ML_RESULT_RESPONSE_FORMAT_ERROR;};
	*ppxDstResponseMessage = &pxClientHandle->xMessage; 
	return 0;
};
/***************************************************************************************************
* Function:    vSGP2_ML_InitServerHandle
* Description:
****************************************************************************************************/
void vSGP2_ML_InitServerHandle(
	str_SGP2_ML_ServerHandle* pxServerHandle,
	str_SGP2_ML_ServerCB*     pxServerCB,
	unsigned int              uiInBuffSize,
	unsigned int              uiAddress,
	unsigned int              uiID)
{
	if (pxServerHandle == 0) return;
	pxServerHandle->pxServerCB  = pxServerCB;
	pxServerHandle->usInBufSize = uiInBuffSize;
	pxServerHandle->uiAddress   = uiAddress;
	pxServerHandle->uiID        = uiID;

	// Reset stack data:
	pxServerHandle->bIsAuthed = 0;
	pxServerHandle->usRemoteClientInBufSize = 0;
	pxServerHandle->uiRemoteClientID        = 0;
};
/***************************************************************************************************
* Function:    usSGP2_ML_ServerProcessRequest
* Description:
****************************************************************************************************/
unsigned short usSGP2_ML_ServerProcessRequest (
	str_SGP2_ML_ServerHandle* pxServerHandle,
	void*                     pvCB_Arg,
	unsigned char*            pcSrcInData,
	unsigned short            usSrcInDataSize,
	unsigned char*            pcDstOutData,
	unsigned short            usDstOutDataMaxSize)
{
	unsigned short s, usTotalSize;
	str_SGP2_PL_PacketData   xServerPacketInData;
	str_SGP2_PL_PacketAttr   xServerPacketAttr;
	str_SGP2_PL_PacketHandle xServerPacket;
	//char* pc;
	unsigned char* pcDataPointer;
	unsigned char ucCS;
	str_SGP2_ML_Message xInMessage, xOutMessage;

	if (pxServerHandle == 0) return 0;

	s = usSGP2_PL_DecodePacket    (pcSrcInData, usSrcInDataSize, &xServerPacketAttr, &xServerPacketInData);
	// Check decode success (parsed size != 0)
	if (!s) return 0;
	// Check packet direction:
	if (xServerPacketAttr.eDirection != SGP2_PL_PD_REQUEST) return 0;
	// Check packet address:
	if (xServerPacketAttr.uiServerAddress)
	{
		if ((xServerPacketAttr.eConnectionType == SGP2_PL_CT_LOCAL)  && (xServerPacketAttr.uiServerAddress != (pxServerHandle->uiAddress & 0x000000FF))) return 0;
		if ((xServerPacketAttr.eConnectionType == SGP2_PL_CT_GLOBAL) && (xServerPacketAttr.uiServerAddress != (pxServerHandle->uiAddress & 0xFFFFFFFF))) return 0;
	};
	// Check response code (must be 0 for requests):
	if (xServerPacketAttr.ucResposeErrorCode != 0) return 0;

	// Create response packet:
	xServerPacketAttr.eDirection = SGP2_PL_PD_RESPONSE;
	//xServerPacketAttr.uiServerAddress = xServerPacketAttr.uiServerAddress;  // NO CHANGES
	// xServerPacketAttr.eConnectionType = xServerPacketAttr.eConnectionType; // NO CHANGES
	// xServerPacketAttr.eType = xServerPacketAttr.eType;                     // NO CHANGES
	// xServerPacketAttr.ucResposeErrorCode = 0;                              // NO CHANGES
	s = usDstOutDataMaxSize;
	if ((pxServerHandle->usRemoteClientInBufSize) && (pxServerHandle->usRemoteClientInBufSize < s)) s = pxServerHandle->usRemoteClientInBufSize;
	vSGP2_PL_InitPacket (&xServerPacket, &xServerPacketAttr, pcDstOutData, s);
	if (xServerPacketAttr.eType == SGP2_PL_PT_HANDSHAKE)
	{
		if (xServerPacketInData.usSize >= 6)
		{
			pxServerHandle->usRemoteClientInBufSize = usSGP2_GetShortFromArray(&xServerPacketInData.pcData[0]);
			pxServerHandle->uiRemoteClientID = uiSGP2_GetIntFromArray(&xServerPacketInData.pcData[2]);

			usSGP2_PL_PacketPutShort(&xServerPacket, pxServerHandle->usInBufSize);
			usSGP2_PL_PacketPutInt  (&xServerPacket, pxServerHandle->uiID);
		}
		else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_PACKET_DATA_FORMAT_ERROR);
	}
	else if (xServerPacketAttr.eType == SGP2_PL_PT_AUTH1)
	{
		if ((xServerPacketInData.usSize) && (pxServerHandle->pxServerCB->ppcGetClientUserPasword))
		{
			xServerPacketInData.pcData[xServerPacketInData.usSize - 1] = 0;
			if (pxServerHandle->pxServerCB->pvGetRandomByteSequence) pxServerHandle->pxServerCB->pvGetRandomByteSequence(pxServerHandle->ucEncryptionBuf, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
			usSGP2_PL_PacketPutData(&xServerPacket, pxServerHandle->ucEncryptionBuf, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
			for (s = 0; s < SGP2_ML_USERNAME_MAX_LEN; s ++) {if (xServerPacketInData.pcData[s] == 0) break; pxServerHandle->cCurrentUser[s] = xServerPacketInData.pcData[s];};
			while (s <= SGP2_ML_USERNAME_MAX_LEN) pxServerHandle->cCurrentUser[s ++] = 0;
			pcDataPointer = pxServerHandle->pxServerCB->ppcGetClientUserPasword(pvCB_Arg, pxServerHandle->cCurrentUser);
			// Create aes key as cycled password (use Packet data as a buf):
			if (pcDataPointer) {vConvertPasswordToKey((const char*)pcDataPointer, xServerPacketInData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);}
			else if (pxServerHandle->pxServerCB->pvGetRandomByteSequence) pxServerHandle->pxServerCB->pvGetRandomByteSequence(xServerPacketInData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
			AES_init_ctx(&pxServerHandle->xAES_Handle, xServerPacketInData.pcData);
			AES_ECB_encrypt(&pxServerHandle->xAES_Handle, pxServerHandle->ucEncryptionBuf);
		}
		else  vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE);
	}
	else if (xServerPacketAttr.eType == SGP2_PL_PT_AUTH2)
	{
		if ((xServerPacketInData.usSize >= SGP2_ML_ENCRYPTION_BLOCK_SIZE) && (pxServerHandle->pxServerCB->ppcGetClientUserPasword))
		{
			for (s = 0; s < SGP2_ML_ENCRYPTION_BLOCK_SIZE; s ++) if (xServerPacketInData.pcData[s] != pxServerHandle->ucEncryptionBuf[s]) break;
			if (s < SGP2_ML_ENCRYPTION_BLOCK_SIZE)
			{
				vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE);
			}
			else
			{
				if (pxServerHandle->pxServerCB->pvGetRandomByteSequence) pxServerHandle->pxServerCB->pvGetRandomByteSequence(pxServerHandle->ucEncryptionBuf, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
				usSGP2_PL_PacketPutData(&xServerPacket, pxServerHandle->ucEncryptionBuf, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
				pcDataPointer = pxServerHandle->pxServerCB->ppcGetClientUserPasword(pvCB_Arg, pxServerHandle->cCurrentUser);
				// Create key as cycled password (use Packet data as a buf):
				if (pcDataPointer) {vConvertPasswordToKey((const char*)pcDataPointer, xServerPacketInData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);}
				else if (pxServerHandle->pxServerCB->pvGetRandomByteSequence) pxServerHandle->pxServerCB->pvGetRandomByteSequence(xServerPacketInData.pcData, SGP2_ML_ENCRYPTION_BLOCK_SIZE);
				// Create session AES key:
				for (s = 0; s < SGP2_ML_ENCRYPTION_BLOCK_SIZE; s ++) xServerPacketInData.pcData[s] ^= pxServerHandle->ucEncryptionBuf[s];
				AES_init_ctx(&pxServerHandle->xAES_Handle, xServerPacketInData.pcData);
				pxServerHandle->bIsAuthed = 1;
			};
		}
		else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE);
	}
	else if (xServerPacketAttr.eType == SGP2_PL_PT_MESSAGE)
	{
		if (pxServerHandle->bIsAuthed)
		{
			if ((xServerPacketInData.usSize >= SGP2_ML_ENCRYPTION_BLOCK_SIZE) && ((xServerPacketInData.usSize % SGP2_ML_ENCRYPTION_BLOCK_SIZE) == 0))
			{
				// Decrypt message:
				s = 0;
				while (s < xServerPacketInData.usSize)
				{
					AES_ECB_decrypt(&pxServerHandle->xAES_Handle, &xServerPacketInData.pcData[s]);
					s += SGP2_ML_ENCRYPTION_BLOCK_SIZE;
				};
				xInMessage.usCmd = usSGP2_GetShortFromArray (&xServerPacketInData.pcData[0]);
				xInMessage.ucPar = xServerPacketInData.pcData[2];
				xInMessage.usDataSize = usSGP2_GetShortFromArray (&xServerPacketInData.pcData[3]);
				xInMessage.usDataMaxSize = xInMessage.usDataSize;
				xInMessage.pcData = &xServerPacketInData.pcData[5];
				if ((xInMessage.usDataSize + 6) <= xServerPacketInData.usSize)
				{
					ucCS = 0;
					for (s = 0; s < xInMessage.usDataSize + 5; s ++) ucCS += xServerPacketInData.pcData[s];
					if (xServerPacketInData.pcData[s] == (0xFF - ucCS))
					{
						if (pxServerHandle->pxServerCB->pvProcessClientRequestMessage == 0) return 0;
						s = usSGP2_PL_PacketGetDataPointer   (&xServerPacket, &(pcDataPointer));
						if (s < 6) return 0;
						if (s < SGP2_ML_ENCRYPTION_BLOCK_SIZE) return 0 ;
						s = s / SGP2_ML_ENCRYPTION_BLOCK_SIZE;
						s = s * SGP2_ML_ENCRYPTION_BLOCK_SIZE;
						xOutMessage.usCmd = xInMessage.usCmd;
						xOutMessage.ucPar = xInMessage.ucPar;
						xOutMessage.pcData = pcDataPointer + 5;
						xOutMessage.usDataSize = 0;
						xOutMessage.usDataMaxSize = s - 6;
						pxServerHandle->pxServerCB->pvProcessClientRequestMessage(pvCB_Arg, pxServerHandle->cCurrentUser, &xInMessage, &xOutMessage);

						// Pack message in packet Data field:
						if (usSGP2_PL_PacketPutShort    (&xServerPacket, xOutMessage.usCmd) != 2) return 0;
						if (usSGP2_PL_PacketPutByte     (&xServerPacket, xOutMessage.ucPar) != 1) return 0;
						if (usSGP2_PL_PacketPutShort    (&xServerPacket, xOutMessage.usDataSize) != 2) return 0;
						if (usSGP2_PL_PacketPutIdleData (&xServerPacket, xOutMessage.usDataSize) != xOutMessage.usDataSize) return 0;
						// Add CS:
						ucCS = 0; for (s = 0; s < (xOutMessage.usDataSize + 5); s ++) ucCS += pcDataPointer[s];
						if (usSGP2_PL_PacketPutByte     (&xServerPacket, (0xFF - ucCS)) != 1) return 0;
						// Extend
						usTotalSize = xOutMessage.usDataSize + 6;
						while (usTotalSize % SGP2_ML_ENCRYPTION_BLOCK_SIZE)
						{
							usSGP2_PL_PacketPutByte (&xServerPacket, ucCS);
							usTotalSize ++;
						};
						// Encrypt message data:
						s = 0;
						while (usTotalSize >= SGP2_ML_ENCRYPTION_BLOCK_SIZE)
						{
							AES_ECB_encrypt(&pxServerHandle->xAES_Handle, &pcDataPointer[s]);
							s += SGP2_ML_ENCRYPTION_BLOCK_SIZE;
							usTotalSize -= SGP2_ML_ENCRYPTION_BLOCK_SIZE;
						};
					}
					else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE); // Decrypt error = key error = auth fault
				}
				else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE); // Decrypt error = key error = auth fault

			}
			else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_PACKET_DATA_FORMAT_ERROR);
		}
		else vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_AUTH_FAILURE);
	}
	else if (xServerPacketAttr.eType == SGP2_PL_PT_PING)
	{
		usSGP2_PL_PacketPutData (&xServerPacket, xServerPacketInData.pcData, xServerPacketInData.usSize);
	}
	else
	{
		vSGP2_PL_PacketSetRespErrorCode (&xServerPacket, SGP2_PL_RESPONSE_ERROR_CODE_OPERATION_PROHIBITED);
	};
	return usSGP2_PL_AssemblePacket(&xServerPacket);;
};
/***************************************************************************************************
* Function:    usSGP2_PL_ValidatePacket
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_ValidatePacket    (unsigned char* pcSrcData, unsigned short usSrcDataSize)
{
	unsigned short usParsedSize, s;
	if (pcSrcData == 0)       return 0;
	if ((pcSrcData[0] == LOCAL_ADDR_REQUEST_SYNC) || (pcSrcData[0] == LOCAL_ADDR_RESPONSE_SYNC))
	{
		if (usSrcDataSize < 7)  return 0;
		usParsedSize = 2;
	}
	else if ((pcSrcData[0] == GLOBAL_ADDR_REQUEST_SYNC) || (pcSrcData[0] == GLOBAL_ADDR_RESPONSE_SYNC))
	{
		if (usSrcDataSize < 10)  return 0;
		usParsedSize = 5;
	}
	else return 0;
	s = pcSrcData[usParsedSize] & 0x0F;
	if (!((s <= 3) || (s == 15)))  return 0;  // Validate packet type (en_SGP2_PL_PacketType)
	usParsedSize ++;
	s = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += 2;
	if ((usParsedSize + s + 2) > usSrcDataSize) return 0;
	usParsedSize += s;
	s = usGetCRC16(pcSrcData, usParsedSize);
	if (s != usSGP2_GetShortFromArray(&pcSrcData[usParsedSize])) return 0;
	return (usParsedSize + 2);
};
/***************************************************************************************************
* Function:    usSGP2_PL_ValidateRequestPacket
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_ValidateRequestPacket  (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstAddr)
{
	unsigned short usParsedSize, s;
	if (pcSrcData == 0)       return 0;
	if (pcSrcData[0] == LOCAL_ADDR_REQUEST_SYNC)
	{
		if (usSrcDataSize < 7)  return 0;
		if (piDstAddr) *piDstAddr = pcSrcData[1];
		usParsedSize = 2;
	}
	else if (pcSrcData[0] == GLOBAL_ADDR_REQUEST_SYNC)
	{
		if (usSrcDataSize < 10)  return 0;
		if (piDstAddr) *piDstAddr = uiSGP2_GetIntFromArray(&pcSrcData[1]);
		usParsedSize = 5;
	}
	else return 0;
	s = pcSrcData[usParsedSize] & 0x0F;
	if (!((s <= 3) || (s == 15)))  return 0;  // Validate packet type (en_SGP2_PL_PacketType)
	usParsedSize ++;
	s = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += 2;
	if ((usParsedSize + s + 2) > usSrcDataSize) return 0;
	usParsedSize += s;
	s = usGetCRC16(pcSrcData, usParsedSize);
	if (s != usSGP2_GetShortFromArray(&pcSrcData[usParsedSize])) return 0;
	return (usParsedSize + 2);
};
/***************************************************************************************************
* Function:    usSGP2_PL_ValidateResponsePacket
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_ValidateResponsePacket (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstAddr)
{
	unsigned short usParsedSize, s;
	if (pcSrcData == 0)       return 0;
	if (pcSrcData[0] == LOCAL_ADDR_RESPONSE_SYNC)
	{
		if (usSrcDataSize < 7)  return 0;
		if (piDstAddr) *piDstAddr = pcSrcData[1];
		usParsedSize = 2;
	}
	else if (pcSrcData[0] == GLOBAL_ADDR_RESPONSE_SYNC)
	{
		if (usSrcDataSize < 10)  return 0;
		if (piDstAddr) *piDstAddr = uiSGP2_GetIntFromArray(&pcSrcData[1]);
		usParsedSize = 5;
	}
	else return 0;
	s = pcSrcData[usParsedSize] & 0x0F;
	if (!((s <= 3) || (s == 15)))  return 0;  // Validate packet type (en_SGP2_PL_PacketType)
	usParsedSize ++;
	s = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += 2;
	if ((usParsedSize + s + 2) > usSrcDataSize) return 0;
	usParsedSize += s;
	s = usGetCRC16(pcSrcData, usParsedSize);
	if (s != usSGP2_GetShortFromArray(&pcSrcData[usParsedSize])) return 0;
	return (usParsedSize + 2);
};

/***************************************************************************************************
* Function:    usSGP2_PL_ExtractResponseHS_Data
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_ExtractResponseHS_Data (unsigned char* pcSrcData, unsigned short usSrcDataSize, unsigned int* piDstServerID, unsigned short* psDstBuffSize)
{
	unsigned short usParsedSize, s;
	if (pcSrcData == 0)       return 0;
	if (pcSrcData[0] == LOCAL_ADDR_RESPONSE_SYNC)
	{
		if (usSrcDataSize < 7)  return 0;
		usParsedSize = 2;
	}
	else if (pcSrcData[0] == GLOBAL_ADDR_RESPONSE_SYNC)
	{
		if (usSrcDataSize < 10)  return 0;
		usParsedSize = 5;
	}
	else return 0;
	s = pcSrcData[usParsedSize] & 0x0F;
	if (s != 0) return 0; // SGP2_PL_PT_HANDSHAKE;
	if (((pcSrcData[usParsedSize] >> 4) & 0x0F) != 0) return 0; // Error code = 0;
	usParsedSize ++;
	s = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += 2;
	if (s < 6) return 0; // HS data min size = 6
	if ((usParsedSize + s + 2) > usSrcDataSize) return 0;
	if (piDstServerID) *piDstServerID = uiSGP2_GetIntFromArray(&pcSrcData[usParsedSize + 2]);
	if (psDstBuffSize) *psDstBuffSize = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += s;
	s = usGetCRC16(pcSrcData, usParsedSize);
	if (s != usSGP2_GetShortFromArray(&pcSrcData[usParsedSize])) return 0;
	return (usParsedSize + 2);
};
/***************************************************************************************************
* Function:    usSGP2_PL_CreateGlobalPingRequest
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_CreateGlobalPingRequest (unsigned int   uiAddr, unsigned char* pcSrcPingData, unsigned short usSrcPingDataSize, unsigned char* pcDstDataArray, unsigned short usDstDataArraySize)
{
	unsigned short s;
	if (pcDstDataArray == 0) return 0;
	if ((usSrcPingDataSize) && (pcSrcPingData == 0)) return 0;
	if ((usSrcPingDataSize + 10) > usDstDataArraySize) return 0;
	pcDstDataArray[0] = GLOBAL_ADDR_REQUEST_SYNC;
	vSGP2_PutIntToArray(uiAddr, &pcDstDataArray[1]);
	pcDstDataArray[5] = (unsigned char)SGP2_PL_PT_PING;
	vSGP2_PutShortToArray(usSrcPingDataSize, &pcDstDataArray[6]);
	for (s = 0; s < usSrcPingDataSize; s ++) pcDstDataArray[8 + s] = pcSrcPingData[s]; 
	s = usGetCRC16(pcDstDataArray, (usSrcPingDataSize + 8));
	vSGP2_PutShortToArray(s, &pcDstDataArray[usSrcPingDataSize + 8]);
	return (usSrcPingDataSize + 10);
};
/***************************************************************************************************
							  LOCAL LIBRARY FUNCTIONS
***************************************************************************************************/
void vConvertPasswordToKey (const char* pcSrcPassword, unsigned char* pcDstKey, unsigned short usKeySize)
{
	unsigned short s;
	if (pcSrcPassword == 0) return;
	if (pcDstKey == 0) return;
	if (usKeySize == 0) return;

	s = 0;
	while (usKeySize)
	{
		usKeySize --;
		pcDstKey[usKeySize] = pcSrcPassword[s];
		s ++;
		if (pcSrcPassword[s] == 0) s = 0;
	};

};
//******************************************************************************//
unsigned short usCheckMessageFreeSize (str_SGP2_ML_Message* pxMessage)
{
	if (pxMessage == 0) return 0;
	if (pxMessage->pcData == 0) return 0;
	if (pxMessage->usDataMaxSize <= pxMessage->usDataSize) return 0;
	return (pxMessage->usDataMaxSize - pxMessage->usDataSize);
};
//******************************************************************************//
// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states.
#define Nb 4        // The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nk 4        // The number of 32 bit words in a key.
#define Nr 10       // The number of rounds in AES Cipher.
// The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static const uint8_t Rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };
static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

#define getSBoxValue(num) (sbox[(num)])
static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
#define getSBoxInvert(num) (rsbox[(num)])
typedef uint8_t state_t[4][4];
static void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key)
{
	unsigned i, j, k;
	uint8_t tempa[4]; // Used for the column/row operations

	// The first round key is the key itself.
	for (i = 0; i < Nk; ++i)
	{
		RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
		RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
		RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
		RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
	}

	// All other round keys are found from the previous round keys.
	for (i = Nk; i < Nb * (Nr + 1); ++i)
	{
		{
		k = (i - 1) * 4;
		tempa[0]=RoundKey[k + 0];
		tempa[1]=RoundKey[k + 1];
		tempa[2]=RoundKey[k + 2];
		tempa[3]=RoundKey[k + 3];
		}

		if (i % Nk == 0)
		{
			// This function shifts the 4 bytes in a word to the left once.
			// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

			// Function RotWord()
			{
				const uint8_t u8tmp = tempa[0];
				tempa[0] = tempa[1];
				tempa[1] = tempa[2];
				tempa[2] = tempa[3];
				tempa[3] = u8tmp;
			}

			// SubWord() is a function that takes a four-byte input word and 
			// applies the S-box to each of the four bytes to produce an output word.

			// Function Subword()
			{
				tempa[0] = getSBoxValue(tempa[0]);
				tempa[1] = getSBoxValue(tempa[1]);
				tempa[2] = getSBoxValue(tempa[2]);
				tempa[3] = getSBoxValue(tempa[3]);
			}

			tempa[0] = tempa[0] ^ Rcon[i/Nk];
		}

		j = i * 4; k=(i - Nk) * 4;
		RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
		RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
		RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
		RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
	}
}
static uint8_t xtime(uint8_t x)
{
	return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}
//******************************************************************************//
void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key)
{
  KeyExpansion(ctx->RoundKey, key);
}
//******************************************************************************//
// MixColumns function mixes the columns of the state matrix
static void MixColumns(state_t* state)
{
	uint8_t i;
	uint8_t Tmp, Tm, t;
	for (i = 0; i < 4; ++i)
	{
		t   = (*state)[i][0];
		Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
		Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
		Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
		Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
		Tm  = (*state)[i][3] ^ t ;              Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
	}
}
// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows(state_t* state)
{
  uint8_t temp;

  // Rotate first row 1 columns to left  
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;

  // Rotate second row 2 columns to left  
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp           = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp           = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}
// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void SubBytes(state_t* state)
{
  uint8_t i, j;
  for (i = 0; i < 4; ++i)
  {
	for (j = 0; j < 4; ++j)
	{
	  (*state)[j][i] = getSBoxValue((*state)[j][i]);
	}
  }
}
// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void AddRoundKey(uint8_t round, state_t* state, const uint8_t* RoundKey)
{
  uint8_t i,j;
  for (i = 0; i < 4; ++i)
  {
	for (j = 0; j < 4; ++j)
	{
	  (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
	}
  }
}
// Cipher is the main function that encrypts the PlainText.
static void Cipher(state_t* state, const uint8_t* RoundKey)
{
  uint8_t round;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(0, state, RoundKey);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without MixColumns()
  for (round = 1; ; ++round)
  {
	SubBytes(state);
	ShiftRows(state);
	if (round == Nr) {
	  break;
	}
	MixColumns(state);
	AddRoundKey(round, state, RoundKey);
  }
  // Add round key to last round
  AddRoundKey(Nr, state, RoundKey);
}
void AES_ECB_encrypt(const struct AES_ctx* ctx, uint8_t* buf)
{
  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher((state_t*)buf, ctx->RoundKey);
}
//******************************************************************************//
static uint8_t Multiply(unsigned char x, unsigned char y)
{
	return ((((y >> 0) & 1) * x) ^                        \
	  (((y >> 1) & 1) * xtime(x)) ^                       \
	  (((y >> 2) & 1) * xtime(xtime(x))) ^                \
	  (((y >> 3) & 1) * xtime(xtime(xtime(x)))) ^         \
	  (((y >> 4) & 1) * xtime(xtime(xtime(xtime(x))))));
};
// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
static void InvMixColumns(state_t* state)
{
	int i;
	uint8_t a, b, c, d;
	for (i = 0; i < 4; ++i)
	{
		a = (*state)[i][0];
		b = (*state)[i][1];
		c = (*state)[i][2];
		d = (*state)[i][3];

		(*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		(*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		(*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		(*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	};
};
// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void InvSubBytes(state_t* state)
{
  uint8_t i, j;
  for (i = 0; i < 4; ++i)
  {
	for (j = 0; j < 4; ++j)
	{
	  (*state)[j][i] = getSBoxInvert((*state)[j][i]);
	}
  }
}
static void InvShiftRows(state_t* state)
{
  uint8_t temp;

  // Rotate first row 1 columns to right  
  temp = (*state)[3][1];
  (*state)[3][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[0][1];
  (*state)[0][1] = temp;

  // Rotate second row 2 columns to right 
  temp = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to right
  temp = (*state)[0][3];
  (*state)[0][3] = (*state)[1][3];
  (*state)[1][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[3][3];
  (*state)[3][3] = temp;
}
static void InvCipher(state_t* state, const uint8_t* RoundKey)
{
  uint8_t round;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(Nr, state, RoundKey);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without InvMixColumn()
  for (round = (Nr - 1); ; --round)
  {
	InvShiftRows(state);
	InvSubBytes(state);
	AddRoundKey(round, state, RoundKey);
	if (round == 0) {
	  break;
	}
	InvMixColumns(state);
  }

}
void AES_ECB_decrypt(const struct AES_ctx* ctx, uint8_t* buf)
{
  // The next function call decrypts the PlainText with the Key using AES algorithm.
  InvCipher((state_t*)buf, ctx->RoundKey);
}

unsigned short usGetPacketFreeSpace (str_SGP2_PL_PacketHandle* pxHandle)
{
	if (pxHandle->usSize >= (pxHandle->xData.usSize - 2)) return 0;
	return (pxHandle->xData.usSize - (pxHandle->usSize + 2));
};

unsigned short usGetCRC16 (unsigned char *pcSrcArray, unsigned short usArraySize)
{
	unsigned int i, uiOffset = 0;
	unsigned short usResult = 0xFFFF;
	unsigned char  ucTemp, c;
	for(i = uiOffset; i < uiOffset + usArraySize; i ++)
	{
		usResult ^= pcSrcArray[i];
		for(c = 0; c < 8; c ++)
		{
			ucTemp = (unsigned char)(usResult & 1);
			usResult >>= 1;
			if(ucTemp != 0) usResult ^= 0xD175;
		};
	}
	return usResult;
};
/***************************************************************************************************
* Function:    vSGP2_PL_InitPacket
* Description:
****************************************************************************************************/
void vSGP2_PL_InitPacket (str_SGP2_PL_PacketHandle* pxHandle, str_SGP2_PL_PacketAttr* pxSrcAttr, unsigned char* pcDstDataArray, unsigned short usDstDataArraySize)
{
	if (pxHandle == 0) return;
	if (pxSrcAttr == 0) return;
	if (pcDstDataArray == 0) return;

	if (pxSrcAttr->eConnectionType == SGP2_PL_CT_LOCAL)
	{
		if (usDstDataArraySize < 7)  return;
		pcDstDataArray[0] = LOCAL_ADDR_REQUEST_SYNC;
		if (pxSrcAttr->eDirection == SGP2_PL_PD_RESPONSE) pcDstDataArray[0] = LOCAL_ADDR_RESPONSE_SYNC;
		pcDstDataArray[1] = (unsigned char)(pxSrcAttr->uiServerAddress);
		pxHandle->usSize = 2;
	}
	else if (pxSrcAttr->eConnectionType == SGP2_PL_CT_GLOBAL)
	{
		if (usDstDataArraySize < 10) return;
		pcDstDataArray[0] = GLOBAL_ADDR_REQUEST_SYNC;
		if (pxSrcAttr->eDirection == SGP2_PL_PD_RESPONSE) pcDstDataArray[0] = GLOBAL_ADDR_RESPONSE_SYNC;
		vSGP2_PutIntToArray(pxSrcAttr->uiServerAddress, &pcDstDataArray[1]);
		pxHandle->usSize = 5;
	}
	else return;

	pxHandle->xData.pcData = pcDstDataArray;
	pxHandle->xData.usSize = usDstDataArraySize;
	// Header:
	pxHandle->xData.pcData[pxHandle->usSize] = (unsigned char)(pxSrcAttr->eType);
	pxHandle->xData.pcData[pxHandle->usSize] |= ((pxSrcAttr->ucResposeErrorCode & 0x0F) << 4);
	// Default packet data size (0):
	pxHandle->xData.pcData[pxHandle->usSize + 1] = 0;
	pxHandle->xData.pcData[pxHandle->usSize + 2] = 0;

	pxHandle->usSize += 3;
};
/***************************************************************************************************
* Function:    vSGP2_PL_PacketSetRespErrorCode
* Description:
****************************************************************************************************/
void vSGP2_PL_PacketSetRespErrorCode (str_SGP2_PL_PacketHandle* pxHandle, unsigned char ucResponseErrorCode)
{
	unsigned short s;
	if (pxHandle == 0) return;
	if (pxHandle->xData.pcData == 0) return;
	if (pxHandle->xData.pcData[0] == LOCAL_ADDR_RESPONSE_SYNC) {s = 2;}
	else if (pxHandle->xData.pcData[0] == GLOBAL_ADDR_RESPONSE_SYNC) {s = 5;}
	else return;
	pxHandle->xData.pcData[s] &= 0x0F;
	pxHandle->xData.pcData[s] |= (ucResponseErrorCode & 0x0F) << 4;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketGetDataPointer
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketGetDataPointer   (str_SGP2_PL_PacketHandle* pxHandle, unsigned char** ppcDstPointer)
{
	unsigned short s;
	if (pxHandle == 0) return 0;
	if (pxHandle->xData.pcData == 0) return 0;
	s = usGetPacketFreeSpace(pxHandle);
	if (!s) return 0;
	*ppcDstPointer = &pxHandle->xData.pcData[pxHandle->usSize];
	return s;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutData
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutData  (str_SGP2_PL_PacketHandle* pxHandle, void* pvSrcData, unsigned short usDataSize)
{
	unsigned short s;
	if (pxHandle == 0)  return 0;
	if (pvSrcData == 0) return 0;
	// Get packet free space:
	s = usGetPacketFreeSpace(pxHandle);
	if (usDataSize > s) usDataSize = s;
	for (s = 0; s < usDataSize; s ++) pxHandle->xData.pcData[pxHandle->usSize + s] = *(((unsigned char*)(pvSrcData)) + s);
	pxHandle->usSize += s;
	return s;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutIdleData
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutIdleData      (str_SGP2_PL_PacketHandle* pxHandle, unsigned short usDataSize)
{
	unsigned short s;
	if (pxHandle == 0)  return 0;
	// Get packet free space:
	s = usGetPacketFreeSpace(pxHandle);
	if (usDataSize > s) usDataSize = s;
	pxHandle->usSize += usDataSize;
	return usDataSize;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutString
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutString (str_SGP2_PL_PacketHandle* pxHandle, char* pcString)
{
	unsigned short s, usFree;
	if (pxHandle == 0) return 0;
	if (pcString == 0) return 0;
	usFree = usGetPacketFreeSpace(pxHandle);
	if (usFree < 1) return 0;
	for (s = 0; s < (usFree - 1); s ++)
	{
		if (pcString[s] == 0) break;
		pxHandle->xData.pcData[pxHandle->usSize + s] = (unsigned char)(pcString[s]);
	};
	pxHandle->xData.pcData[pxHandle->usSize + s] = 0;
	pxHandle->usSize += (s + 1);
	return (s + 1);
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutByte
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutByte  (str_SGP2_PL_PacketHandle* pxHandle, unsigned char ucByte)
{
	if (pxHandle == 0) return 0;
	if (usGetPacketFreeSpace(pxHandle) < 1) return 0;
	pxHandle->xData.pcData[pxHandle->usSize ++] = ucByte;
	return 1;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutShort
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutShort (str_SGP2_PL_PacketHandle* pxHandle, unsigned short usShort)
{
	if (pxHandle == 0) return 0;
	if (usGetPacketFreeSpace(pxHandle) < 2) return 0;
	vSGP2_PutShortToArray(usShort, &pxHandle->xData.pcData[pxHandle->usSize]);
	pxHandle->usSize += 2;
	return 2;
};
/***************************************************************************************************
* Function:    usSGP2_PL_PacketPutInt
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_PacketPutInt   (str_SGP2_PL_PacketHandle* pxHandle, unsigned int uiInt)
{
	if (pxHandle == 0) return 0;
	if (usGetPacketFreeSpace(pxHandle) < 4) return 0;
	vSGP2_PutIntToArray(uiInt, &pxHandle->xData.pcData[pxHandle->usSize]);
	pxHandle->usSize += 4;
	return 4;
};
/***************************************************************************************************
* Function:    usSGP2_PL_AssemblePacket
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_AssemblePacket   (str_SGP2_PL_PacketHandle* pxHandle)
{
	unsigned short s;
	if (pxHandle == 0) return 0;
	// Get packet data fact size:
	if ((pxHandle->xData.pcData[0] == LOCAL_ADDR_REQUEST_SYNC) || (pxHandle->xData.pcData[0] == LOCAL_ADDR_RESPONSE_SYNC))
	{
		s = pxHandle->usSize - 5;
		vSGP2_PutShortToArray(s, &pxHandle->xData.pcData[3]);
	}
	else if ((pxHandle->xData.pcData[0] == GLOBAL_ADDR_REQUEST_SYNC) || (pxHandle->xData.pcData[0] == GLOBAL_ADDR_RESPONSE_SYNC))
	{
		s = pxHandle->usSize - 8;
		vSGP2_PutShortToArray(s, &pxHandle->xData.pcData[6]);
	}
	else return 0;
	// Add CRC-16:
	s =  usGetCRC16 (pxHandle->xData.pcData, pxHandle->usSize);
	vSGP2_PutShortToArray(s, &pxHandle->xData.pcData[pxHandle->usSize]);
	return (pxHandle->usSize + 2);
};

/***************************************************************************************************
* Function:    usSGP2_PL_DecodePacket
* Description:
****************************************************************************************************/
unsigned short usSGP2_PL_DecodePacket    (unsigned char* pcSrcData, unsigned short usSrcDataSize, str_SGP2_PL_PacketAttr* pxDstAttr, str_SGP2_PL_PacketData* pxDstPacketData)
{
	unsigned short usParsedSize, s;
	if (pcSrcData == 0)       return 0;
	if (pxDstAttr == 0)       return 0;
	if (pxDstPacketData == 0) return 0;
	if ((pcSrcData[0] == LOCAL_ADDR_REQUEST_SYNC) || (pcSrcData[0] == LOCAL_ADDR_RESPONSE_SYNC))
	{
		if (usSrcDataSize < 7)  return 0;
		pxDstAttr->eConnectionType = SGP2_PL_CT_LOCAL;
		pxDstAttr->eDirection = SGP2_PL_PD_REQUEST;
		if (pcSrcData[0] == LOCAL_ADDR_RESPONSE_SYNC) pxDstAttr->eDirection = SGP2_PL_PD_RESPONSE;
		pxDstAttr->uiServerAddress = pcSrcData[1];
		usParsedSize = 2;
	}
	else if ((pcSrcData[0] == GLOBAL_ADDR_REQUEST_SYNC) || (pcSrcData[0] == GLOBAL_ADDR_RESPONSE_SYNC))
	{
		if (usSrcDataSize < 10)  return 0;
		pxDstAttr->eConnectionType = SGP2_PL_CT_GLOBAL;
		pxDstAttr->eDirection = SGP2_PL_PD_REQUEST;
		if (pcSrcData[0] == GLOBAL_ADDR_RESPONSE_SYNC) pxDstAttr->eDirection = SGP2_PL_PD_RESPONSE;
		pxDstAttr->uiServerAddress = uiSGP2_GetIntFromArray(&pcSrcData[1]);
		usParsedSize = 5;
	}
	else return 0;
	s = pcSrcData[usParsedSize] & 0x0F;
	if      (s == 0)  {pxDstAttr->eType = SGP2_PL_PT_HANDSHAKE;}
	else if (s == 1)  {pxDstAttr->eType = SGP2_PL_PT_AUTH1;}
	else if (s == 2)  {pxDstAttr->eType = SGP2_PL_PT_AUTH2;}
	else if (s == 3)  {pxDstAttr->eType = SGP2_PL_PT_MESSAGE;}
	else if (s == 15) {pxDstAttr->eType = SGP2_PL_PT_PING;}
	else return 0;
	pxDstAttr->ucResposeErrorCode = (pcSrcData[usParsedSize] >> 4) & 0x0F;
	usParsedSize ++;
	pxDstPacketData->usSize = usSGP2_GetShortFromArray(&pcSrcData[usParsedSize]);
	usParsedSize += 2;
	if ((usParsedSize + pxDstPacketData->usSize + 2) > usSrcDataSize) return 0;
	pxDstPacketData->pcData = &pcSrcData[usParsedSize];
	usParsedSize += pxDstPacketData->usSize;
	s = usGetCRC16(pcSrcData, usParsedSize);
	if (s != usSGP2_GetShortFromArray(&pcSrcData[usParsedSize])) return 0;
	return (usParsedSize + 2);
};


