#ifndef SGQUEUE_H
#define SGQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
                                            INCLUDES
***************************************************************************************************/
#include <stdint.h>
#include <stddef.h>
/***************************************************************************************************
                                            DEFINES
***************************************************************************************************/
typedef struct
{
    unsigned char* pcDataHeap;
    unsigned int   uiItemsMaxCount;
    unsigned int   uiItemSize;
    unsigned int   uiFrontPosition;
    unsigned int   uiEndPosition;
    unsigned int   uiItemsCount;
    unsigned char  bIsExcluding;
} str_SgQueueHandle;

typedef enum
{
    SGQR_SUCCESS             = 0,
    SGQR_QUEUE_IS_FULL       = 1,
    SGQR_ITEM_ALREADY_EXISTS = 2,
    SGQR_QUEUE_IS_EMPTY      = 3,
    SGQR_QUEUE_ERROR         = 0xFF
} en_SgQueueResult;

/***************************************************************************************************
                                    LIBRARY FUNCTIONS PROTOTYPES
***************************************************************************************************/
int              iSgQueueInitQueue         (str_SgQueueHandle* pxHandle, unsigned int uiItemSize, unsigned char* pcDataHeap, unsigned int uiDataHeapSize);
int              iSgQueueInitExludingQueue (str_SgQueueHandle* pxHandle, unsigned int uiItemSize, unsigned char* pcDataHeap, unsigned int uiDataHeapSize);
unsigned int    uiSgQueueGetItemsCount     (str_SgQueueHandle* pxHandle);
en_SgQueueResult eSgQueuePutToFront        (str_SgQueueHandle* pxHandle, void* pvSrcItem);
en_SgQueueResult eSgQueueReceiveFromEnd    (str_SgQueueHandle* pxHandle, void* pvDstItem);
#ifdef __cplusplus
}
#endif
#endif  // SGQUEUE_H
