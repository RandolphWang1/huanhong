#include "./libiconv-1.14/include/iconv.h"
#include <stdio.h>
#include <string.h>

int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{

	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1)
		return -1;
	iconv_close(cd);
	return 0;

}

//UNICODE码转为GB2312码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-16","gb2312",inbuf,inlen,outbuf,outlen);
}
//GB2312码转为UNICODE码
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}
#if 0
int main(void)

{
    int i;
	unsigned char in_utf8[] ={0x65,0x36,0x51,0x65,0x00,0x31,0x00,0x30,0x00,0x30,0x51,0x43};
	unsigned char out[50]={'\0'};
	int rc;
	rc = u2g(in_utf8,sizeof(in_utf8),out,sizeof(in_utf8));
    printf("rc =%d\n",rc);
	printf("strlen in_utfi=%d\n",sizeof(in_utf8));
	for(i=0;i<sizeof(in_utf8);i++)
		printf("%x\n",out[i]);
}
#endif
