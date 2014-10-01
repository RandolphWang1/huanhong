#include "aliqr.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Main.h"

char qrQueryResult[16] = {0};
struct payInfo qrpay_info;
static unsigned long long query_number = 0;

int  generator_qrcode_to_bmp(int out, char* price)
{
char szQrcodeString[1024] = {0};
char* szSourceString;
// code from d620d start
int ret;
int enc_mode;
// code from d620d end


struct tm *ptr;
time_t td;
char ticket_number[13]={0};
char client_number[21]={0};
char serial_number[28]={0};


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
//strcpy(qrpay_info.total_fee,"0.01");^M
strcpy(qrpay_info.order_subject,"ccc");
strcpy(qrpay_info.order_time,"2014-08-0514:15:30");

/* print the qr code from alipay */
alipay_main(szQrcodeString, &qrpay_info, ALI_PRECREATE_ORDER);
szSourceString = szQrcodeString;

/* print QR code on D620D */
         ret = PrintQR(5, 1, 2, szSourceString, 5, 5);
         if (0 > ret)
         {
              printf("the PrintQR return value is %d\n",ret);
         }
return 0;
}
