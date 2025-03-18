#ifndef SGLIST_LIB_H
#define SGLIST_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
                          LIBRARY INCLUDES
*******************************************************************************/

/*******************************************************************************
						  LIBRARY DEFINES
*******************************************************************************/

typedef struct
{
	unsigned int  uiKey;
	void*         pvItem;
} str_SgListItem;

typedef struct
{
	str_SgListItem* pxItems;
	unsigned int    uiCapacity;
	unsigned int    uiCount;
} str_SgList;
/*******************************************************************************
                          LIBRARY FUNCTIONS PROTOTYPES
*******************************************************************************/
void vSgListInitHandle     (str_SgList* pxSgList, unsigned char* pcDataHeap, unsigned int uiDataHeapSize);
int  iSgListAddItem        (str_SgList* pxSgList, unsigned int* piDstNewKey, void*  pvSrcItem);
int  iSgListPutItem        (str_SgList* pxSgList, unsigned int uiKey, void*  pvSrcItem);
int  iSgListGetItem        (str_SgList* pxSgList, unsigned int uiKey, void** ppvDstItem);
int  iSgListRepeatGetItem  (str_SgList* pxSgList, unsigned int uiKey, void** ppvDstItem, int iPreviousResult);
int  iSgListGetFirstItem   (str_SgList* pxSgList, unsigned int* piDstFirstItemKey, void** ppvDstItem);
int  iSgListGetNextItem    (str_SgList* pxSgList, unsigned int* piCurrentItemKey,  void** ppvDstItem); 
int  iSgListGetNearestItem (str_SgList* pxSgList, unsigned int* piCurrentItemKey,  void** ppvDstItem); 
int  iSgListDeleteItem     (str_SgList* pxSgList, unsigned int uiKey);

//int  iSgListReadItem      (str_SgList* pxSgList, int iItemIndex, unsigned int* piKey, void** ppvDstItem); 
   

#ifdef __cplusplus
}
#endif
#endif  // SGLIST_LIB_H
