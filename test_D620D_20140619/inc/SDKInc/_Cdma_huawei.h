#ifndef __CDMA_H_
#define __CDMA_H_




#define CDMA_LINK_STATUS_ON					1
#define CDMA_LINK_STATUS_CONNING			2
#define CDMA_LINK_STATUS_BREAK				3

/*
 * function		:	打开CDMA模块
 * return		:	模块句柄
 */
int CDMA_Open();

/*
 * function		:	CDMA模块初始化
 * param1		:	模块句柄
 * param2		:	用户名(可以为NULL)
 * param3		:	密码(可以为NULL)
 * param4		:	APN(可以为NULL)
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Init(int iHandle, char *user, char *pwd);

/*
 * function		:	CDMA模块获取IP
 * param1		:	模块句柄
 * param2		:	用户名(可以为NULL)
 * param3		:	密码(可以为NULL)
 * param4		:	APN(可以为NULL)
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_DHCP(int iHandle, char *user, char *pwd);

/*
 * function		:	连接服务器
 * param1		:	模块句柄
 * param2		:	服务器地址
 * param3		:	服务器端口
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Connect(int iHandle, char *ip, char *port);

/*
 * function		:	连接服务器
 * param1		:	模块句柄
 * param2		:	服务器地址
 * param3		:	服务器端口
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_ConnectEx(int iHandle, char *ip, char *port);

/*
 * function		:	发送数据
 * param1		:	模块句柄
 * param2		:	发送数据缓冲区
 * param3		:	发送数据长度
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Send(int iHandle, unsigned char *pucData, int uiLen);

/*
 * function		:	接收数据
 * param1		:	模块句柄
 * param2		:	接收数据缓冲区
 * param3		:	接收数据长度
 * param4		:	超时时间(单位:秒)
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Recv(int iHandle, char *pucData, int *puiLen, ULONG ulTimeOut);

/*
 * function		:	关闭CDMA句柄
 * param1		:	模块句柄
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Close(int iHandle);

/*
 * function		:	重置CDMA模块
 * param1		:	模块句柄
 * return		:	成功:	OK
 *					失败:	ERROR
 */
int CDMA_Reset(int iHandle);

/*
 * function		:	获取连接状态
 * return			:	当前连接状态
 */
int CDMA_GetLinkStatus();



#endif

