//	---------------------------------------------------------------------------
//		
//		QRGenerator
//
//		Create:		15/05/2013
//		Last update:	15/05/2013
//
//		Author:	TWOTM
//
//
//		Note:
//
//		/o ULTRAMUNDUM FOUNDATION - all rights reserved
//	---------------------------------------------------------------------------


//	-------------------------------------------------------
//	Includes
//	-------------------------------------------------------

//#include "stdafx.h"
#include <string.h>
#include <errno.h>
//#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#include <unistd.h>
#include <signal.h>

#include "qrencode.h"
#include "aliqr.h"
#include <time.h>
//	-------------------------------------------------------


//	-------------------------------------------------------
//	DEFines
//	-------------------------------------------------------

//#define QRCODE_TEXT					"https://qr.alipay.com/apvn2t5fxkb7d1z695";		// Text to encode into QRCode
#define QRCODE_TEXT					"https://qr.alipay.com/ap8igdp2fuwd3bud65";		// Text to encode into QRCode
#define OUT_FILE					"test.bmp"					// Output file name
#define LOGO_FILE                   "/var/ftp/pub/logo.bmp"
#define OUT_FILE_PIXEL_PRESCALER	8								// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)

#define PIXEL_COLOR_R				0							// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0xff

	// BMP defines

typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;

#define BI_RGB			0L

//#pragma pack(push, 2)

typedef struct  
	{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
	}  __attribute__((packed, aligned( 1 )))BITMAPFILEHEADER;

#define length_bitmapfileheader = 0x0e
typedef struct 
	{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
	} __attribute__((packed, aligned( 1 )))BITMAPINFOHEADER;

//#pragma pack(pop)

//	-------------------------------------------------------


unsigned char *pBmpBuff;
unsigned char* pData = NULL;
char qrQueryResult[16] = {0};
struct payInfo qrpay_info;
static unsigned long long query_number = 0;
//	-------------------------------------------------------
//	Main
//	-------------------------------------------------------

//int _tmain(int argc, _TCHAR* argv[])
//int main(void)
int  generator_qrcode_to_bmp(int out, char* price)
{
//char*			szSourceString = QRCODE_TEXT;
char szQrcodeString[1024] = {0};
char*			szSourceString;
unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
unsigned char*	pRGBData, *pSourceData, *pDestData;
QRcode*			pQRC;
FILE*			f;
unsigned char data[] = {0x1B,0x33,0x00};
unsigned char escBmp[] = { 0x1B, 0x2A, 0x00, 0x00, 0x00 };
//int i,j,k;
// code from d620d start
int ret;
int enc_mode; 
// code from d620d end
unsigned char cutCommand[]= {0x1d,0x56,0x31,0x1d,0x72,0x01};
unsigned char escd[]= {0x1b,0x64,0x05};
unsigned char chinesecmd[]={0x1c,0x26};
unsigned char companyname[]={0xd3,0xaf,0xc8,0xf3,0xbd,0xdd,0xcd,0xa8,0x0a};
FILE* fd;
int logo_bmpWidth,logo_bmpHeight;

struct tm *ptr;
time_t td;
char ticket_number[13]={0};
char client_number[21]={0};
char serial_number[28]={0};

//struct payInfo qrpay_info;
#if 0
qrpay_info.imsi = (char *)malloc(17*sizeof(char));
memset(qrpay_info.imsi,0,17*sizeof(char));
strcpy(qrpay_info.imsi,"123456789012345");
qrpay_info.order_key = (char *)malloc(33*sizeof(char));
memset(qrpay_info.order_key,0,33*sizeof(char));
strcpy(qrpay_info.order_key,"11");
qrpay_info.order_number = 22;
//strcpy(qrpay_info.total_fee,price); 
qrpay_info.total_fee = (char *)malloc(16*sizeof(char));
memset(qrpay_info.total_fee,0,16*sizeof(char));
strcpy(qrpay_info.total_fee,"0.01"); 
qrpay_info.order_subject = (char *)malloc(129*sizeof(char));
memset(qrpay_info.order_subject,0,129*sizeof(char));
strcpy(qrpay_info.order_subject,"ccc");
qrpay_info.order_time = (char *)malloc(20*sizeof(char));
memset(qrpay_info.order_time,0,20*sizeof(char));
strcpy(qrpay_info.order_time,"2014-08-0514:15:30");
#endif
strcpy(qrpay_info.imsi,"460006922139942");
strcpy(qrpay_info.order_key,"11");

if(query_number == 0) {
   time(&td);
   ptr = (struct tm *)localtime(&td);
   /* start from yymmddhhmm00 */
   if (ptr->tm_year < 100 || ptr->tm_year > 141)
   strftime(ticket_number,sizeof(ticket_number),"%m%d%H%M00",ptr);
   else 
   strftime(ticket_number,sizeof(ticket_number),"%y%m%d%H%M00",ptr);
   /* use first 6-bit of IMSI */
   strncpy(client_number, qrpay_info.imsi, 6);
   strcat(client_number, ticket_number);
   query_number = (unsigned long long)atoll(client_number);
}
query_number = query_number + 1;
qrpay_info.order_number = query_number;
sprintf(serial_number,"NO:%lld\n",query_number);
strcpy(qrpay_info.total_fee,price); 
//strcpy(qrpay_info.total_fee,"0.01");
strcpy(qrpay_info.order_subject,"ccc");
strcpy(qrpay_info.order_time,"2014-08-0514:15:30");

/*
 * Create a symbol from the string. The library automatically parses the input
 * string and encodes in a QR Code symbol.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param string input string. It must be NUL terminated.
 * @param version version of the symbol. If 0, the library chooses the minimum
 *                version for the given input data.
 * @param level error correction level.
 * @param hint tell the library how non-alphanumerical characters should be
 *             encoded. If QR_MODE_KANJI is given, kanji characters will be
 *             encoded as Shif-JIS characters. If QR_MODE_8 is given, all of
 *             non-alphanumerical characters will be encoded as is. If you want
 *             to embed UTF-8 string, choose this.
 * @param casesensitive case-sensitive(1) or not(0).
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version. On error, NULL is returned,
 *         and errno is set to indicate the error. See Exceptions for the
 *         details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw ERANGE input data is too large.
 */
/* print the qr code from alipay */
alipay_main(szQrcodeString, &qrpay_info, ALI_PRECREATE_ORDER);
szSourceString = szQrcodeString;

		// Compute QRCode

/* print QR code on D620D */
         ret = PrintQR(5, 1, 2, szSourceString, 5, 5);
         if (0 > ret)
         {
              printf("the PrintQR return value is %d\n",ret);
         }
#if 0
	if (pQRC = QRcode_encodeString(szSourceString, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
		{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

			// Allocate pixels buffer

		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
			{
			printf("Out of memory");
			exit(-1);
			}
			
			// Preset to white

		memset(pRGBData, 0xff, unDataBytes);


			// Prepare bmp headers

		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize =	sizeof(BITMAPFILEHEADER)   +
								sizeof(BITMAPINFOHEADER) +
								unDataBytes;
               printf("BITMAPFILEHEADER =%x, BITMAPINFOHEADER=%x, unDataBytes=%x,kFileHeader.bfSize%x\n",sizeof(BITMAPFILEHEADER) , sizeof(BITMAPINFOHEADER), unDataBytes,kFileHeader.bfSize);
                printf("DWORD=%d, WORD=%d\n",sizeof(DWORD),sizeof(WORD));
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits =	sizeof(BITMAPFILEHEADER)  +
								sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;


			// Convert QrCode bits to bmp pixels

		pSourceData = pQRC->data;
		for(y = 0; y < unWidth; y++)
			{
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
			for(x = 0; x < unWidth; x++)
				{
				if (*pSourceData & 1)
					{
					for(l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
						{
						for(n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
							{
							*(pDestData +		n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_B;
							*(pDestData + 1 +	n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_G;
							*(pDestData + 2 +	n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_R;
							}
						}
					}
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
				}
			}


			// Output the bmp file

                f = fopen(OUT_FILE,"wb");
		//if (!(fopen_s(&f, OUT_FILE, "wb")))
			{
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER) , 1, f);
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER) , 1, f);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
			
			fclose(f);
			}

                   printf("end\n");
                   //write(out,cutCommand,6);
#if 0 //logo   
			readBmp(LOGO_FILE,&logo_bmpWidth,&logo_bmpHeight);
			printf("logo_bmpWidth=%d,logo_bmpHeight=%d\n",logo_bmpWidth,logo_bmpHeight);
			if(logo_bmpHeight<0){//not test
            	for(j=0;j<-logo_bmpHeight;j++){
					for(i=0;i<logo_bmpWidth*3;i++)
                 		pRGBData[kInfoHeader.biWidth*3*((-kInfoHeader.biHeight)/2+j )+ (-kInfoHeader.biHeight)/2*3+i] =pBmpBuff[i+j*logo_bmpWidth*3]; 
			    }
			}
			else
			{
				for(i=0;i<logo_bmpHeight;i++)
				{
	    			memcpy(pData + i*logo_bmpWidth*3, pBmpBuff+ (logo_bmpHeight-i-1) * logo_bmpWidth*3 , logo_bmpWidth*3);
				}   
                for(j=0;j<logo_bmpHeight;j++){
					for(i=0;i<logo_bmpWidth*3;i++)
		                pRGBData[kInfoHeader.biWidth*3*((-kInfoHeader.biHeight)/2 +j )+ ((-kInfoHeader.biHeight)/2 - logo_bmpWidth/2)*3 +i] =pData[i+j*    logo_bmpWidth*3];
				}
			}
			free(pBmpBuff);
			free(pData);
			//save again
			f = fopen("adi.bmp","wb");
		    {   
			   fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER) , 1, f);
			   fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER) , 1, f);
			   fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
			   fclose(f);
			}

#endif //logo

                   write(out,data,sizeof(data));
                   data[0] = 0x00;
                   data[1] = 0x00;
                   data[2] = 0x00;
                   escBmp[2] =0x21;
                   //nL, nH
                 escBmp[3] = (kInfoHeader.biWidth % 256);
                 escBmp[4] = (kInfoHeader.biWidth / 256);
                 printf("width=%d, height=%d\n",kInfoHeader.biWidth,kInfoHeader.biHeight);
                 // data
                 for (i = 0; i < (-kInfoHeader.biHeight / 24) + 1; i++)
                 {
                     write(out,escBmp,sizeof(escBmp));
                     for (j = 0; j < kInfoHeader.biWidth; j++)
                     {
                         for (k = 0; k < 24; k++)
                         {
                             if (((i * 24) + k) < -kInfoHeader.biHeight)   // if within the BMP size
                             {
                                 if(pRGBData[(((i*24)+k)*kInfoHeader.biWidth +j)*3+2] == 0)
                                 {
                                     data[k / 8] += (128 >> (k % 8));
                                 }
                             }
                         }
                         write(out,data,3);
                         data[0] = 0x00;
                         data[1] = 0x00;
                         data[2] = 0x00;    // Clear to Zero.
                        
                     }
                         write(out,"\n",1);
			 sleep(1);
                 } // data
                 printf("bmp print end\n");
#if 1
                         data[0] = 0x1c;
                         data[1] = 0x26;
                         write(out,data,2);   //set to chinese print

                         write(out,companyname,sizeof(companyname));

                         data[0] = 0x1c;
                         data[1] = 0x2e;
                         write(out,data,2);   //escape from chinese print

                         data[0] = 0x1b;
                         data[1] = 0x40;
                         write(out,data,2);

                         data[0] = 0x0a;
                         write(out,data,1);
                         write(out,serial_number,28);

                         data[0] = 0x1b;
                         data[1] = 0x33;
                         data[2] = 0x33;    // Clear to Zero.
                         write(out,data,3);

                         data[0] = 0x1b;
                         data[1] = 0x64;
                         data[2] = 0x05;
                         write(out,data,3); // head 5 lines for cut cmd

#if 0
             write(out,"\n",1);
			 //sleep(1);
                         write(out,"\n",1);
			 //sleep(1);
                         write(out,"\n",1);
			 //sleep(1);
                         write(out,"\n",1);
			 //sleep(1);
                         write(out,"\n",1);
#endif
                   write(out,cutCommand,6);
#endif
		free(pRGBData);
		QRcode_free(pQRC);
		}
	else
		{
		printf("NULL returned");
		exit(-1);
		}
#endif

	return 0;
}
//	-------------------------------------------------------
#define WIDTHBYTES(i) ((i+31)/32*4)  
#define DEBUG 1
int readBmp(char *bmpName,int *bmpWidth,int *bmpHeight)
{
#ifdef DEBUG
	    FILE *f;
#endif
	    DWORD LineByte,ImgSize;   
		BITMAPFILEHEADER fileheader;
		BITMAPINFOHEADER head;	//信息头
		FILE *fp=fopen(bmpName,"rb");
		if(0==fp)
			return 1;
#ifndef DEBUG
		fseek(fp,sizeof(BITMAPFILEHEADER),0);	//跳过位图文件头
#else
		fread(&fileheader,sizeof(BITMAPFILEHEADER),1,fp);
#endif
		//printf("bfsize=%d\n",fileheader.bfSize);
		fread(&head,sizeof(BITMAPINFOHEADER),1,fp);
		LineByte=(DWORD)WIDTHBYTES(head.biWidth*head.biBitCount); //计算位图的实际宽度并确保它为32的倍数  
		ImgSize=(DWORD)LineByte*head.biHeight; 
		printf("LineByte=%ld, ImgSize=%ld\n",LineByte,ImgSize);
							
		*bmpWidth=head.biWidth;
	    *bmpHeight=head.biHeight;
		//biBitCount=head.biBitCount;
										

		if (!(pBmpBuff = (unsigned char*)malloc(ImgSize)))
		{
			fclose(fp);
			return 2;
		}
		pData = (unsigned char*)malloc(ImgSize);
		fread(pBmpBuff,ImgSize,1,fp);
		fclose(fp);
#ifdef DEBUG
		f = fopen("test1.bmp","wb");
		if (f)
	    {
	        fwrite(&fileheader, sizeof(BITMAPFILEHEADER) , 1, f);
		    fwrite(&head, sizeof(BITMAPINFOHEADER) , 1, f);
		    fwrite(pBmpBuff, sizeof(unsigned char), ImgSize, f);
			fclose(f);
		}
#endif
		//free(pBmpBuff);
	    return 0;
}
