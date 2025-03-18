#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H

/***************************************************************************************************
                                        DEFINES
***************************************************************************************************/ 
#ifndef SETB
#define SETB(x)                        |= (1<<x)  // Set bit
#endif
#ifndef CLRB
#define CLRB(x)                        &=~(1<<x)  // Reset bit
#endif 
#ifndef INVB
#define INVB(x)                        ^=(1<<x)   // Inv bit 
#endif
#ifndef LOBYTE
#define LOBYTE(x)                      ((unsigned char)(x & 0x00FF))
#endif
#ifndef HIBYTE
#define HIBYTE(x)                      ((unsigned char)((x & 0xFF00) >> 8))
#endif
#ifndef NULL
#define NULL 0
#endif


/***************************************************************************************************
                                LIBRARY FUNCTIONS PROTOTYPES
***************************************************************************************************/ 
unsigned short usHF_GetShortFromBytesArray         (unsigned char* pcSrcArray);
unsigned int   uiHF_GetIntFromBytesArray           (unsigned char* pcSrcArray);
void            vHF_PutShortToBytesArray           (unsigned short usSrcValue, unsigned char* pcDstArray);
void            vHF_PutIntToBytesArray             (unsigned int uiSrcValue, unsigned char* pcDstArray);
void            vHF_CalcCRC16                      (unsigned char ucNewByte, unsigned short *psDstCurrentValue, unsigned short usPoly);
char            cHF_ConvertToLowRegister           (char cSymCode);
unsigned int   uiHF_GetTextLenght                  (const char* pcSrcText);
unsigned char   bHF_FindText                       (const char *pcRefText,  char *pcInputString, unsigned short usMaxPosition, unsigned int *piDstFoundPosition);
unsigned int   uiHF_AddStringToArray               (const char* pcSrcString, unsigned int uiSrcStringSize, unsigned char* pcDstArray, unsigned int uiDstArraySize);
unsigned int   uiHF_ConvertCardNumber              (unsigned int uiRealCardNumber);
void            vHF_UnixToDateTime                 (unsigned int uiUnixTime, unsigned char* pcDateTimeDstArray);
unsigned int   uiHF_DateTimeToUnix                 (unsigned char* pcDateTimeArray);
unsigned char   bHF_DecodeWigandData               (unsigned char* pcSrcData, unsigned short usSrcDataBitsCount, unsigned int* piDstCardCode);
unsigned char  ucHF_GetMajorIntValueFromBytesArray (unsigned char* pcSrcArray, unsigned int* piResult);
void            vHF_PutIntToMajorBytesArray        (unsigned int uiSrc, unsigned char* pcDst);
unsigned char  ucHF_GetMaskState                   (unsigned char *ucMask, unsigned char ucBitNumber);
void            vHF_SetMaskBit                     (unsigned char *ucMask, unsigned char ucBitNumber, unsigned short usMaxBitAdress);
unsigned char  ucHF_AddValueToArray                (unsigned int  uiValue, unsigned char *pcArray, unsigned int  uiBitIndex, unsigned char ucBitsCount);
unsigned char  ucHF_ExtractValueFromArray          (unsigned int *piValue, unsigned char *pcArray, unsigned int  uiBitIndex, unsigned char ucBitsCount);
unsigned char  ucHF_GetModulOfChar                 (unsigned char ucSrc);
unsigned char  ucHF_GetHorizontalVectors           (unsigned char *pcVerticalSrcVector, unsigned char* ucDstH1Vector, unsigned char* ucDstH2Vector);

#endif
