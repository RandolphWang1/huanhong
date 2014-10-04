#include "aliqr.h"
#include "qrgenerator.h"
#include <stdio.h>
#include <string.h>
#include "Main.h"

char qrQueryResult[16] = {0};
struct payInfo qrpay_info;
unsigned long long query_number = 0;
char pos_imsi[20];

char szQrcodeString[QRRESULT] = {0};
int  generator_qrcode_to_bmp(int out, char* price)
{
    char* szSourceString = NULL;
    // code from d620d start
    int ret;
    int enc_mode;
#if 0
    FILE *fp;
    int i;
    char buffer[30];
#endif
    // code from d620d end


    T_DATETIME tTime;
    char ticket_number[13]={0};
    char client_number[21]={0};
    char serial_number[28]={0};

#if 0
    //strcpy(qrpay_info.imsi,"460006922139942");
    //strcpy(qrpay_info.imsi,"460024104033474");
    if (pos_imsi[0] == '\0'){
        /* get imsi from config.tx */
        fp = fopen("/usr/local/config.txt","r");
        if(fp == NULL)
        {
            printf("couldn't open config.txt\n");
            return;
        }
        if( fgets(buffer, 30, fp) == NULL )
        {
            printf("Error reading config\n");
            fclose(fp);
            return ;
        }
        for (i=0; i<30; i++) {
            if(buffer[i] == '\n') {
                buffer[i] = '\0';
                break;
            }
        }
        fclose(fp);
        /* copy after IMSI: */
        strcpy(pos_imsi,&buffer[5]);
        printf("the pos imsi buffer string is %s\n",pos_imsi);
    }
    strcpy(qrpay_info.imsi,pos_imsi);
#else
    getIMSIconfig();
#endif
    strcpy(qrpay_info.order_key,"11");
    /* Time for D620D Pos */
    GetDateTime(&tTime);
    if(query_number == 0) {
        sprintf(ticket_number,"%s%s%s%s%s00",tTime.year,tTime.month,tTime.day,tTime.hour,tTime.minute);
        /* use last 6-bit of IMSI */
        strncpy(client_number, &(qrpay_info.imsi[10]), 6);
        strcat(client_number, ticket_number);
        query_number = (unsigned long long)atoll(client_number);
    }


    query_number = query_number + 1;
    qrpay_info.order_number = query_number;
    sprintf(serial_number,"NO:%lld\n",query_number);
    strcpy(qrpay_info.total_fee,price);
    //strcpy(qrpay_info.total_fee,"0.01");^M
    //strcpy(qrpay_info.order_subject,"ccc");
    strcpy(qrpay_info.order_subject,"GoldenLakeCafe");
    strcpy(qrpay_info.order_time,"2014-08-0514:15:30");
    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    /* print the qr code from alipay */
    alipay_main((struct qr_result*)szQrcodeString, &qrpay_info, ALI_PRECREATE_ORDER);
    szSourceString = szQrcodeString;
    if(szQrcodeString) {
        /* print QR code on D620D */
        //ret = PrintQR(10, 1, 2, szSourceString, 5, 5);
        //ret = PrintQR(6, 1, 2, szSourceString, 5, 7);
        ret = PrintQR(6, 3, 2, szSourceString, 50, 7);
        if (0 > ret)
        {
            printf("the PrintQR return value is %d\n",ret);
        }
    }
    return 0;
}

//user don't need to input imsi and year month date, but hour,minutes,and serial no is needed,
void getSNoPre(char* prefix_str)
{
    T_DATETIME tTime;
    char ticket_number[13]={0};
    char client_number[21]={0};
    getIMSIconfig();
    GetDateTime(&tTime);
    memset(ticket_number, 0, 13);
    memset(client_number, 0, 21);
    sprintf(ticket_number,"%s%s%s\0",
            tTime.year, tTime.month, tTime.day);
    /* use last 6-bit of IMSI */
    strncpy(client_number, &(qrpay_info.imsi[10]), 6);
    strcat(client_number, ticket_number);
    memcpy(prefix_str, client_number,strlen(client_number));
    printf("ticket_number:%s, client_number:%s, prefix:%s\n",
            ticket_number, client_number, prefix_str);
}

void getIMSIconfig()
{
    FILE *fp;
    int i;
    char buffer[30];
    //strcpy(qrpay_info.imsi,"460006922139942");
    //strcpy(qrpay_info.imsi,"460024104033474");
    if (pos_imsi[0] == '\0'){
        /* get imsi from config.tx */
        fp = fopen("/usr/local/config.txt","r");
        if(fp == NULL)
        {
            printf("couldn't open config.txt\n");
            return;
        }
        if( fgets(buffer, 30, fp) == NULL )
        {
            printf("Error reading config\n");
            fclose(fp);
            return ;
        }
        for (i=0; i<30; i++) {
            if(buffer[i] == '\n') {
                buffer[i] = '\0';
                break;
            }
        }
        fclose(fp);
        /* copy after IMSI: */
        strcpy(pos_imsi,&buffer[5]);
        printf("the pos imsi buffer string is %s\n",pos_imsi);
    }
    strcpy(qrpay_info.imsi,pos_imsi);
}


int alipay_query_single(unsigned long long queryNo)
{
    int ret = 0;
    getIMSIconfig();
    strcpy(qrpay_info.order_key,"11");
    qrpay_info.order_number = queryNo;
    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    alipay_main((struct qr_result*)szQrcodeString, &qrpay_info, ALI_PRECREATE_QUERY_SINGLE);
    if(strcmp("TRADE_SUCCESS", szQrcodeString) == 0)
        ret = 1;
    else
        ret = 0;
    return ret;
}

int alipay_query_24h(char* query_24h)
{
    int ret = 0;
    getIMSIconfig();
    strcpy(qrpay_info.order_key,"11");
    alipay_main((struct qr_result*)query_24h, &qrpay_info, ALI_PRECREATE_QUERY_24H);
    return ret;
}
