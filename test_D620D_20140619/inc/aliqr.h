#ifndef ALIQR_H 
#define ALIQR_H 
/* pay information struct (must) */
struct payInfo {
#if 1
    char imsi[16+1];
    char order_key[32+1];
    unsigned long long  order_number;
    char order_time[19+1];
    char order_subject[128+1];
    char total_fee[15+1];
#endif
#if 0
    char *imsi;
    char *order_key;
    int  order_number;
    char *order_time;
    char *order_subject;
    char *total_fee;
#endif
}; 
#define QRRESULTSTR 102400
struct qr_result {
    char qr_string[QRRESULTSTR];
    char time_mark[32];
    char out_trade_no[65];
};

#define QRRESULT sizeof(struct qr_result)

struct receipt_info {
    char serial_number[24];
    char out_trade_no[16];
    char trade_no[32];
    char trade_status[16];
    char total_fee[16];
};

/* single query parameters for multi payment results */
struct queryInfo {
    char imsi[16+1];
    char timemark[32];
};
//int alipay_precreate(char* precr, int* len, struct payInfo* order_info);
enum precreate_type {
    ALI_PRECREATE_ORDER = 0, /* require an online order qrcode from alipay */
    ALI_PRECREATE_QUERY = 1, /* query the payment status of order with timemask */
    ALI_PRECREATE_QUERY_SINGLE = 2, /* query the with serial no */
    ALI_PRECREATE_QUERY_24H = 3, /* query the recent 24h */
};
int alipay_precreate(char* precr, int* len, struct payInfo* order_info, int type);
int alipay_preorder(char* precr, int* len, struct payInfo* order_info);
int alipay_queryorder(char* precr, int* len, struct payInfo* order_info);
int alipay_query(char* precr, int* len, char* str_imsi);
int alipay_main(struct qr_result *query_result, struct payInfo* order_info, int order_type);
int alipay_query24h(char* precr, int* len, char* str_imsi);
#endif
