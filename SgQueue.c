/***************************************************************************************************
                                        INCLUDES
***************************************************************************************************/
#include "SgQueue.h"
/***************************************************************************************************
                                        DEFINES
***************************************************************************************************/

/***************************************************************************************************
                            	LOCAL FUNCTIONS PROTOTYPES
***************************************************************************************************/
/***************************************************************************************************
* Function:    iSgQueueInitQueue
* Description:
****************************************************************************************************/
int iSgQueueInitQueue (str_SgQueueHandle* pxHandle, unsigned int uiItemSize, unsigned char* pcDataHeap, unsigned int uiDataHeapSize)
{
    if (pxHandle == 0)   return -1;
    if (pcDataHeap == 0) return -1;
    if (uiItemSize == 0) return -1;

    pxHandle->pcDataHeap      = pcDataHeap;
    pxHandle->uiItemSize      = uiItemSize;
    pxHandle->uiItemsMaxCount = uiDataHeapSize / uiItemSize;
    pxHandle->uiFrontPosition = 0;
    pxHandle->uiEndPosition   = 0;
    pxHandle->uiItemsCount    = 0;
    pxHandle->bIsExcluding    = 0;

    return (int)pxHandle->uiItemsMaxCount;
};
/***************************************************************************************************
* Function:    iSgQueueInitExludingQueue
* Description:
****************************************************************************************************/
int iSgQueueInitExludingQueue (str_SgQueueHandle* pxHandle, unsigned int uiItemSize, unsigned char* pcDataHeap, unsigned int uiDataHeapSize)
{
    if (pxHandle == 0)   return -1;
    if (pcDataHeap == 0) return -1;
    if (uiItemSize == 0) return -1;

    pxHandle->pcDataHeap      = pcDataHeap;
    pxHandle->uiItemSize      = uiItemSize;
    pxHandle->uiItemsMaxCount = uiDataHeapSize / uiItemSize;
    pxHandle->uiFrontPosition = 0;
    pxHandle->uiEndPosition   = 0;
    pxHandle->uiItemsCount    = 0;
    pxHandle->bIsExcluding    = 1;

    return (int)pxHandle->uiItemsMaxCount;
};
/***************************************************************************************************
* Function:    uiSgQueueGetItemsCount
* Description:
****************************************************************************************************/
unsigned int uiSgQueueGetItemsCount (str_SgQueueHandle* pxHandle)
{
    if (pxHandle == 0) return 0;
    return (pxHandle->uiItemsCount);
};
/***************************************************************************************************
* Function:    eSgQueuePutToFront
* Description:
****************************************************************************************************/
en_SgQueueResult eSgQueuePutToFront    (str_SgQueueHandle* pxHandle, void* pvSrcItem)
{
    unsigned int ui, uiOffset;

    if (pxHandle == 0)             return SGQR_QUEUE_ERROR;
    if (pxHandle->pcDataHeap == 0) return SGQR_QUEUE_ERROR;
    if (pxHandle->uiItemsCount >= pxHandle->uiItemsMaxCount) return SGQR_QUEUE_IS_FULL;
    if (pvSrcItem == 0)             return SGQR_QUEUE_ERROR;

    if (pxHandle->bIsExcluding)
    {
        uiOffset = pxHandle->uiEndPosition;
        for (;;)
        {
            if (uiOffset == pxHandle->uiFrontPosition) break;
            for (ui = 0; ui < pxHandle->uiItemSize; ui ++) if (*((unsigned char*)(pvSrcItem) + ui) != pxHandle->pcDataHeap[uiOffset * pxHandle->uiItemSize + ui]) break;
            if (ui >= pxHandle->uiItemSize) return SGQR_ITEM_ALREADY_EXISTS;
            uiOffset ++;
            if (uiOffset >= pxHandle->uiItemsMaxCount) uiOffset = 0;
        };
    };

    uiOffset = pxHandle->uiFrontPosition;
    uiOffset = uiOffset * pxHandle->uiItemSize;

    for (ui = 0; ui < pxHandle->uiItemSize; ui ++) pxHandle->pcDataHeap[uiOffset + ui] = *(((unsigned char*)(pvSrcItem)) + ui);

    pxHandle->uiFrontPosition ++;
    if (pxHandle->uiFrontPosition >= pxHandle->uiItemsMaxCount) pxHandle->uiFrontPosition = 0;
    pxHandle->uiItemsCount ++;
    
    return SGQR_SUCCESS;
};
/***************************************************************************************************
* Function:    eSgQueueReceiveFromEnd
* Description:
****************************************************************************************************/
en_SgQueueResult eSgQueueReceiveFromEnd (str_SgQueueHandle* pxHandle, void* pvDstItem)
{
    unsigned int ui, uiOffset;

    if (pxHandle == 0)               return SGQR_QUEUE_ERROR;
    if (pxHandle->pcDataHeap == 0)   return SGQR_QUEUE_ERROR;
    if (pxHandle->uiItemsCount == 0) return SGQR_QUEUE_IS_EMPTY;
    if (pvDstItem == 0)              return SGQR_QUEUE_ERROR;

    uiOffset = pxHandle->uiEndPosition;
    uiOffset = uiOffset * pxHandle->uiItemSize;

    for (ui = 0; ui < pxHandle->uiItemSize; ui ++) *(((unsigned char*)(pvDstItem)) + ui) = pxHandle->pcDataHeap[uiOffset + ui];

    pxHandle->uiEndPosition ++;
    if (pxHandle->uiEndPosition >= pxHandle->uiItemsMaxCount) pxHandle->uiEndPosition = 0;
    pxHandle->uiItemsCount --;
    
    return SGQR_SUCCESS;
};
