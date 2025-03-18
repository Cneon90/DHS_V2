/*******************************************************************************
                                  INCLUDES
*******************************************************************************/
#include "ProtocolsLib.h"
/*******************************************************************************
                                  DEFINES
*******************************************************************************/
#ifndef NULL
#define NULL 0
#endif
/*******************************************************************************
                           LOCAL FUNCTIONS DEFINES
*******************************************************************************/
unsigned short usGetMidepMessageCRC (unsigned char *pcSrcArray, unsigned short usArraySize);
unsigned short usGet_PL_CRC16 (unsigned char *pcSrcArray, unsigned short usSrcArraySize, unsigned short usPoly);
/*******************************************************************************
                                  FUNCTIONS:
*******************************************************************************/
/******************************************************************************/
void vProtocolsLibMidepInitMessage (str_MidepMessage* pxMessage, unsigned int uiServerID, unsigned char ucCommand, unsigned char ucSubCommand, unsigned char* pcStackData, unsigned short usStackDataSize)
{
	if (pxMessage == NULL)    return;
	if (pcStackData == NULL)  return;
	if (usStackDataSize < 10) return;
	
	pxMessage->ucMessageDataHeap = pcStackData;
	pxMessage->usMessageDataHeapMaxSize = usStackDataSize;
	
	// Save attributes:
	pxMessage->uiServerID         = uiServerID;
	pxMessage->ucCommand          = ucCommand;
	pxMessage->ucSubCommand       = ucSubCommand;
	pxMessage->ucTransactionIndex = 0;
	pxMessage->ucResultCode       = 0;
	
	// No data blocks in init state:
	pxMessage->ucDataBlocksCount = 0;
};
/******************************************************************************/
unsigned short usProtocolsLibMidepCompileMessage (str_MidepMessage* pxMessage)
{
	unsigned short s, usLenght;
	unsigned char  c;
	
	if (pxMessage == NULL) return 0;
	if (pxMessage->ucMessageDataHeap == 0) return 0;
	if (pxMessage->usMessageDataHeapMaxSize < 10) return 0;
	
	usLenght = 0;
	
	// Server ID:
	pxMessage->ucMessageDataHeap[usLenght ++] = (unsigned char)(pxMessage->uiServerID >> 24);
	pxMessage->ucMessageDataHeap[usLenght ++] = (unsigned char)(pxMessage->uiServerID >> 16);
	pxMessage->ucMessageDataHeap[usLenght ++] = (unsigned char)(pxMessage->uiServerID >> 8);
	pxMessage->ucMessageDataHeap[usLenght ++] = (unsigned char)(pxMessage->uiServerID >> 0);
	
	// Command:
	pxMessage->ucMessageDataHeap[usLenght ++] = pxMessage->ucCommand; 
	
	// Subcommand:
	pxMessage->ucMessageDataHeap[usLenght ++] = pxMessage->ucSubCommand; 
	
	// TRI/RSL:
    pxMessage->ucMessageDataHeap[usLenght ++] = ((unsigned char)(pxMessage->ucTransactionIndex << 6)) | (pxMessage->ucResultCode & 0x3F);
	
	// Data blocks:
	for (c = 0; c < pxMessage->ucDataBlocksCount; c ++)
	{
		pxMessage->ucMessageDataHeap[usLenght ++] = pxMessage->xDataBlock[c].ucSize;
		usLenght += pxMessage->xDataBlock[c].ucSize;
	};
	
	// No more data blocks:
	pxMessage->ucMessageDataHeap[usLenght ++] = 0;
	
	// CRC:
	s = usGetMidepMessageCRC(pxMessage->ucMessageDataHeap, usLenght);
	pxMessage->ucMessageDataHeap[usLenght ++]  = (unsigned char)(s >> 8);
	pxMessage->ucMessageDataHeap[usLenght ++]  = (unsigned char)(s >> 0);
	
	// Result:
	return usLenght;
};
/******************************************************************************/
void vMidepInitMessage(str_MidepMessage* pxMessage, unsigned int uiServerID, unsigned char ucCommand, unsigned char ucSubCommand, unsigned char* pcStackData, unsigned short usStackDataSize)
{
	if (pxMessage == NULL)    return;
	if (pcStackData == NULL)  return;
	if (usStackDataSize < 10) return;
	
	pxMessage->ucMessageDataHeap = pcStackData;
	pxMessage->usMessageDataHeapMaxSize = usStackDataSize;
	
//	// Server ID:
//	pxMessage->ucMessageDataHeap[0] = (unsigned char)(uiServerID >> 24);
//	pxMessage->ucMessageDataHeap[1] = (unsigned char)(uiServerID >> 16);
//	pxMessage->ucMessageDataHeap[2] = (unsigned char)(uiServerID >> 8);
//	pxMessage->ucMessageDataHeap[3] = (unsigned char)(uiServerID >> 0);
//	
//	// Command:
//	pxMessage->ucMessageDataHeap[4] = ucCommand;
//	
//	// Subcommand:
//	pxMessage->ucMessageDataHeap[5] = ucSubCommand;
//	
//	// Transaction index & result (default values zero)
//	pxMessage->ucMessageDataHeap[6] = 0;
//	
	// Save attributes:
	pxMessage->uiServerID         = uiServerID;
	pxMessage->ucCommand          = ucCommand;
	pxMessage->ucSubCommand       = ucSubCommand;
	pxMessage->ucTransactionIndex = 0;
	pxMessage->ucResultCode       = 0;
	
	// No data blocks in init state:
	pxMessage->ucDataBlocksCount = 0;
};
/******************************************************************************/
unsigned int uiProtocolsLibMidepDecodeMessage (unsigned char* pcSrcData, unsigned short usSrcDataLenght, str_MidepMessage* pxDstMessage)
{
	unsigned short s;
	if (pxDstMessage == NULL) return 0;
	if (pcSrcData == NULL) return 0;
	if (usSrcDataLenght < 10) return 0;
	
	// Check null-block:
	if (pcSrcData[usSrcDataLenght - 3] != 0) return 0;
	
	// Check CRC:
	s = usGetMidepMessageCRC(pcSrcData, (usSrcDataLenght - 2));
	if (pcSrcData[usSrcDataLenght - 2] != ((unsigned char)(s >> 8))) return 0;
	if (pcSrcData[usSrcDataLenght - 1] != ((unsigned char)(s >> 0))) return 0;
	
	// Fill attributes:
	pxDstMessage->uiServerID   = pcSrcData[0];
	pxDstMessage->uiServerID   = pxDstMessage->uiServerID << 8;
	pxDstMessage->uiServerID  += pcSrcData[1];
	pxDstMessage->uiServerID   = pxDstMessage->uiServerID << 8;
	pxDstMessage->uiServerID  += pcSrcData[2];
	pxDstMessage->uiServerID   = pxDstMessage->uiServerID << 8;
	pxDstMessage->uiServerID  += pcSrcData[3];
	pxDstMessage->ucCommand    = pcSrcData[4];
	pxDstMessage->ucSubCommand = pcSrcData[5];
	pxDstMessage->ucResultCode = pcSrcData[6] & 0x3F;
	pxDstMessage->ucTransactionIndex = (unsigned char)(pcSrcData[6] >> 6);

	// Data blocks:
	s = 7;
	pxDstMessage->ucDataBlocksCount = 0;
	while (pcSrcData[s])
	{
		if (pxDstMessage->ucDataBlocksCount >= MIDEP_DATA_BLOCKS_MAX_COUNT) return 0;
		if (s >= usSrcDataLenght) return 0;
		pxDstMessage->xDataBlock[pxDstMessage->ucDataBlocksCount].ucSize = pcSrcData[s];
		pxDstMessage->xDataBlock[pxDstMessage->ucDataBlocksCount].pcData = &pcSrcData[s + 1];
		s += (pxDstMessage->xDataBlock[pxDstMessage->ucDataBlocksCount].ucSize + 1);
		pxDstMessage->ucDataBlocksCount ++;
	};
	
	// Success:
	return usSrcDataLenght;
};
/******************************************************************************/
unsigned int   uiProtocolsLibMidepGetIntFormArray(unsigned char *pcArray)
{
	unsigned int uiResult = 0;
	if (pcArray == NULL) return uiResult;
	uiResult   = pcArray[0];
	uiResult   = uiResult << 8;
	uiResult  += pcArray[1];
	uiResult   = uiResult << 8;
	uiResult  += pcArray[2];
	uiResult   = uiResult << 8;
	uiResult  += pcArray[3];
	return uiResult;
};
/******************************************************************************/
unsigned short usGetMidepMessageCRC (unsigned char *pcSrcArray, unsigned short usArraySize)
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
			if(ucTemp != 0) usResult ^= 0xB309;
		};
	}
	return usResult;

};
//---------------------------------------------------------------------------
unsigned short usGet_PL_CRC16 (unsigned char *pcSrcArray, unsigned short usSrcArraySize, unsigned short usPoly)
{
	unsigned short s, usResultCRC16 = 0xFFFF;
    unsigned char ucTemp, c;
	for (s = 0; s < usSrcArraySize; s ++)
    {
		usResultCRC16 ^= pcSrcArray[s];
        for (c = 0; c < 8; c++)
        {
			ucTemp = (unsigned char)(usResultCRC16 & 1);
            usResultCRC16 >>= 1;
            if(ucTemp != 0) usResultCRC16 ^= usPoly;
        };
	};
	return usResultCRC16;
};
/***************************************************************************************************
* Function:    usProtocolsLibSGP_DecodeShuttle
* Description: In case of success, returns count of parsed bytes. Otherwise returns 0.
****************************************************************************************************/
unsigned short usProtocolsLibSGP_DecodeShuttle (unsigned char* pcSrcData, unsigned short usSrcDataSize, str_SGP_Shuttle *pxDstSPG_Shuttle)
{
	unsigned short s;
	if (pcSrcData == NULL) return 0;
	if (pxDstSPG_Shuttle == NULL) return 0;
	if (usSrcDataSize < 6) return 0;

	if ((pcSrcData[0] == 0x55) || (pcSrcData[0] == 0xAA)) // Local address
	{
		//pxDstSPG_Shuttle->ucAddressType = HF_SGP_ADDR_TYPE_LOCAL;
		s = 2;
	}
	else  if ((pcSrcData[0] == 0x77) || (pcSrcData[0] == 0x88)) // Global address
	{
		//pxDstSPG_Shuttle->ucAddressType = HF_SGP_ADDR_TYPE_GLOBAL;
		s = 5;
	}
	else return 0;

	// Data heap:
	pxDstSPG_Shuttle->pcDataHeap = pcSrcData;
	pxDstSPG_Shuttle->usDataHeapSize = usSrcDataSize;

	// Check CRC:
	if (usGet_PL_CRC16(pcSrcData, (usSrcDataSize - 2), 0xD175) != *((unsigned short*)(&pcSrcData[usSrcDataSize - 2]))) return 0;

	// Address:
	/*pxDstSPG_Shuttle->uiServerAddress = pcSrcData[1];
	if (pxDstSPG_Shuttle->ucAddressType == HF_SGP_ADDR_TYPE_GLOBAL)
	{
		pxDstSPG_Shuttle->uiServerAddress  = pcSrcData[4];
		pxDstSPG_Shuttle->uiServerAddress  = pxDstSPG_Shuttle->uiServerAddress << 8;
		pxDstSPG_Shuttle->uiServerAddress += pcSrcData[3];
		pxDstSPG_Shuttle->uiServerAddress  = pxDstSPG_Shuttle->uiServerAddress << 8;
		pxDstSPG_Shuttle->uiServerAddress += pcSrcData[2];
		pxDstSPG_Shuttle->uiServerAddress  = pxDstSPG_Shuttle->uiServerAddress << 8;
		pxDstSPG_Shuttle->uiServerAddress += pcSrcData[1];
	};  */
	// Index:
	pxDstSPG_Shuttle->ucIndex = pcSrcData[s];
	// Data:
	//pxDstSPG_Shuttle->pcLoadData = &pcSrcData[s + 1];
	pxDstSPG_Shuttle->usLoadIndex = s + 1;
	// Total size:
	pxDstSPG_Shuttle->usTotalSize = usSrcDataSize - 2;
	// Data len:
	//pxDstSPG_Shuttle->usLoadSize = usSrcDataSize - s - 3;
	// Success:
	return usSrcDataSize;
};
/***************************************************************************************************
* Function:    usProtocolsLibSGP_CreateEstConShuttle
* Description: Create SGP create establish connection shuttle
* Retval:      Number of bytes
****************************************************************************************************/
unsigned short usProtocolsLibSGP_CreateEstConShuttle (str_SGP_Shuttle *pxDstSPG_Shuttle, unsigned int uiServerID, unsigned short usInBuffSize, unsigned char* pcStackData, unsigned short usStackDataSize)
{
	unsigned short usCRC;
	if (pxDstSPG_Shuttle == NULL) return 0;
	if (pcStackData == NULL)      return 0;
	if (usStackDataSize < 11)     return 0;

	pxDstSPG_Shuttle->pcDataHeap = pcStackData;
	pxDstSPG_Shuttle->usDataHeapSize = usStackDataSize;

	pcStackData[0] = 0x77;  // Global address
	pcStackData[1] = (unsigned char)(uiServerID >> 0);
	pcStackData[2] = (unsigned char)(uiServerID >> 8);
	pcStackData[3] = (unsigned char)(uiServerID >> 16);
	pcStackData[4] = (unsigned char)(uiServerID >> 24);
	pxDstSPG_Shuttle->ucIndex = 0; // Set connection type
	pcStackData[5] = pxDstSPG_Shuttle->ucIndex;
	pxDstSPG_Shuttle->usLoadIndex = 6;
	pcStackData[6] = (unsigned char)(usInBuffSize >> 0);
	pcStackData[7] = (unsigned char)(usInBuffSize >> 8);
	pcStackData[8] = SGP_IFC_LAN_TCP_SERVER;
	pxDstSPG_Shuttle->usTotalSize = 9;
	usCRC = usGet_PL_CRC16(pxDstSPG_Shuttle->pcDataHeap, 9, 0xD175);
	pcStackData[9]  = (unsigned char)(usCRC >> 0);
	pcStackData[10] = (unsigned char)(usCRC >> 8);
	return  (pxDstSPG_Shuttle->usTotalSize + 2);
};
/***************************************************************************************************
* Function:    usProtocolsLibSGP_CompileShuttle
* Description: Calc CRC and add to endof shuttle
****************************************************************************************************/
unsigned short usProtocolsLibSGP_CompileShuttle (str_SGP_Shuttle *pxDstSPG_Shuttle)
{
	unsigned short usCRC;
	if (pxDstSPG_Shuttle == NULL) return 0;
	if ((pxDstSPG_Shuttle->usTotalSize + 2) > pxDstSPG_Shuttle->usDataHeapSize) return 0;
	if (pxDstSPG_Shuttle->pcDataHeap == NULL) return 0;

	usCRC = usGet_PL_CRC16(pxDstSPG_Shuttle->pcDataHeap, pxDstSPG_Shuttle->usTotalSize, 0xD175);

	pxDstSPG_Shuttle->pcDataHeap[pxDstSPG_Shuttle->usTotalSize + 0] = (unsigned char)(usCRC >> 0);
	pxDstSPG_Shuttle->pcDataHeap[pxDstSPG_Shuttle->usTotalSize + 1] = (unsigned char)(usCRC >> 8);

	return (pxDstSPG_Shuttle->usTotalSize + 2);
};
/***************************************************************************************************
* Function:    ucProtocolsLibSGP_ExractAddress
* Description: Extract address value from shuttle
* Retval:      Type of address or 0 in case of fail
****************************************************************************************************/
unsigned char  ucProtocolsLibSGP_ExractAddress (str_SGP_Shuttle *pxSrcSPG_Shuttle, unsigned int* piDstAddress)
{
	unsigned int uiAddress;
	if (pxSrcSPG_Shuttle == NULL) return 0;
	if (pxSrcSPG_Shuttle->pcDataHeap == NULL) return 0;
	if (piDstAddress == NULL) return 0;
	if (pxSrcSPG_Shuttle->usLoadIndex == 3)
	{
		*piDstAddress = (unsigned int)(pxSrcSPG_Shuttle->pcDataHeap[1]);
		return HF_SGP_ADDR_TYPE_LOCAL;
	}
	else if (pxSrcSPG_Shuttle->usLoadIndex == 6)
	{
		uiAddress  = pxSrcSPG_Shuttle->pcDataHeap[4];
		uiAddress  = uiAddress << 8;
		uiAddress += pxSrcSPG_Shuttle->pcDataHeap[3];
		uiAddress  = uiAddress << 8;
		uiAddress += pxSrcSPG_Shuttle->pcDataHeap[2];
		uiAddress  = uiAddress << 8;
		uiAddress += pxSrcSPG_Shuttle->pcDataHeap[1];
		*piDstAddress = uiAddress;
		return HF_SGP_ADDR_TYPE_GLOBAL;
	}
	else return 0;
}

