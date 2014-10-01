#ifndef __CONVERT_H_
#define __CONVERT_H_


//-*******************************************************************
// Abbreviations:
// 		asc = ASCII fixed size string
// 		bcd = Binary Coded Decimal (packet numeric digits as 4 bit items)
// 		str = Null terminated strings (C style)
//  	av2 = ASCII string preceeded by 2 digit length
// 		bv2 = BCD string preceeded by 2 digit length
// 		bv3 = BCD string preceeded by 3 digit length
// 		av3 = ASCII string preceeded by 3 digit length
// 		xbc = "Signed" ("C"-credit side or "D"-debit side) followed by BCD string
// 		bit = Bit string
//-*********************************************************************
#define Add			1
#define Minus			0

#define ASC_ASC      0
#define AV3_AV3      1
#define BIT_BIT      2
#define BCD_BCD      3
#define BCD_ASC      4
#define ASC_STR      5
#define BCD_STR      6
#define BCD_SNZ      7
#define AV2_STR      8
#define BV2_STR      9
#define AV3_STR     10
#define XBC_STR     11
#define BV3_STR     12
#define CONVERTOR_END 13
//#define VARIANT     13          //This must be last +1
//#define COMPUTE     (VARIANT+1)

extern UCHAR guchPad_Nibble_8583;

typedef VOID (* (converters[2]) )(INT);  //a pair or pointers to functions

extern const converters convert_table[];


//---------------------------------------------------------
// Straight movers

VOID asc_to_asc(INT);   // Straight transfer of N bytes
VOID av3_to_av3(INT);   // Straight transfer of 2 byte counted string
VOID bit_to_bit(INT);   // Straight transfer of bits. Whole bytes.
VOID bcd_to_bcd(INT);   // Straight transfer of N nibbles. Whole bytes.

//---------------------------------------------------------
// Convert ASCII bytes to/from BCD nibbles

VOID asc_to_bcd(INT);   // N bytes of ASCII to N nibbles of BCD
VOID bcd_to_asc(INT);   //  N nibbles of BCD to N bytes of ASCII

//---------------------------------------------------------
// Convert ASCII bytes to/from c-cstring

VOID asc_to_str(INT);   // N bytes of ASCII to C-string
VOID str_to_asc(INT);   // C-string to N bytes, pad on right with space

//---------------------------------------------------------
// Convert BCD to/from C-string

VOID bcd_to_str(INT);   // N nibbles of BCD to C-string, preserve lead 0
VOID str_to_bcd(INT);   // C-string to N nibbles, pad on left with 0
VOID bcd_to_snz(INT);   // N nibbles of BCD to C-string, drop lead 0

//---------------------------------------------------------
// Convert short counted ASCII string to/from C-string

VOID av2_to_str(INT);   // 1 byte counted string of ASCII to C-string
VOID str_to_av2(INT);   // C-string to 1 byte counted string of ASCII

//---------------------------------------------------------
// Convert short counted BCD string to/from null terminated string

VOID bv2_to_str(INT);   // 1 byte counted string of BCD to C-string
VOID str_to_bv2(INT);   // C-string to 1 byte counted string of BCD

//---------------------------------------------------------
// Convert short counted BCD string to/from null terminated string

VOID bv3_to_str(INT);   // 2 byte counted string of BCD to C-string
VOID str_to_bv3(INT);   // C-string to 2 byte counted string of BCD

//---------------------------------------------------------
// Convert short counted BCD string to/from null terminated string

VOID bv3_to_str(INT);   // 2 byte counted string of BCD to C-string
VOID str_to_bv3(INT);   // C-string to 2 byte counted string of BCD

//---------------------------------------------------------
// Convert LONG counted ASCII string to/from null terminated string

VOID av3_to_str(INT);   // 2 byte counted string of ASCII to C-string
VOID str_to_av3(INT);   // C-string to 2 byte counted string of ASCII

//---------------------------------------------------------
// Convert "signed" ("C" or "D") followed by BCD string

VOID xbc_to_str(INT);   // Signed BCD to C-string
VOID str_to_xbc(INT);   // C-string to 2 signed BCD
void HEX2ASC(char *dest_asc, char *src_bcd, char src_len);
void ASC2HEX(char *dest_bcd, char *src_asc, char src_len);
INT SumAmount(CHAR* Amount,CHAR* SumAmount,CHAR AddMinusFlag);
int Ascii_To_Bcd(unsigned char* Source, unsigned char* Dest, int  SourceLength);
void Bcd_To_Ascii(unsigned char *Source,unsigned char *Dest,unsigned char SourceLength);
void Word2TwoByte(UCHAR * pDest,UCHAR * pSource,INT SourceLen);
void TwoByte2Word(UCHAR * pDest,UCHAR * pSource,INT SourceLen);
void Bcd_To_Ascii(unsigned char *Source,unsigned char *Dest,unsigned char SourceLength);


#endif //#ifndef _CONVERT_H_
