#ifndef QRGENERATOR_H
#define QRGENERATOR_H

/*int generator_qrcode_to_bmp(int out);*/
int generator_qrcode_to_bmp(int out, char* price);
void getSNoPre(char* pre); //in arm can't return unsigned long long, maybe because the arm ASM return r0; only can return int32;
int alipay_query_serialno(unsigned long long queryNo);
void getIMSIconfig();
#endif
