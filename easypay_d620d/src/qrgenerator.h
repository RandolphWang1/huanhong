#ifndef QRGENERATOR_H
#define QRGENERATOR_H

/*int generator_qrcode_to_bmp(int out);*/
int generator_qrcode_to_bmp(int out, char* price);
int readBmp(char *bmpName,int *bmpWidth, int *bmpHeight);
#endif
