#include "help_functions.h"
#include <math.h>
static const unsigned short lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
static const unsigned short mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}; 

/***************************************************************************************************
* Function:    usHF_GetShortFromBytesArray
* Description: 
****************************************************************************************************/
unsigned short usHF_GetShortFromBytesArray(unsigned char* pcSrcArray)
{
	unsigned short usResult;
	usResult  = pcSrcArray[0];
	usResult  = usResult << 8;
	usResult += pcSrcArray[1];
	return usResult;
};
/***************************************************************************************************
* Function:    uiHF_GetIntFromBytesArray
* Description: 
****************************************************************************************************/
unsigned int uiHF_GetIntFromBytesArray (unsigned char* pcSrcArray)
{
	unsigned int uiResult;
	uiResult  = pcSrcArray[0];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[1];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[2];
	uiResult  = uiResult << 8;
	uiResult += pcSrcArray[3];
	return uiResult;
};
/***************************************************************************************************
* Function:    vHF_PutShortToBytesArray
* Description: 
****************************************************************************************************/
void vHF_PutShortToBytesArray(unsigned short usSrcValue, unsigned char* pcDstArray)
{
	pcDstArray[0] = (unsigned char)(usSrcValue >> 8);
	pcDstArray[1] = (unsigned char)(usSrcValue >> 0);
};
/***************************************************************************************************
* Function:    vHF_PutIntToBytesArray
* Description: 
****************************************************************************************************/
void vHF_PutIntToBytesArray(unsigned int uiSrcValue, unsigned char* pcDstArray)
{
	pcDstArray[0] = (unsigned char)(uiSrcValue >> 24);
	pcDstArray[1] = (unsigned char)(uiSrcValue >> 16);
	pcDstArray[2] = (unsigned char)(uiSrcValue >> 8);
	pcDstArray[3] = (unsigned char)(uiSrcValue >> 0);
};
/***************************************************************************************************
* Function:    vHF_CalcCRC16
* Description: 
****************************************************************************************************/
void vHF_CalcCRC16 (unsigned char ucNewByte, unsigned short *psDstCurrentValue, unsigned short usPoly)
{
	unsigned char c, ucTemp;
	* (psDstCurrentValue) ^= (unsigned char) ucNewByte;
	for(c = 0; c < 8; c ++)
	{
		ucTemp = (unsigned char)( (* (psDstCurrentValue)) & 0x0001);
		* (psDstCurrentValue) >>= 1;
		if(ucTemp) * (psDstCurrentValue) ^= usPoly;
	};
};
/***************************************************************************************************
* Function name: cHF_ConvertToLowRegister 
* Description:   Return only low register symbols if SymCode is high register symbol 
****************************************************************************************************/
char            cHF_ConvertToLowRegister           (char cSymCode)
{
	unsigned char c;
	c = (unsigned char)cSymCode;
	if (((c >= 65) && (c <= 90)) || ((c >= 192) && (c <= 223))) c += 32;
	return (char)c;	
};
/***************************************************************************************************
* Function name: uiHF_GetTextLenght 
* Description:   Calculate lenght of string (count of symbols exept last '\0') 
****************************************************************************************************/
unsigned int uiHF_GetTextLenght(const char* pcSrcText)
{
	unsigned int i = 0;
	if (pcSrcText == 0) return 0;
	while (pcSrcText[i ++]){};
	if (i) i --;
	return i;
};
/***************************************************************************************************
* Function name: bHF_FindText 
* Description:   Search text string in array
****************************************************************************************************/
unsigned char bHF_FindText(const char *pcRefText,  char *pcInputString, unsigned short usMaxPosition, unsigned int *piDstFoundPosition)
{
	unsigned int  i, k;
	unsigned char h;     
	for (i = 0; i < (unsigned int)(usMaxPosition); i ++)
	{
		h = 1;
		k = 0;
		while (pcRefText[k])
		{
			if ((i + k) >= (unsigned int)(usMaxPosition)) return 0;
			if (pcRefText[k] != pcInputString[i + k]) h = 0;
			if (pcInputString[i + k] == 0) {h = 0; break;};
			k ++;
		};
		if (h)
		{
			*(piDstFoundPosition) = i;
			return 1;
		};
	};
	return 0;
};
/***************************************************************************************************
* Function name: uiHF_AddStringToArray 
* Description:   
****************************************************************************************************/
unsigned int   uiHF_AddStringToArray (const char* pcSrcString, unsigned int uiSrcStringSize, unsigned char* pcDstArray, unsigned int uiDstArraySize)
{
	unsigned int ui;
	if (uiDstArraySize == 0) return 0;
	ui = 0;
	while (pcSrcString[ui])
	{
		if (ui >= uiDstArraySize) break;
		if ((uiSrcStringSize) && (ui >= uiSrcStringSize)) break;
		pcDstArray[ui] = pcSrcString[ui];
		ui ++;
	};
	while (ui < uiDstArraySize) pcDstArray[ui ++] = 0;
	return ui;	
};
/***************************************************************************************************
* Function:    uiHF_ConvertCardNumber
* Description: Convert card code ot tehnovizor coded format
****************************************************************************************************/
unsigned int uiHF_ConvertCardNumber(unsigned int uiRealCardNumber)
{
	unsigned int  uiResult = 0;
	unsigned char c;
	if (uiRealCardNumber == 0xFFFFFFFF) return 0xFFFFFFFF;
	for (c = 0; c < 16; c ++)
	{
		if (uiRealCardNumber & (1 << c)) uiResult SETB ((15 - c));
	};
	for (c = 0; c < 8; c ++)
	{
		if (uiRealCardNumber & (1 << (16 + c))) uiResult SETB ((23 - c));
		if (uiRealCardNumber & (1 << (24 + c))) uiResult SETB ((31 - c));
	};
	
	return uiResult;
};
/***************************************************************************************************
* Function:    vHF_UnixToDateTime
* Description: Convert UNIX-time ti date/time: [0] - year ... [5] - second
****************************************************************************************************/
void vHF_UnixToDateTime (unsigned int uiUnixTime, unsigned char* pcDateTimeDstArray)
{
	#define MONTAB(year)                        ((((year) & 03) || ((year) == 0)) ? mos : lmos)
	#define	DAYS_TO_32(year, mon)               (((year - 1) / 4) + MONTAB(year)[mon])
	
	unsigned int uiDays;
	unsigned int uiSecs;
	unsigned int uiYear;
	unsigned int i;
	const unsigned short* pMonth;
	
	// Total days:
	uiDays =  25567 + (uiUnixTime / 86400);
	
	// Determine hours, mins, secs:
	uiSecs = uiUnixTime % 86400;
	pcDateTimeDstArray[3] = uiSecs / 3600;
	uiSecs = uiSecs % 3600;
	pcDateTimeDstArray[4] = uiSecs / 60;
	pcDateTimeDstArray[5] = uiSecs % 60;
	
	// Determine year: 
	for (uiYear = (uiDays / 365); uiDays < (i = DAYS_TO_32(uiYear, 0) + (365 * uiYear)); ) { -- uiYear;};
	uiDays -= i;
	pcDateTimeDstArray[0] = uiYear - 100;
	
	// Determine month and date:
	pMonth = MONTAB(uiYear);
	for (uiYear = 12; uiDays < pMonth[-- uiYear]; );
	pcDateTimeDstArray[1] = uiYear + 1;  
	pcDateTimeDstArray[2] = uiDays - pMonth[uiYear] + 1;      
};
/***************************************************************************************************
* Function:    uiHF_DateTimeToUnix
* Description: [0] - year ... [5] - second
****************************************************************************************************/
unsigned int uiHF_DateTimeToUnix (unsigned char* pcDateTimeArray)
{
	unsigned int   uiSecs;
	unsigned int   uiMonth;
	unsigned int   uiYear;
	unsigned int   uiDays;

	// Calculate number of days:
	if (pcDateTimeArray[1]) {uiMonth = pcDateTimeArray[1] - 1;} else uiMonth = 0; 
	uiYear = 100 + pcDateTimeArray[0];
	uiDays = DAYS_TO_32(uiYear, uiMonth) - 1;
	uiDays += 365 * uiYear;
	uiDays += pcDateTimeArray[2]; 
	uiDays -= 25567; // (70 * 365) + 17;

	// Calculate number of seconds:
	uiSecs  = pcDateTimeArray[3]; 
	uiSecs  = uiSecs * 3600;
	uiSecs += 60 * pcDateTimeArray[4];
	uiSecs += pcDateTimeArray[5];
	uiSecs += uiDays * 86400;
	
	return (uiSecs);
};
/***************************************************************************************************
* Function name: bHF_DecodeWigandData 
* Description:   Decode bits sequence of wiegand protocol
****************************************************************************************************/
unsigned char bHF_DecodeWigandData (unsigned char* pcSrcData, unsigned short usSrcDataBitsCount, unsigned int* piDstCardCode)
{
	unsigned int uiBuf;
	unsigned int i;
	unsigned char ucParity;

	if (usSrcDataBitsCount == 25)
	{
		i = 3;
		for (;;)
		{
			pcSrcData[i] = pcSrcData[i] << 1;
			pcSrcData[i] CLRB(0);
			if (i == 0) break;
			if (pcSrcData[i - 1] & 0x80) pcSrcData[i] SETB(0);
			i --;
		};
		usSrcDataBitsCount ++;
	}
	else
	{
		if ((usSrcDataBitsCount != 26) && (usSrcDataBitsCount != 50)) return 0;
		ucParity = 0;
		for (i = 0; i < ((usSrcDataBitsCount / 2) - 1); i ++)  if (pcSrcData[(i + 1) / 8] & (1 << ((1 + i) % 8))) ucParity ++;
		if (ucParity & 0x01)
		{
			if (!(pcSrcData[0] & (1 << 0))) return 0;
		}
		else
		{
			if (pcSrcData[0]  & (1 << 0)) return 0;
		};
	};
	
	ucParity = 0;
	for (i = 0; i < ((usSrcDataBitsCount / 2) - 1); i ++)  if (pcSrcData[(i + (usSrcDataBitsCount / 2)) / 8] & (1 << ((i + (usSrcDataBitsCount / 2)) % 8))) ucParity ++;        
	if (ucParity & 0x01) 
	{
		if (pcSrcData[(usSrcDataBitsCount - 1) / 8] & (1 << ((usSrcDataBitsCount - 1) % 8))) return 0;
	}
	else
	{              
		if (!(pcSrcData[(usSrcDataBitsCount - 1) / 8] & (1 << ((usSrcDataBitsCount - 1) % 8)))) return 0;
	};

	
	uiBuf = 0;
	for (i = 0; i < (usSrcDataBitsCount - 2); i ++) 
	{
		if (i >= 32) break;
		if (pcSrcData[(usSrcDataBitsCount - 2 - i) / 8] & (1 << ((usSrcDataBitsCount - 2 - i) % 8))) uiBuf SETB (i);
	};
	*(piDstCardCode) = uiBuf;
		
	return 1;
};
/***************************************************************************************************
* Function name: ucHF_GetMajorVoteIntValueFormBytesArray 
* Description:   Majoritary selection int value from array "3" of "4"
* Returns:       0 - success, 1 - error 
****************************************************************************************************/
unsigned char ucHF_GetMajorIntValueFromBytesArray (unsigned char* pcSrcArray, unsigned int* piResult)
{
	unsigned int uiValue[4];        
	uiValue[0] = uiHF_GetIntFromBytesArray(&pcSrcArray[0]);
	uiValue[1] = 0xFFFFFFFF - uiHF_GetIntFromBytesArray(&pcSrcArray[4]);
	uiValue[2] = uiHF_GetIntFromBytesArray(&pcSrcArray[8]);
	uiValue[3] = 0xFFFFFFFF - uiHF_GetIntFromBytesArray(&pcSrcArray[12]);      
	if (uiValue[0] != uiValue[1])
	{
		if ((uiValue[1] == uiValue[3]) && (uiValue[1] == uiValue[2])) {*piResult = uiValue[1]; return 0;};
		if ((uiValue[0] == uiValue[2]) && (uiValue[0] == uiValue[3])) {*piResult = uiValue[0]; return 0;};
	}
	else
	{
		if (uiValue[2] == uiValue[1]){*piResult = uiValue[0]; return 0;};
		if (uiValue[3] == uiValue[1]){*piResult = uiValue[0]; return 0;};
	};
	return 1;    
};
/***************************************************************************************************
* Function name: vPutIntToMajoritaryBytesArray 
* Description:   
****************************************************************************************************/
void vHF_PutIntToMajorBytesArray (unsigned int uiSrc, unsigned char* pcDst)
{
	vHF_PutIntToBytesArray(uiSrc, &pcDst[0]);
	vHF_PutIntToBytesArray((0xFFFFFFFF - uiSrc), &pcDst[4]);
	vHF_PutIntToBytesArray(uiSrc, &pcDst[8]);
	vHF_PutIntToBytesArray((0xFFFFFFFF - uiSrc), &pcDst[12]); 
};
/***************************************************************************************************
* Function name: ucGetMaskState 
* Description:   Get bit value in mask
****************************************************************************************************/
unsigned char ucHF_GetMaskState (unsigned char *ucMask, unsigned char ucBitNumber)
{
	if ((*(ucMask + ((ucBitNumber - ucBitNumber % 8) / 8))) & (1 << (7 - ucBitNumber % 8))) return 1;
	return 0;
};
/***************************************************************************************************
* Function name: vSetMaskBit 
* Description:   Set bit in mask
****************************************************************************************************/
void vHF_SetMaskBit(unsigned char *ucMask, unsigned char ucBitNumber, unsigned short usMaxBitAdress)
{
	if (ucBitNumber > usMaxBitAdress) return;
	*(ucMask + ((ucBitNumber - ucBitNumber % 8) / 8)) |=  (1 << (7 - ucBitNumber % 8));  	
};
/***************************************************************************************************
* Function name: ucHF_AddValueToArray 
* Description:   Adds x-bits-len value to bytes array 
****************************************************************************************************/
unsigned char ucHF_AddValueToArray (unsigned int  uiValue, unsigned char *pcArray, unsigned int  uiBitIndex, unsigned char ucBitsCount)
{
	unsigned char  i;
	unsigned short usOffsetIndex;
	unsigned char  ucBitIndex;

	// Смещение индекса массива:
	usOffsetIndex = (uiBitIndex) >> 3;

	// Расчет индекса бита внутри байта:
	ucBitIndex = (unsigned char) (uiBitIndex & 0x00000007);

	// Основной цикл заталкивания:
	for (i = 0; i < ucBitsCount; i ++)
	{
		if (uiValue & (1 << ((ucBitsCount - 1) - i)))
		{
			*(pcArray + usOffsetIndex) |=  (unsigned char)(1 << (7 - ucBitIndex));
		}
		else
		{
			*(pcArray + usOffsetIndex) &= ~(unsigned char)(1 << (7 - ucBitIndex));
		};
		ucBitIndex ++;
		if (ucBitIndex > 7)
		{
			ucBitIndex = 0;
			usOffsetIndex ++;
		};
	};
	return i;
};
/***************************************************************************************************
* Function name: ucHF_ExtractValueFromArray 
* Description:   Extracts x-bits-len value from bytes array 
****************************************************************************************************/
unsigned char ucHF_ExtractValueFromArray(unsigned int *piValue, unsigned char *pcArray, unsigned int  uiBitIndex, unsigned char ucBitsCount)
{
	unsigned char  i;
	unsigned short usOffsetIndex;
	unsigned char  ucBitIndex;

	// Сбрасываем переменную:
	*(piValue) = 0x00000000;

	// Смещение индекса массива:
	usOffsetIndex = uiBitIndex >> 3;

	// Расчет индекса бита внутри байта
	ucBitIndex = (unsigned char) (uiBitIndex & 0x00000007);

	// Цикл выгрузки данных:
	for (i = 0; i < ucBitsCount; i ++)
	{

		if ((*(pcArray + usOffsetIndex)) & (1 << (7 - ucBitIndex)))
		{
			*(piValue) |=  (1 << ((ucBitsCount - 1) - i));
		}
		else
		{
			*(piValue) &= ~(1 << ((ucBitsCount - 1) - i));
		};

		ucBitIndex ++;
		if (ucBitIndex > 7)
		{
			ucBitIndex = 0;
			usOffsetIndex ++;
		};
	};
	return i;
};
/***************************************************************************************************
* Function name: ucHF_GetModulOfChar 
* Description:   
****************************************************************************************************/
unsigned char ucHF_GetModulOfChar (unsigned char ucSrc)
{
	if (ucSrc & 0x80) return (0xFF - ucSrc + 1);
	return ucSrc;
};
/***************************************************************************************************
* Function name: ucHF_GetHorizontalVectors 
* Description:   
****************************************************************************************************/
unsigned char ucHF_GetHorizontalVectors(unsigned char *pcVerticalSrcVector, unsigned char* ucDstH1Vector, unsigned char* ucDstH2Vector)
{
/*	int i;
	unsigned int k;
	float f1, f2;
	unsigned char ucMinValueIndex;
	int iPlaneCoefficient[3];

	// Loocking for index of minimal module of ratio:
	ucMinValueIndex = 0;
	k = 0xFFFFFFFF;
	for (i = 0; i < 3; i ++)
	{
		if (k > (unsigned int)(ucHF_GetModulOfChar(pcVerticalSrcVector[i]))) 
		{
			ucMinValueIndex = i;
			k = ucHF_GetModulOfChar(pcVerticalSrcVector[i]);
		};
	};

	// H1 ration depends on index of min value ov vertical vector ratio;
	if (ucMinValueIndex == 0)
	{
		ucDstH1Vector[0] = 0;
		ucDstH1Vector[1] = (unsigned char)(-((signed char)(pcVerticalSrcVector[2])));
		ucDstH1Vector[2] = (unsigned char)(+((signed char)(pcVerticalSrcVector[1])));
	}
	else if (ucMinValueIndex == 1)
	{
		ucDstH1Vector[1] = 0;
		ucDstH1Vector[0] = (unsigned char)(-((signed char)(pcVerticalSrcVector[2])));
		ucDstH1Vector[2] = (unsigned char)(+((signed char)(pcVerticalSrcVector[0])));
	}
	else
	{
		ucDstH1Vector[2] = 0;
		ucDstH1Vector[0] = (unsigned char)(-((signed char)(pcVerticalSrcVector[1])));
		ucDstH1Vector[1] = (unsigned char)(+((signed char)(pcVerticalSrcVector[0])));
	};
	// Plane coefficients:
	iPlaneCoefficient[0] = ((signed char)(pcVerticalSrcVector[1])) * ((signed char)(ucDstH1Vector[2])) - ((signed char)(pcVerticalSrcVector[2])) * ((signed char)(ucDstH1Vector[1]));
	iPlaneCoefficient[1] = ((signed char)(pcVerticalSrcVector[2])) * ((signed char)(ucDstH1Vector[0])) - ((signed char)(pcVerticalSrcVector[0])) * ((signed char)(ucDstH1Vector[2]));
	iPlaneCoefficient[2] = ((signed char)(pcVerticalSrcVector[0])) * ((signed char)(ucDstH1Vector[1])) - ((signed char)(pcVerticalSrcVector[1])) * ((signed char)(ucDstH1Vector[0]));
	for (i = 0; i < 3; i ++) iPlaneCoefficient[i] = iPlaneCoefficient[i] / 10;

	// Normalisation H1:
	f2 = 0;
	for (i = 0; i < 3; i ++) f2 = f2 + ((signed char)(ucDstH1Vector[i])) * ((signed char)(ucDstH1Vector[i]));
	f2 = sqrt(f2);
	if (f2 > 0) for (i = 0; i < 3; i ++)
	{
		f1 = (signed char)(ucDstH1Vector[i]);
		f1 = f1 * 100;
		f1 = f1 / f2;
		ucDstH1Vector[i] = (unsigned char)((signed char)(f1));
	};

	// Normalisation H2:
	f2 = 0;
	for (i = 0; i < 3; i ++) f2 = f2 + iPlaneCoefficient[i] * iPlaneCoefficient[i];
	f2 = sqrt(f2);
	if (f2 > 0) for (i = 0; i < 3; i ++)
	{
		f1 = iPlaneCoefficient[i];
		f1 = f1 * 100;
		f1 = f1 / f2;
		if (f1 >  100) f1 =  100;
		if (f1 < -100) f1 = -100;
		ucDstH2Vector[i] = (unsigned char)((signed char)(f1));
	};     */

	return 0;
};

