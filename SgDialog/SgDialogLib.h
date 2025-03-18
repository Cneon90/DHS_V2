//---------------------------------------------------------------------------

#ifndef SgDialogLibH
#define SgDialogLibH
//---------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
											DEFINES
***************************************************************************************************/
#define SG_LIB_VERSION_H   1
#define SG_LIB_VERSION_L   1

#define SG_DIALOG_HEAP_SIZE  1024
#define DIALOG_MAX_ANSWERS_COUNT	8
#define VIRTUAL_FILE_SIZE   		0x100000
#define ITEM_DATA_BUF_SIZE  		0x10000

typedef enum
{
    SDIT_ANY       = 0x0,
    SDIT_QUESTION  = 0x1,
    SDIT_ANSWER    = 0x2,
    SDIT_SOLUTION  = 0x3,
    SDIT_DIALOG    = 0x4,
    SDIT_MAX_VALUE = 0x5,
    SDIT_EMPTY     = 0xFF
} enSgDialogItemType;

typedef struct
{
	unsigned int 		uiID;
	enSgDialogItemType 	eType;
	unsigned int 		uiSize;
	void*        		pvItemData;
} strSgDialogItem;

typedef struct
{
	int iQuestion;
	unsigned char ucAnswersCount;
	int iAnswer[DIALOG_MAX_ANSWERS_COUNT];
	int iNextStep[DIALOG_MAX_ANSWERS_COUNT];
} str_xDialogItemDataStruct;

typedef struct
{
    int            (*piReadDescriptor)                 (void* pvCB_Arg,       unsigned char* pcDstData, int iOffset, int iSize);
    int            (*piWriteDescriptor)                (void* pvCB_Arg, const unsigned char* pcSrcData, int iOffset, int iSize);
} strSgDialogCB;

typedef struct
{
    int           iNextDataOffset;
} strSgDialogReadListHandle;



/***************************************************************************************************
										LIBRARY FUNCTIONS PROTOTYPES
***************************************************************************************************/
// Return value - count of added/readed/modified/removed item(s). -1 - in case of error
int iSgDialogCreateNewDialog  (const strSgDialogCB* pxCB, void* pvCB_Arg);
int iSgDialogReadItemsInit    (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogReadListHandle* pxHandle);
int iSgDialogReadNextItem     (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogReadListHandle* pxHandle, strSgDialogItem* pxDstItem, unsigned char* pcItemDataBuf, unsigned int uiItemDataBufSize);
int iSgDialogReadItem         (const strSgDialogCB* pxCB, void* pvCB_Arg, unsigned int uiID, strSgDialogItem* pxDstItem, unsigned char* pcItemDataBuf, unsigned int uiItemDataBufSize);
int iSgDialogAddItem          (const strSgDialogCB* pxCB, void* pvCB_Arg, strSgDialogItem* pxSrcItem);
int iSgDialogRemoveModifyItem (const strSgDialogCB* pxCB, void* pvCB_Arg, unsigned int uiID, strSgDialogItem* pxSrcItem);

#ifdef __cplusplus
}
#endif
#endif  // SGDIALOGLIB_H

