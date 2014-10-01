#include "md5.h"
#include "aliqr.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#if 0  //namie_liu's PID and Key alipay seller apk not work for face pay
static char partner[17]= "2088002374756150";
static char* key = "gzyypc49bgtwqu3sykph20lc2fn3jttt";
#else //PID and Key of alipay-test20@alipay.com 
static char partner[17]= "2088201565141845";
static char* key = "ai1ce2jkwkmd3bddy97z0xnz3lxqk731";
#endif

#if 1//precreate
char* service = "alipay.acquire.precreate";
#else //query
char* service = "alipay.acquire.query";
#endif

extern  char time_mark[32];
char* _input_charset = "utf-8";
//char out_trade_no[65] = "333307617728412";
char out_trade_no[65] = "353307617728127";
char* subject = "testGPRS_uninet";
char* product_code = "QR_CODE_OFFLINE";
char* total_fee = "0.03";
char* goods_detail = "[{\"goodsName\":\"ipad\",\"price\":\"0.01\",\"quantity\":\"1\"}]";
char* royalty_parameters="[{\"serialNo\":\"1\",\"transOut\":\"2088201565141845\",\"transIn\":\"2088002374756150\",\"amount\":\"0.01\",\"desc\":\"分账测试1\"}";
#if 0
int alipay_precreatebak(char* precr, int* len)
{
	char encrypt[1024];
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;
	//without goods_detail
	//sprintf(encrypt,"_input_charset=%s&out_trade_no=%s&partner=%s&product_code=%s&service=%s&subject=%s&total_fee=%s%s",_input_charset, out_trade_no,partner,product_code,service,subject,total_fee,key);
	//with goods_detail
	//sprintf(encrypt,"_input_charset=%s&goods_detail=%s&out_trade_no=%s&partner=%s&product_code=%s&service=%s&subject=%s&total_fee=%s%s",_input_charset, goods_detail, out_trade_no,partner,product_code,service,subject,total_fee,key);
	//with royalty
	sprintf(encrypt,"_input_charset=%s&goods_detail=%s&out_trade_no=%s&partner=%s&product_code=%s&royalty_parameters=%s&service=%s&subject=%s&total_fee=%s%s",_input_charset, goods_detail, out_trade_no,partner,product_code,royalty_parameters,service,subject,total_fee,key);
	printf("\nMD5 input:encrypt=\n%s", encrypt);

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)encrypt, strlen(encrypt));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	printf("\nencrypt output:\n");
	puts(hex_output);

	printf("\nalipay.acquire.precreate:\n");
	char https[1024*3];
	char encrypt1wokey[1024*3];
	//without goods_detail
	//sprintf(encrypt1wokey,"_input_charset=%s&out_trade_no=%s&partner=%s&product_code=%s&service=%s&subject=%s&total_fee=%s",_input_charset,out_trade_no,partner,product_code,service,subject,total_fee);
	//with goods_detail
	//sprintf(encrypt1wokey,"_input_charset=%s&goods_detail=%s&out_trade_no=%s&partner=%s&product_code=%s&service=%s&subject=%s&total_fee=%s",_input_charset,goods_detail,out_trade_no,partner,product_code,service,subject,total_fee);
	sprintf(encrypt1wokey,"_input_charset=%s&goods_detail=%s&out_trade_no=%s&partner=%s&product_code=%s&royalty_parameters=%s&service=%s&subject=%s&total_fee=%s",_input_charset,goods_detail,out_trade_no,partner,product_code,royalty_parameters,service,subject,total_fee);
	*len = sprintf(https,"https://mapi.alipay.com/gateway.do?%s&sign_type=MD5&sign=%s",encrypt1wokey,hex_output);
	puts(https);
	memset(precr, 0, *len+1);
	memcpy(precr, https, *len);
	return *len;
}
#endif
#if 1
char* jfserver = "182.92.186.90";
int portnumber = 8080;
#else
char* jfserver = "192.168.1.104";
int portnumber = 8180;
#endif

char* IMSI = "123456789012345";
char* jfkey= "11";
int serial_number = 20;
char* jftotal_fee = "0.01";
char* jfsubject = "ccc";
char* order_time ="2014-08-0211:21:20";
char* str_timemark = "1408001801550";
//char* time_mark = "1408002548964";
int alipay_precreate(char* precr, int* len, struct payInfo* order_info, int type)
{
    if (type == ALI_PRECREATE_ORDER)
    //preorder
    alipay_preorder(precr,len,order_info);
    else if(type == ALI_PRECREATE_QUERY)
    //queryorder
    //alipay_queryorder(precr,order_info);
    alipay_query(precr,len,order_info->imsi);
}
int alipay_preorder(char* precr, int* len, struct payInfo* order_info)
{
    
	char https[1024];
    
	char encrypt[1024];
	char encrypt1wokey[1024];
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;
//	*len = sprintf(encrypt,"IMSI=%s&order_time=%s&serial_number=%d&subject=%s&total_fee=%s#%s", 
//                            IMSI, order_time, serial_number, jfsubject, jftotal_fee,jfkey);
        *len = sprintf(encrypt,"IMSI=%s&order_time=%s&serial_number=%d&subject=%s&total_fee=%s#%s",
                              order_info->imsi, order_info->order_time, order_info->order_number, order_info->order_subject, order_info->total_fee, order_info->order_key);
	printf("\nMD5 input:encrypt=%s", encrypt);

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)encrypt, strlen(encrypt));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	printf("\nencrypt output:");
	puts(hex_output);

	//*len = sprintf(encrypt1wokey,"IMSI=%s&serial_number=%d&total_fee=%s&subject=%s&order_time=%s", IMSI, serial_number, jftotal_fee, jfsubject, order_time);
	*len = sprintf(encrypt1wokey,"IMSI=%s&serial_number=%d&total_fee=%s&subject=%s&order_time=%s", order_info->imsi, order_info->order_number, order_info->total_fee, order_info->order_subject, order_info->order_time);
	//*len = sprintf(https,"http://192.168.1.104:8180/qrcode/preorder/?IMSI=123456789012345&serial_number=12&total_fee=0.01&subject=ccc&order_time=2014-08-0211:21:20");
	//*len = sprintf(https,"http://%s:%d/qrcode/preorder/?IMSI=%s&serial_number=%d&total_fee=%d&subject=%s&order_time=%s", jfserver, portnumber, IMSI, serial_number, jftotal_fee, jfsubject, order_time);
	*len = sprintf(https,"http://%s:%d/qrcode/preorder/?%s&sign=%s",jfserver, portnumber, encrypt1wokey, hex_output);
	puts(https);
	memset(precr, 0, *len+1);
	memcpy(precr, https, *len);
	return *len;
}

/* single query */
int alipay_queryorder(char* precr, int* len, struct payInfo* order_info)
{
    
	char https[1024];
    
	char encrypt[1024];
	char encrypt1wokey[1024];
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;
	//*len = sprintf(encrypt,"IMSI=%s&serial_number=%d#%s", IMSI, serial_number, jfkey);
        /* TODO:using order_info->imsi may causing crash */
	*len = sprintf(encrypt,"IMSI=%s&serial_number=%d#%s", order_info->imsi, order_info->order_number, order_info->order_key);
	printf("\nMD5 input:encrypt=%s", encrypt);

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)encrypt, strlen(encrypt));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	printf("\nencrypt output:");
	puts(hex_output);

	//*len = sprintf(encrypt1wokey,"IMSI=%s&serial_number=%d", IMSI, serial_number);
	*len = sprintf(encrypt1wokey,"IMSI=%s&serial_number=%d", IMSI, order_info->order_number);
	//*len = sprintf(https,"http://192.168.1.104:8180/qrcode/preorder/?IMSI=123456789012345&serial_number=12&total_fee=0.01&subject=ccc&order_time=2014-08-0211:21:20");
	//*len = sprintf(https,"http://%s:%d/qrcode/preorder/?IMSI=%s&serial_number=%d&total_fee=%d&subject=%s&order_time=%s", jfserver, portnumber, IMSI, serial_number, jftotal_fee, jfsubject, order_time);
	*len = sprintf(https,"http://%s:%d/qrcode/queryorder/?%s&sign=%s",jfserver, portnumber, encrypt1wokey, hex_output);
	puts(https);
	memset(precr, 0, *len+1);
	memcpy(precr, https, *len);
	return *len;
}

/* multi query */
int alipay_query(char* precr, int* len, char* str_imsi)
{

        char https[1024];

        char encrypt[1024];
        char encrypt1wokey[1024];
        md5_state_t state;
        md5_byte_t digest[16];
        char hex_output[16*2 + 1];
        int di;
        if(time_mark[0]=='\0')
        *len = sprintf(encrypt,"IMSI=%s&time_mark=%s#%s", str_imsi, str_timemark, jfkey);
        else
        *len = sprintf(encrypt,"IMSI=%s&time_mark=%s#%s", str_imsi, time_mark, jfkey);
        printf("\nMD5 input:encrypt=%s", encrypt);

        md5_init(&state);
        md5_append(&state, (const md5_byte_t *)encrypt, strlen(encrypt));
        md5_finish(&state, digest);
        for (di = 0; di < 16; ++di)
            sprintf(hex_output + di * 2, "%02x", digest[di]);
        printf("\nencrypt output:");
        puts(hex_output);

        if(time_mark[0]=='\0')
        *len = sprintf(encrypt1wokey,"IMSI=%s&time_mark=%s", str_imsi, str_timemark);
        else
        *len = sprintf(encrypt1wokey,"IMSI=%s&time_mark=%s", str_imsi, time_mark);
        //*len = sprintf(https,"http://192.168.1.104:8180/qrcode/preorder/?IMSI=123456789012345&serial_number=12&total_fee=0.01&subject=ccc&order_time=2014-08-0211:21:20");
        //*len = sprintf(https,"http://%s:%d/qrcode/preorder/?IMSI=%s&serial_number=%d&total_fee=%d&subject=%s&order_time=%s", jfserver, portnumber, IMSI, serial_number, jftotal_fee, jfsubject, order_time);
        *len = sprintf(https,"http://%s:%d/qrcode/query/?%s&sign=%s",jfserver, portnumber, encrypt1wokey, hex_output);
        puts(https);
        memset(precr, 0, *len+1);
        memcpy(precr, https, *len);
        return *len;
}
