#ifndef QRGENERATOR_H
#define QRGENERATOR_H

/*int generator_qrcode_to_bmp(int out);*/
int generator_qrcode_to_bmp(int out, char* price);
unsigned long long getSNoPre(char* pre);
int alipay_query_serialno(unsigned long long queryNo);
void getIMSIconfig();
#endif
