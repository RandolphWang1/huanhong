/*=============================================================================
    File Name: UTILITY.H
    Date: 2006.5.27
=============================================================================*/
#ifndef __UTILITY_H
#define __UTILITY_H

#include "SDKInc/_Type.h"

#define SPACE           0x20


extern const CHAR gBin2HexTbl[16];
extern const WORD gCRCTbl[256];

//void TxStr(BYTE PortID, PSTR pstzStr);
//void TxNData(BYTE PortID, CHAR *pData, INT cbBytesToSend);
void TxStr(PSTR pstzStr);
void TxNData(CHAR *pData, INT cbBytesToSend);
BYTE ToUpper(BYTE uch);
BOOL IsHexa(BYTE uch);
BYTE Hex2Bin(BYTE uch);
BYTE Bin2Hex(BYTE uch);
void BCD2Dec(BYTE uch,CHAR achBuff[]);
BYTE BCD2Bin(BYTE uch);
BYTE Dec2BCD(CHAR *pDecStr);
BYTE Dec2Bin(CHAR *pDecStr);
LONG Dec2Long(CHAR *pDecStr, int index);
void Word2Dec(WORD wData, CHAR achBuff[]);
void Long2Dec(LONG lData, CHAR achBuff[]);

WORD Dec2Word(CHAR *pDec);
INT DecN2Int(CHAR *pDec, INT nSize);
BOOL HexAddr2BinAddr(BYTE *pbStrAddr, BYTE** ppbBinAddr);
void Byte2Hex(BYTE uchData, CHAR *pBuff);
void Word2Hex(WORD wData, CHAR *pBuff);
void Long2Hex(LONG lData, CHAR *pBuff);
void NChar2Hex(void *pData, void *pBuff, INT cbBytesToConvert);
void Hex2NStr(void *pStrDest, void *pHexSrc, INT StrBuffLen);
void Hex2NStr1(void *pStrDest, void *pHexSrc, INT StrBuffLen); // added by sh at 2004/03/26
INT Str2Hex(CHAR *pDest, CHAR *pSrc, INT cbSrcLen);
BOOL Hex2Str(void * pvSrc, void * pvDest, INT nLength);
INT Int2Str(INT Integer, CHAR *pBuff, INT Count);

//
// string
//
void StrCpy(CHAR *pDest, const CHAR  *pSrc);
INT  StrLen(const CHAR *pStr);
INT  StrLenFS(const CHAR *pStr);
INT  StrCat(CHAR* Dest, const CHAR *Src);
CHAR Str2Money(const CHAR *Str,CHAR *MoneyStr);
CHAR money2float(CHAR *dest, const CHAR *src);       // added by ksh@0403
INT  TrimRightSpace(CHAR *pDest, const CHAR *pSrc, INT nLen);
INT  TrimLeftZero(CHAR *pDest, const CHAR *pSrc, INT nLen);

//
// memory
//
void MemSet(VOID  *pStart, CHAR ch, WORD wSize);
void MemCpy(VOID  *pDest,  const VOID  *pSrc, WORD wSize);
INT  MemSCpy(LPSTR pDest, LPSTR pSrc, INT Length);
INT  MemFCpy(LPSTR pDest, LPSTR pSrc, INT Length);
BOOL MemCmp(LPSTR pDest, LPSTR pSrc, INT cbData);
INT  MemSCpyToFlagChar(LPSTR pDest, const CHAR  *pSrc, INT nLen,
                       CHAR FlagChar);
INT  MemSCpyAndFillCh(LPSTR pDest, const CHAR * pSrc, INT nLen, CHAR FillCh);
BOOL CheckMemoryMapViolation(void  *Ptr);

//
// Date Time
//
void GetSysTime(CHAR *pBuff);
INT GetSysDate(CHAR *pBuff);


//
// Check
//
UCHAR CalcLRC(CHAR *pData, INT cbByteToCalculate);
WORD CalcCRC(void * pBuff, INT nSize);
WORD CheckSum(UCHAR *pStartAddr, LONG Len);

//
// Calender
//
const CHAR *GetDayTable(WORD Year);
BYTE GetADayOfWeek(WORD Year, WORD Month, WORD Day);
INT FillCh(CHAR *pBuff, CHAR ch, INT Cnt);
WORD CheckSum(UCHAR *pStartAddr, LONG Len);

void Delay_ms(INT ms);


#endif  // _UTILITY_H
