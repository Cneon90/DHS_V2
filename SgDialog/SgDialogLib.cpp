//---------------------------------------------------------------------------
/***************************************************************************************************
										INCLUDES
***************************************************************************************************/
#include "SgDialogLib.h"
/***************************************************************************************************
										DEFINES
***************************************************************************************************/
#define DEFAULT_BUF_SIZE   256

#pragma hdrstop

#include "SgDialogLib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


void vSgDialogUpdateCRC16 (unsigned short *psCurrentValue, unsigned char ucByte);
/***************************************************************************************************
                                    LIBRARY FUNCTIONS
***************************************************************************************************/
/***************************************************************************************************
* Function:    iSgDialogCreateNewDialog
* Description: Start handle initialisation
****************************************************************************************************/
int iSgDialogCreateNewDialog (const strSgDialogCB* pxCB, void* pvCB_Arg)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    unsigned short usCRC16;
    int iOffset = 0;
    int i;

    if (pxCB == 0) return -1;

    usCRC16 = 0xFFFF;

    // TS (optional)[0..3] = 0 & rezerved[6..9] =0;
    for (i = 0; i < 10; i ++) ucBuf[i] = 0;
    ucBuf[4]  = SG_LIB_VERSION_H;
    ucBuf[5]  = SG_LIB_VERSION_L;
    ucBuf[10] = (unsigned char)SDIT_EMPTY;

    if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 11) != 11) return -1;
    for (i = 0; i < 11; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    iOffset += 11;

    if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)&usCRC16, iOffset, sizeof(usCRC16)) != sizeof(usCRC16)) return -1;

    return 0;
};

/***************************************************************************************************
* Function:    iSgDialogReadItemsInit
* Description:
****************************************************************************************************/
int iSgDialogReadItemsInit    (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogReadListHandle* pxHandle)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    unsigned short usCRC16;
    int iOffset = 0;
    int i;
    unsigned int ui;
    int iItemsCount = 0;
    strSgDialogItem xItem;

    usCRC16 = 0xFFFF;

    if (pxCB == 0) return -1;
    if (pxHandle == 0) return -1;
    pxHandle->iNextDataOffset = -1;
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 10) != 10) return -1;
    for (i = 0; i < 10; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    // Check version:
    if (ucBuf[4] > SG_LIB_VERSION_H) return -1;
    if (ucBuf[5] > SG_LIB_VERSION_L) return -1;
    iOffset += 10;

    // Read items:
    for (;;)
    {
        // Read type:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
        vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
        iOffset ++;
        if ((enSgDialogItemType)ucBuf[0] == SDIT_EMPTY) break;
        // Type validation:
        if (ucBuf[0] >= (unsigned char)SDIT_MAX_VALUE) return -1;
        xItem.eType = (enSgDialogItemType)ucBuf[0];
        if (pxHandle->iNextDataOffset < 0)  pxHandle->iNextDataOffset = iOffset - 1;
        // Item ID and size:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 8) != 8) return -1;
        for (i = 0; i < 8; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        xItem.uiID   = *((unsigned int*)&ucBuf[0]);
        xItem.uiSize = *((unsigned int*)&ucBuf[4]);
        iOffset += 8;
        // Skip item data:
        ui = 0;
        while (xItem.uiSize)
        {
            ui = xItem.uiSize;
            if (ui > DEFAULT_BUF_SIZE) ui = DEFAULT_BUF_SIZE;
            if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, (int)ui) != (int)ui) return -1;
            for (i = 0; i < (int)ui; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
            iOffset += ui;
            xItem.uiSize -= ui;
        };
    };

    // Read and parse CRC:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 2) != 2) return -1;
    if (ucBuf[0] != (unsigned char)(usCRC16 >> 0)) return -1;
    if (ucBuf[1] != (unsigned char)(usCRC16 >> 8)) return -1;

    return iItemsCount;
};
/***************************************************************************************************
* Function:    iSgDialogReadNextItem
* Description:
****************************************************************************************************/
int iSgDialogReadNextItem     (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogReadListHandle* pxHandle, strSgDialogItem* pxDstItem, unsigned char* pcItemDataBuf, unsigned int uiItemDataBufSize)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    int i;

    if (pxCB == 0) return -1;
    if (pxHandle == 0) return -1;
    if (pxDstItem == 0) return -1;

    // Read type:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, pxHandle->iNextDataOffset, 1) != 1) return -1;
    if ((enSgDialogItemType)ucBuf[0] == SDIT_EMPTY) return 0;
    // Type validation:
    if (ucBuf[0] >= (unsigned char)SDIT_MAX_VALUE) return -1;
    pxDstItem->eType = (enSgDialogItemType)ucBuf[0];
    pxHandle->iNextDataOffset ++;
    // Item ID and size:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, pxHandle->iNextDataOffset, 8) != 8) return -1;
    pxDstItem->uiID   = *((unsigned int*)&ucBuf[0]);
    pxDstItem->uiSize = *((unsigned int*)&ucBuf[4]);
    pxHandle->iNextDataOffset += 8;
    // Read and return item data;
    if (uiItemDataBufSize < pxDstItem->uiSize) return -1;
    if (pxCB->piReadDescriptor(pvCB_Arg, pcItemDataBuf, pxHandle->iNextDataOffset, (int)pxDstItem->uiSize) != (int)pxDstItem->uiSize) return -1;
    pxHandle->iNextDataOffset += (int)pxDstItem->uiSize;
    pxDstItem->pvItemData      = pcItemDataBuf;
    return 1;
};
/***************************************************************************************************
* Function:    iSgDialogReadItem
* Description:
****************************************************************************************************/
int iSgDialogReadItem       (const strSgDialogCB* pxCB, void* pvCB_Arg, unsigned int uiID, strSgDialogItem* pxDstItem, unsigned char* pcItemDataBuf, unsigned int uiItemDataBufSize)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    unsigned short usCRC16;
    int iOffset = 0;
    int i;
    unsigned int ui;
    int iItemsCount = 0;
    strSgDialogItem xItem;

    usCRC16 = 0xFFFF;

    if (pxCB == 0) return -1;
    if (uiItemDataBufSize == 0) return -1;
    if (pcItemDataBuf == 0) return -1;

    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 10) != 10) return -1;
    for (i = 0; i < 10; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    // Check version:
    if (ucBuf[4] > SG_LIB_VERSION_H) return -1;
    if (ucBuf[5] > SG_LIB_VERSION_L) return -1;
    iOffset += 10;

    // Read items:
    for (;;)
    {
        // Read type:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
        vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
        iOffset ++;
        if ((enSgDialogItemType)ucBuf[0] == SDIT_EMPTY) break;
        // Type validation:
        if (ucBuf[0] >= (unsigned char)SDIT_MAX_VALUE) return -1;
        xItem.eType = (enSgDialogItemType)ucBuf[0];
        // Item ID and size:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 8) != 8) return -1;
        for (i = 0; i < 8; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        xItem.uiID   = *((unsigned int*)&ucBuf[0]);
        xItem.uiSize = *((unsigned int*)&ucBuf[4]);
        iOffset += 8;

        // Item data:
        if (xItem.uiID == uiID)
        {
            // Read and return item data;
            if (uiItemDataBufSize < xItem.uiSize) return -1;
            if (pxCB->piReadDescriptor(pvCB_Arg, pcItemDataBuf, iOffset, (int)xItem.uiSize) != (int)xItem.uiSize) return -1;
            for (ui = 0; ui < xItem.uiSize; ui ++) vSgDialogUpdateCRC16 (&usCRC16, pcItemDataBuf[ui]);
            iOffset += xItem.uiSize;
            xItem.pvItemData      = pcItemDataBuf;
            pxDstItem->uiID       = xItem.uiID;
            pxDstItem->eType      = xItem.eType;
            pxDstItem->pvItemData = pcItemDataBuf;
            pxDstItem->uiSize     = xItem.uiSize;
            iItemsCount ++;
        }
        else
        {
            // Skip item data:
            ui = 0;
            while (xItem.uiSize)
            {
                ui = xItem.uiSize;
                if (ui > DEFAULT_BUF_SIZE) ui = DEFAULT_BUF_SIZE;
                if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, (int)ui) != (int)ui) return -1;
                for (i = 0; i < (int)ui; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
                iOffset += ui;
                xItem.uiSize -= ui;
            };
        };
    };

    // Read and parse CRC:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 2) != 2) return -1;
    if (ucBuf[0] != (unsigned char)(usCRC16 >> 0)) return -1;
    if (ucBuf[1] != (unsigned char)(usCRC16 >> 8)) return -1;

    return iItemsCount;
};
/***************************************************************************************************
* Function:    iSgDialogAddItem
* Description:
****************************************************************************************************/
int iSgDialogAddItem         (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogItem* pxSrcItem)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    unsigned short usCRC16;
    int iOffset = 0;
    int i, k, n;
    unsigned int ui;
    unsigned int uiSize;
    unsigned int uiMaxItemID = 0xFFFFFFFF;

    usCRC16 = 0xFFFF;

    if (pxCB == 0) return -1;
    if (pxSrcItem == 0) return -1;
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 10) != 10) return -1;
    for (i = 0; i < 10; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    // Check version:
    if (ucBuf[4] > SG_LIB_VERSION_H) return -1;
    if (ucBuf[5] > SG_LIB_VERSION_L) return -1;
    iOffset += 10;

    // Read items:
    for (;;)
    {
        // Read type:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
        vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
        iOffset ++;
        if ((enSgDialogItemType)ucBuf[0] == SDIT_EMPTY) break;
        // Type validation:
        if (ucBuf[0] >= (unsigned char)SDIT_MAX_VALUE) return -1;
        // Item ID and size:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 8) != 8) return -1;
        for (i = 0; i < 8; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        uiSize   = *((unsigned int*)&ucBuf[0]);
        if ((uiSize > uiMaxItemID) || (uiMaxItemID == 0xFFFFFFFF)) uiMaxItemID = uiSize; // Search for max ID
        uiSize = *((unsigned int*)&ucBuf[4]);
        iOffset += 8;
        // Skip item data:
        ui = 0;
        while (uiSize)
        {
            ui = uiSize;
            if (ui > DEFAULT_BUF_SIZE) ui = DEFAULT_BUF_SIZE;
            if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, (int)ui) != (int)ui) return -1;
            for (i = 0; i < (int)ui; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
            iOffset += ui;
            uiSize -= ui;
        };

    };

    // Read and parse CRC:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 2) != 2) return -1;
    if (ucBuf[0] != (unsigned char)(usCRC16 >> 0)) return -1;
    if (ucBuf[1] != (unsigned char)(usCRC16 >> 8)) return -1;



    // Copy new content till empty item and recalculate CRC16:
    k = 0; // Local offset
    iOffset --;
    usCRC16 = 0xFFFF;
    for (;;)
    {
        if (k >= iOffset) break;
        n = (iOffset - k);
        if (n > DEFAULT_BUF_SIZE) n = DEFAULT_BUF_SIZE;
        if (pxCB->piReadDescriptor (pvCB_Arg, ucBuf, k, n) != n) return -1;
        if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, k, n) != n) return -1;
        for (i = 0; i < n; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        k += n;
    };

    // Next ID:
    pxSrcItem->uiID = uiMaxItemID + 1;
    // Type, ID and size:
    ucBuf[0] = (unsigned char)pxSrcItem->eType;
    *((int*)&ucBuf[1]) = pxSrcItem->uiID;
    *((int*)&ucBuf[5]) = pxSrcItem->uiSize;
    if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 9) != 9) return -1;
    for (i = 0; i < 9; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    iOffset += 9;
    // Item data:
    if (pxSrcItem->uiSize)
    {
        if (pxSrcItem->pvItemData == 0) return -1;
        if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)pxSrcItem->pvItemData, iOffset, pxSrcItem->uiSize) != (int)pxSrcItem->uiSize) return -1;
        for (i = 0; i < (int)pxSrcItem->uiSize; i ++) vSgDialogUpdateCRC16 (&usCRC16, *(((unsigned char*)pxSrcItem->pvItemData) + i));
        iOffset += pxSrcItem->uiSize;
    };
    // Add end of items (empty item) tag:
    ucBuf[0] = (unsigned char)SDIT_EMPTY;
    if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
    vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
    iOffset ++;

    // Add CRC-16:
    if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)&usCRC16, iOffset, sizeof(usCRC16)) != sizeof(usCRC16)) return -1;

    return 1;
};

/***************************************************************************************************
* Function:    iSgDialogRemoveModifyItem
* Description:
****************************************************************************************************/
int iSgDialogRemoveModifyItem (const strSgDialogCB* pxCB, void* pvCB_Arg, unsigned int uiID, strSgDialogItem* pxSrcItem)
{
    unsigned char ucBuf[DEFAULT_BUF_SIZE];
    unsigned short usCRC16;
    int iOffset = 0, iFoundStartDataPointer = 0, iRemainSectorStartDataPointer = 0, iRemainSectorEndDataPointer = 0;
    int i, k, n;
    unsigned int ui;
    unsigned int uiSize;
    unsigned int uiMaxItemID = 0xFFFFFFFF;
    unsigned char ucFoundFlag = 0;

    usCRC16 = 0xFFFF;

    if (pxCB == 0) return -1;

    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 10) != 10) return -1;
    for (i = 0; i < 10; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    // Check version:
    if (ucBuf[4] > SG_LIB_VERSION_H) return -1;
    if (ucBuf[5] > SG_LIB_VERSION_L) return -1;
    iOffset += 10;

    // Read items:
    for (;;)
    {
        // Read type:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
        vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
        iOffset ++;
        if ((enSgDialogItemType)ucBuf[0] == SDIT_EMPTY)
        {
            if (ucFoundFlag == 2)
            {
                ucFoundFlag = 3;
                iRemainSectorEndDataPointer = iOffset;
            };
            break;
        };
        // Type validation:
        if (ucBuf[0] >= (unsigned char)SDIT_MAX_VALUE) return -1;
        // Item ID and size:
        if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 8) != 8) return -1;
        for (i = 0; i < 8; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        uiSize   = *((unsigned int*)&ucBuf[0]);
        if ((uiSize > uiMaxItemID) || (uiMaxItemID == 0xFFFFFFFF)) uiMaxItemID = uiSize; // Search for max ID
        if ((uiSize == uiID) && (ucFoundFlag == 0))
        {
            ucFoundFlag = 1;
            iFoundStartDataPointer = iOffset - 1;
        };
        uiSize = *((unsigned int*)&ucBuf[4]);
        iOffset += 8;
        // Skip item data:
        ui = 0;
        while (uiSize)
        {
            ui = uiSize;
            if (ui > DEFAULT_BUF_SIZE) ui = DEFAULT_BUF_SIZE;
            if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, (int)ui) != (int)ui) return -1;
            for (i = 0; i < (int)ui; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
            iOffset += ui;
            uiSize -= ui;
        };
        if (ucFoundFlag == 1)
        {
            ucFoundFlag = 2;
            iRemainSectorStartDataPointer = iOffset;
        };

    };

    // Read and parse CRC:
    if (pxCB->piReadDescriptor(pvCB_Arg, ucBuf, iOffset, 2) != 2) return -1;
    if (ucBuf[0] != (unsigned char)(usCRC16 >> 0)) return -1;
    if (ucBuf[1] != (unsigned char)(usCRC16 >> 8)) return -1;

    if (ucFoundFlag != 3) return 0;  // No items to remove/modify

    // Copy old content till found item and recalculate CRC16:
    k = 0; // Local offset
    iOffset = iFoundStartDataPointer;
    usCRC16 = 0xFFFF;
    for (;;)
    {
        if (k >= iOffset) break;
        n = (iOffset - k);
        if (n > DEFAULT_BUF_SIZE) n = DEFAULT_BUF_SIZE;
        if (pxCB->piReadDescriptor (pvCB_Arg, ucBuf, k, n) != n) return -1;
        if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, k, n) != n) return -1;
        for (i = 0; i < n; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        k += n;
    };


    // Modify:
    if (pxSrcItem)
    {
        // Type, ID and size:
        ucBuf[0] = (unsigned char)pxSrcItem->eType;
        *((int*)&ucBuf[1]) = uiID;
        *((int*)&ucBuf[5]) = pxSrcItem->uiSize;
        if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 9) != 9) return -1;
        for (i = 0; i < 9; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        iOffset += 9;
        // Item data:
        if (pxSrcItem->uiSize)
        {
            if (pxSrcItem->pvItemData == 0) return -1;
            if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)pxSrcItem->pvItemData, iOffset, pxSrcItem->uiSize) != (int)pxSrcItem->uiSize) return -1;
            for (i = 0; i < (int)pxSrcItem->uiSize; i ++) vSgDialogUpdateCRC16 (&usCRC16, *(((unsigned char*)pxSrcItem->pvItemData) + i));
            iOffset += pxSrcItem->uiSize;
        };
    }


    // Copy last part of content and update CRC:
    k = iRemainSectorStartDataPointer; // Local offset
    for (;;)
    {
        if (k >= iRemainSectorEndDataPointer) break;
        n = (iRemainSectorEndDataPointer - k);
        if (n > DEFAULT_BUF_SIZE) n = DEFAULT_BUF_SIZE;
        if (pxCB->piReadDescriptor (pvCB_Arg, ucBuf, k, n) != n) return -1;
        if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, n) != n) return -1;
        for (i = 0; i < n; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
        iOffset += n;
        k += n;
    };
    /*
    // Next ID:
    pxSrcItem->uiID = uiMaxItemID + 1;
    // Type, ID and size:
    ucBuf[0] = (unsigned char)pxSrcItem->eType;
    *((int*)&ucBuf[1]) = pxSrcItem->uiID;
    *((int*)&ucBuf[5]) = pxSrcItem->uiSize;
    if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 9) != 9) return -1;
    for (i = 0; i < 9; i ++) vSgDialogUpdateCRC16 (&usCRC16, ucBuf[i]);
    iOffset += 9;
    // Item data:
    if (pxSrcItem->uiSize)
    {
        if (pxSrcItem->pvItemData == 0) return -1;
        if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)pxSrcItem->pvItemData, iOffset, pxSrcItem->uiSize) != (int)pxSrcItem->uiSize) return -1;
        for (i = 0; i < (int)pxSrcItem->uiSize; i ++) vSgDialogUpdateCRC16 (&usCRC16, *(((unsigned char*)pxSrcItem->pvItemData) + i));
        iOffset += pxSrcItem->uiSize;
    };
    // Add end of items (empty item) tag:
    ucBuf[0] = (unsigned char)SDIT_EMPTY;
    if (pxCB->piWriteDescriptor(pvCB_Arg, ucBuf, iOffset, 1) != 1) return -1;
    vSgDialogUpdateCRC16 (&usCRC16, ucBuf[0]);
    iOffset ++;
    */

    // Add CRC-16:
    if (pxCB->piWriteDescriptor(pvCB_Arg, (unsigned char*)&usCRC16, iOffset, sizeof(usCRC16)) != sizeof(usCRC16)) return -1;

    return 1;
};
/***************************************************************************************************
* Function:    vSgDialogUpdateCRC16
* Description:
****************************************************************************************************/
void vSgDialogUpdateCRC16 (unsigned short *psCurrentValue, unsigned char ucByte)
{
    #define CRC_POLY  0xCC27
    unsigned char c, ucTemp;

    *(psCurrentValue) ^= (unsigned char)ucByte;
    for (c = 0; c < 8; c ++)
    {
        ucTemp = (unsigned char)( (* (psCurrentValue)) & 0x0001);
        * (psCurrentValue) >>= 1;
        if (ucTemp) * (psCurrentValue) ^= CRC_POLY;
    };
};

