
#if!defined __TYPE_
#define	__TYPE_


typedef char					CHAR;
typedef char*					PSTR;
typedef char*					LPSTR;

typedef unsigned char			UCHAR;
typedef short int				INT;       /* 2Bytes ,D620是32位处理器,如果直接用int 则是4Bytes*/
typedef unsigned short int		UINT;
typedef long					LONG;
typedef unsigned long			ULONG;

typedef UCHAR*					FPTR;
typedef UCHAR*					LPBYTE;

typedef float					FLOAT;
typedef double					DOUBLE;
typedef long double				LDOUBLE;

typedef void					VOID;

typedef char					INT8;       /* Signed 8-bit integer */
typedef unsigned char			UINT8;      /* Unsigned 8-bit integer */
typedef short int				INT16;      /* Signed 16-bit integer */
typedef unsigned short int		UINT16;     /* Unsigned 16-bit integer	*/
typedef long int				INT32;      /* Signed 32-bit integer */
typedef unsigned long int		UINT32;     /* Unsigned 32-bit integer */

typedef float					FLOAT32;    /* 32-bit IEEE single precision */
typedef double					FLOAT64;    /* 64-bit IEEE double precision */
typedef long double				FLOAT80;    /* 80-bit IEEE max precision */

typedef void*					PTR;        /* Pointer to any data type */
typedef UINT8*					PTR8;       /* Pointer to 8-bit data */
typedef UINT16*					PTR16;      /* Pointer to 16-bit data */
typedef UINT32*					PTR32;      /* Pointer to 32-bit data */


typedef unsigned char			BYTE;       /* 8-bit data */
typedef unsigned short int		WORD;       /* 16-bit data */
typedef unsigned long int		DWORD;      /* 32-bit data */

typedef BYTE*					BYTE_PTR;   /* Pointer to 8-bit data */
typedef WORD*					WORD_PTR;   /* Pointer to 16-bit data */
typedef DWORD*					DWORD_PTR;  /* Pointer to 32-bit data */

typedef int					BOOL;

#define	lowbyte(word)        ((word) & 0xff)
#define	highbyte(word)       lowbyte((word) >> 8)
#define	dim(x)               (sizeof(x) / sizeof(x[0]))
#define	setvect(inum, addr)  *((ISRP  *) ((inum) * 4)) = ((ISRP) addr)
#define	getvect(inum)        (ISRP) (*((ISRP  *) ((inum) * 4)))

#if	!defined(min)
#define max(a,b)             (((a) > (b)) ? (a) : (b))
#define min(a,b)             (((a) < (b)) ? (a) : (b))
#endif


#define OFF					0
#define ON					1

#define NO					0
#define YES					1             

#define OK					0
#define ERROR				-1
#define TIMEOUT				-2
#define CANCEL				-3
#define ERROR_CONN			-4
#define ERROR_SEND			-5
#define ERROR_RECV			-6


#define TRUE					1
#define FALSE				0


#define  TEXTOUT_START         9
#define  TEXTOUT_CONN           0
#define  TEXTOUT_END             1

#endif

