/*******************************************************************************
                                  INCLUDES
*******************************************************************************/
#include "SgListLib.h"
/*******************************************************************************
                                  DEFINES
*******************************************************************************/
#ifndef NULL
#define NULL 0
#endif
/*******************************************************************************
                           LOCAL FUNCTIONS DEFINES
*******************************************************************************/
int iBinarySearch (unsigned int uiKey, str_SgListItem* pxListArray, unsigned int uiArraySize);
/*******************************************************************************
                                  FUNCTIONS:
*******************************************************************************/
/***************************************************************************************************
* Function:    vSgListInitHandle
* Description: Init list handle
* Retval:      none
****************************************************************************************************/
void vSgListInitHandle (str_SgList* pxSgList, unsigned char* pcDataHeap, unsigned int uiDataHeapSize)
{
	pxSgList->uiCount = 0;
	pxSgList->uiCapacity = 0;
	if (pcDataHeap == NULL) return;
	pxSgList->pxItems = (str_SgListItem*)(pcDataHeap);
	pxSgList->uiCapacity = uiDataHeapSize / sizeof(str_SgListItem);
}
/***************************************************************************************************
* Function:    iSgListAddItem
* Description: Add new item to SgList with creating new Key (put in piDstNewKey in case of success
* Retval:      index of new item / (-1) in case of error
****************************************************************************************************/
int  iSgListAddItem        (str_SgList* pxSgList, unsigned int* piDstNewKey, void*  pvSrcItem)
{
	unsigned int uiNewKey = 0;
	if (pxSgList->uiCount >= pxSgList->uiCapacity) return -1;
	if (pxSgList->uiCount) {uiNewKey = pxSgList->pxItems[pxSgList->uiCount - 1].uiKey + 1; if (uiNewKey == 0) return -1;} 
	pxSgList->pxItems[pxSgList->uiCount].uiKey = uiNewKey;
	pxSgList->pxItems[pxSgList->uiCount].pvItem = pvSrcItem;
	*piDstNewKey = uiNewKey;
	pxSgList->uiCount ++;
	return (pxSgList->uiCount - 1);	
};
/***************************************************************************************************
* Function:    iSgListPutItem
* Description: Add/modify item to SgList
* Retval:      index of item / (-1) in case of error
****************************************************************************************************/
int  iSgListPutItem    (str_SgList* pxSgList, unsigned int uiKey, void* pvSrcItem)
{
	unsigned int n, m;
	int iIndex;
	if (pxSgList->uiCount >= pxSgList->uiCapacity) return -1;
	// Search item:
	iIndex = iBinarySearch(uiKey, pxSgList->pxItems, pxSgList->uiCount);
	// Check if item found:
	if (iIndex >= 0)
	{
		// Found: change value:
		pxSgList->pxItems[iIndex].pvItem = pvSrcItem;
		return iIndex;
	}
	// Insert value:
	n = pxSgList->uiCount;
	while (n)
	{
		if (uiKey > pxSgList->pxItems[n - 1].uiKey) break;
		n --;
	}
	for (m = pxSgList->uiCount; m > n; m --)
	{
		pxSgList->pxItems[m] = pxSgList->pxItems[m - 1];
	};
	pxSgList->pxItems[n].uiKey  = uiKey;
	pxSgList->pxItems[n].pvItem = pvSrcItem;

	pxSgList->uiCount ++;
	return n;

}
/***************************************************************************************************
* Function:    iSgListGetItem
* Description: Get item from SgList by Key
* Retval:      index of found item / (-1) in case of error
****************************************************************************************************/
int  iSgListGetItem    (str_SgList* pxSgList, unsigned int uiKey, void** ppvDstItem)
{
	int iIndex;
	if (pxSgList->uiCount == 0) return -1;
	iIndex = iBinarySearch(uiKey, pxSgList->pxItems, pxSgList->uiCount);
	if (iIndex >= 0)
	{
		// Found: return value:
		if (pxSgList->pxItems[iIndex].pvItem == NULL) return -1;
		if (ppvDstItem != NULL) *ppvDstItem = pxSgList->pxItems[iIndex].pvItem;
		return (int)(iIndex);
	};
    return -1;
};
/***************************************************************************************************
* Function:    iSgListRepeatGetItem
* Description: Get item from SgList by Key ing to use previous index
* Retval:      index of found item / (-1) in case of error
****************************************************************************************************/
int  iSgListRepeatGetItem    (str_SgList* pxSgList, unsigned int uiKey, void** ppvDstItem, int iPreviousResult)
{
	int iIndex;
	if (pxSgList->uiCount == 0) return -1;
	iIndex = iPreviousResult;
	if ((iIndex >= 0) && (iIndex < pxSgList->uiCount))
	{
		if (pxSgList->pxItems[iIndex].uiKey == uiKey)
		{
			if (pxSgList->pxItems[iIndex].pvItem != NULL)
			{
				*ppvDstItem = pxSgList->pxItems[iIndex].pvItem;
				return iIndex;
			};
		};
	};
	iIndex = iBinarySearch(uiKey, pxSgList->pxItems, pxSgList->uiCount);
	if (iIndex >= 0)
	{
		// Found: return value:
		if (pxSgList->pxItems[iIndex].pvItem == NULL) return -1;
		if (ppvDstItem != NULL) *ppvDstItem = pxSgList->pxItems[iIndex].pvItem;
		return (int)(iIndex);
	};
    return -1;
};
/***************************************************************************************************
* Function:    iSgListGetFirstItem
* Description: Get first item. piKey replaced by first item key in case of success
* Retval:      (0) index of first item / (-1) in case of error
****************************************************************************************************/
int  iSgListGetFirstItem   (str_SgList* pxSgList, unsigned int* piDstFirstItemKey, void** ppvDstItem)
{
	if (pxSgList->uiCount == 0) return -1;
	*ppvDstItem = pxSgList->pxItems[0].pvItem;
	*piDstFirstItemKey = pxSgList->pxItems[0].uiKey;
	return 0;
};
/***************************************************************************************************
* Function:    iSgListGetNextItem
* Description: Get next item depending on preveous Key. piCurrentItemKey replaced by next item key in 
*              case of success
* Retval:      index of found item / (-1) in case of error
****************************************************************************************************/
int  iSgListGetNextItem   (str_SgList* pxSgList, unsigned int* piCurrentItemKey, void** ppvDstItem)
{
	int iIndex;
	unsigned int uiKey;
	unsigned int uiL, uiR, uiMid;
	if (pxSgList->uiCount == 0) return -1;
	uiKey = *piCurrentItemKey + 1;

	uiR = pxSgList->uiCount - 1;
	uiL = 0; // Also keep index of found or nearest (higher) item

	while (uiL <= uiR)
	{
		uiMid = (uiL + uiR) >> 1; // 2
		if (uiKey == pxSgList->pxItems[uiMid].uiKey) {uiL = uiMid; break;};
		if (uiKey < pxSgList->pxItems[uiMid].uiKey)
		{
			if (uiMid == 0) return -1;
			uiR = uiMid - 1;
		}
		else uiL = uiMid + 1;
	};
	if (uiL >= pxSgList->uiCount) return -1;
	*piCurrentItemKey = pxSgList->pxItems[uiL].uiKey;
	*ppvDstItem = pxSgList->pxItems[uiL].pvItem;
	return uiL;	
};
/***************************************************************************************************
* Function:    iSgListGetNearestItem
* Description: Get item by key or nearest to it. piCurrentItemKey replaced by nearest item key in 
*              case of success
* Retval:      index of found item / (-1) in case of error
****************************************************************************************************/
int  iSgListGetNearestItem   (str_SgList* pxSgList, unsigned int* piCurrentItemKey, void** ppvDstItem)
{
	int iIndex;
	unsigned int uiKey;
	unsigned int uiL, uiR, uiMid;
	if (pxSgList->uiCount == 0) return -1;
	
	if (*piCurrentItemKey == 0) // Return first item
	{
		*ppvDstItem = pxSgList->pxItems[0].pvItem;
		*piCurrentItemKey = pxSgList->pxItems[0].uiKey;
		return 0;
	};
	
	uiKey = *piCurrentItemKey;

	uiR = pxSgList->uiCount - 1;
	uiL = 0; // Also keep index of found or nearest (higher) item

	while (uiL <= uiR)
	{
		uiMid = (uiL + uiR) >> 1; // 2
		if (uiKey == pxSgList->pxItems[uiMid].uiKey) {uiL = uiMid; break;};
		if (uiKey < pxSgList->pxItems[uiMid].uiKey)
		{
			if (uiMid == 0) return -1;
			uiR = uiMid - 1;
		}
		else uiL = uiMid + 1;
	};
	if (uiL >= pxSgList->uiCount) return -1;
	*piCurrentItemKey = pxSgList->pxItems[uiL].uiKey;
	*ppvDstItem = pxSgList->pxItems[uiL].pvItem;
	return uiL;	
};
/***************************************************************************************************
* Function:    iSgListDeleteItem
* Description: Remove item from SgList by Key
* Retval:      index of founed and removed item / (-1) in case of error
****************************************************************************************************/
int  iSgListDeleteItem (str_SgList* pxSgList, unsigned int uiKey)
{
	unsigned int n;
	int iIndex;
	if (pxSgList->uiCount == 0) return -1;
	// Search item:
	iIndex = iBinarySearch(uiKey, pxSgList->pxItems, pxSgList->uiCount);
	// Check if item found:
	if (iIndex < 0) return -1;
	// Delete value:
	for (n = iIndex; n < pxSgList->uiCount - 1; n ++)
	{
		pxSgList->pxItems[n] = pxSgList->pxItems[n + 1];
	};
	pxSgList->uiCount --;
	return (int)(iIndex);
}
/***************************************************************************************************
* Function:    iBinarySearch
* Description:
* Retval:      Index of found item or (-1) if not found
****************************************************************************************************/
int iBinarySearch (unsigned int uiKey, str_SgListItem* pxListArray, unsigned int uiArraySize)
{
	unsigned int uiL, uiR, uiMid;
	if (uiArraySize == 0) return -1;

	uiR = uiArraySize - 1;
	uiL = 0;

	while (uiL <= uiR)
	{
		uiMid = (uiL + uiR) >> 1; // 2
		if (uiKey == pxListArray[uiMid].uiKey) return uiMid;
		if (uiKey < pxListArray[uiMid].uiKey)
		{
			if (uiMid == 0) return -1;
			uiR = uiMid - 1;
		}
		else uiL = uiMid + 1;
	}
	return -1;
}


