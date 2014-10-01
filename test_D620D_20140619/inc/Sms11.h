#ifndef _SMS_H_
#define _SMS_H_

// 用户信息编码方式
#define GSM_7BIT		0
#define GSM_8BIT		4
#define GSM_UCS2		8

// 应答状态
#define GSM_WAIT		0		// 等待，不确定
#define GSM_OK			1		// OK
#define GSM_ERR			-1		// ERROR

#define CP_ACP          0      // default to ANSI code page


// 短消息参数结构，编码/解码共用
// 其中，字符串以'\0'结尾
typedef struct {
	char SCA[16];			// 短消息服务中心号码(SMSC地址)
	char TPA[16];			// 目标号码或回复号码(TP-DA或TP-RA)
	char TP_PID;			// 用户信息协议标识(TP-PID)
	char TP_DCS;			// 用户信息编码方式(TP-DCS)
	char TP_SCTS[16];		// 服务时间戳字符串(TP_SCTS), 接收时用到
	char TP_UD[160];		// 原始用户信息(编码前或解码后的TP-UD)
	short index;			// 短消息序号，在读取时用到
} SM_PARAM;

// 读取应答的缓冲区
typedef struct {
	int len;
	char data[16384];
} SM_BUFF;

int gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmInvertNumbers(const char* pSrc, char* pDst, int nSrcLength);
int gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength);
int gsmEncodePdu(const SM_PARAM* pSrc, char* pDst);
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst);

BOOL gsmInit();

int gsmSendMessage(SM_PARAM* pSrc);
int gsmReadMessageList();
int gsmDeleteMessage(int index);

int gsmGetResponse(SM_BUFF* pBuff);

int gsmParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff);

char GSM_RecString_Comp(char ans[], int length);

BOOL Init_Gprs();				//初始化GPRS功能
BOOL Open_GPGS_Mode();			//打开GPRS模式
BOOL Close_GPGS_Mode();			//关闭GPRS模式
BOOL Read_GPRS_Data();			//读取GPRS数据中心的数据
BOOL Write_GPRS_Data(char a[],int length);		//向数据中心写数据

BOOL AT_Command(char *cmd, char length, char code, int time);
int ReadComm(void* pData, int nLength);
int WriteComm(void* pData, int nLength);
int MultiByteToWideChar(const unsigned short * lpMultiByteStr,int cchMultiByte,unsigned short * lpWideCharStr);
int WideCharToMultiByte(const unsigned short *  lpWideCharStr,int  cchWideChar,LPSTR  lpMultiByteStr);

#endif

