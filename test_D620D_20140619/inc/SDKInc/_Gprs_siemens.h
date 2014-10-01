#ifndef __GPRS_H_
#define __GPRS_H_




#define GPRS_LINK_STATUS_ON					1
#define GPRS_LINK_STATUS_CONNING			2
#define GPRS_LINK_STATUS_BREAK				3

/*
 * function	:	打开GPRS模块
 * return		:	模块句柄
 */
int GPRS_Open();
int GPRS_init();

void GPRS_SetNet1(char *user, char *pwd, char *apn, char *ip, char *port, int flag);
void GPRS_SetNet2(char *user, char *pwd, char *apn, char *ip, char *port, int flag);

/*
 * function	:	GPRS模块初始化
 * param1	:	模块句柄
 * param2	:	用户名(可以为NULL)
 * param3	:	密码(可以为NULL)
 * param4	:	APN(可以为NULL)
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int GPRS_ConfigNet1();
int GPRS_ConfigNet2();
int GPRS_ConfigHttp1();
int GPRS_ConfigHttp2();

int GPRS_GetOper(char *name);
int DistingOper(char * name);

int GPRS_SimReady();
int GPRSAttach();

/*
 * function	:	GPRS模块重置
 * param1	:	模块句柄
 * param2	:	用户名(可以为NULL)
 * param3	:	密码(可以为NULL)
 * param4	:	APN(可以为NULL)
 * return		:	成功:	OK
 *				失败:	ERROR
 *///已更改
int GPRS_Reset();

/*
 * function	:	连接服务器
 * param1	:	模块句柄
 * param2	:	服务器地址
 * param3	:	服务器端口
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int GPRS_ConnectNet1();
int GPRS_ConnectNet2();
int GPRS_ConnectHttp1();
int GPRS_ConnectHttp2();

/*
 * function	:	发送数据
 * param1	:	模块句柄
 * param2	:	发送数据缓冲区
 * param3	:	发送数据长度
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int GPRS_Send(unsigned char *pucData, int uiLen);
int GPRS_SendEx(unsigned char *pucData, int uiLen);

int GPRS_Send_Http(unsigned char *pucData, int uiLen);
/*
 * function	:	接收数据
 * param1	:	模块句柄
 * param2	:	接收数据缓冲区
 * param3	:	接收数据长度
 * param4	:	超时时间(单位:秒)
 * return		:	成功:	OK
 *				失败:	ERROR
 */
#ifdef _GPRS_RECV_OLD
int GPRS_Recv(char *pucData, int puiLen, ULONG ulTimeOut,int flag);
#else
int GPRS_Recv(char *pucData, int *puiLen, ULONG ulTimeOut,int flag);
#endif
int GPRS_Recv_Http(char *pucData, int *puiLen, ULONG ulTimeOut,int flag);

/*
 * function	:	关闭GPRS句柄
 * param1	:	模块句柄
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int GPRS_Close();

/*
 * function	:	获取模块信号值
 * param1	:	模块句柄
 * param2	:	信号值缓冲区
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int GetSignal(int *nSignal);

/*
 * function	:	搜索信号
 * param1	:	模块句柄
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int SearchSignal();

/*
 * function	:	显示信号
 * param1	:	显示位置x坐标
 * param2	:	显示位置y坐标
 * param3	:	信号值
 * return		:	成功:	OK
 *				失败:	ERROR
 */
void ShowSignal(int x, int y, int nSignal);


/*
 * function	:	打开串口
 * return		:	成功:	串口句柄
 *				失败:	ERROR
 */
int OpenUart();

/*
 * function		:	获取连接状态
 * return		:	当前连接状态
 */
int GPRS_GetLinkStatus();

int M35_power_on();



#endif


