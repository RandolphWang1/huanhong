#ifndef __TF_CARD_H_
#define __TF_CARD_H_



//清理串口数据
void ClearComChanel(int iHandle);

/*
 * function	:	打开读卡设备
 * return	:	>0	-	执行正常
 			<=0	-	执行错误
 */
 int FM1Open(int port);

/*
 * function	:	关闭读卡设备
 */
void FM1Close(int iHandle);

/*
 * function	:	寻卡
 * param3	:	超时时间
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FM1Detect(int iHandle, char *szID, ULONG ulTimeOut);

/*
 * function	:	验证读卡密钥
 * param1	:	块号
 * param2	:	密钥
 * param3	:	超时时间
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FM1AuthorReadKey(int iHandle, BYTE block_no, char *szID, char *key);

/*
 * function	:	读取M1卡
 * param1	:	读卡设备句柄
 * param2	:	块号
 * param3	:	读出内容-16字节
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FM1Read(int iHandle, BYTE block_no, char *out_buff);

/*
 * function	:	验证写卡密钥
 * param1	:	块号
 * param2	:	密钥
 * param3	:	超时时间
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FM1AuthorWriteKey(int iHandle, BYTE block_no, char *szID, char *key);

/*
 * function	:	写M1卡
 * param1	:	写卡设备句柄
 * param2	:	块号
 * param3	:	写入内容-16字节
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FM1Write(int iHandle, BYTE block_no, char *in_buff);

/*
 *port            :默认使用port=1;
 * function	:	测试非接触CPU卡
 * return	:	0	-	执行正常
 				<0	-	执行错误
 */
int FFCardCPUTest(int port);


#endif

