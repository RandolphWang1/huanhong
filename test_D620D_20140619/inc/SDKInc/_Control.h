#ifndef __CONTROL_H_
#define __CONTROL_H_




/*
 * function	:	初始化控制驱动
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	驱动打开错误
 				-2	-	纽扣电池异常，提示用户更换电池
 				-3	-	其它错误
 */
int InitControl();

#define POWER_MODE_DC			0	//仅使用外接电源。
#define POWER_MODE_BAT			1	//仅使用电池。
#define POWER_MODE_CHARGE		2	//外接电源和电池同时使用，充电中
#define POWER_MODE_COMPLETE		3	//外接电源和电池同时使用，充电完毕
/*
 * function	:	获取供电模式
 * param1	:	
 * return	:	0	-	仅使用外接电源。
 				1	-	仅使用电池。
 				2	-	外接电源和电池同时使用，充电中
 				3	-	外接电源和电池同时使用，充电完毕
 				其它-	执行错误
 */
int GetPowerMode();

#define POWER_STATUS_RELEASE_0		0	//放电状态，0级电量，紧急状态，建议立即备份，给出提示，强制关机。
#define POWER_STATUS_RELEASE_1		1	//放电状态，1级电量，电池剩余不到20%电量，需要尽快充电。
#define POWER_STATUS_RELEASE_2		2	//放电状态，2级电量
#define POWER_STATUS_RELEASE_3		3	//放电状态，3级电量
#define POWER_STATUS_RELEASE_4		4	//放电状态，4级电量
#define POWER_STATUS_RELEASE_5		5	//放电状态，5级电量，电池剩余电量>80%

#define POWER_STATUS_CHARGE_1		6	//充电状态，充入电量小于20%
#define POWER_STATUS_CHARGE_2		7	//充电状态，充入电量20%~40%
#define POWER_STATUS_CHARGE_3		8	//充电状态，充入电量40%~60%
#define POWER_STATUS_CHARGE_4		9	//充电状态，充入电量60%~80%
#define POWER_STATUS_CHARGE_5		10	//充电至90%以上

#define POWER_STATUS_DC				11	//仅交流适配器供电
/*
 * function	:	获取电池状态
 * param1	:	
 * return	:	0	-	放电状态，0级电量，紧急状态，建议立即备份，给出提示，强制关机。
 				1	-	放电状态，1级电量，电池剩余不到20%电量，需要尽快充电。
 				2	-	放电状态，2级电量
 				3	-	放电状态，3级电量
 				4	-	放电状态，4级电量
 				5	-	放电状态，5级电量，电池剩余电量>80%
 				6	-	充电状态，充入电量小于20%
 				7	-	充电状态，充入电量20%~40%
 				8	-	充电状态，充入电量40%~60%
 				9	-	充电状态，充入电量60%~80%
 				10	-	充电至90%以上
 				11	-	仅交流适配器供电
 				其它-	执行错误
 * info:		1. 连续监测时，建议每2秒或更长时间检测一次。
				2. 电池不同负载下，电压会不同。所以某些情况出现电量显示回落，是正常现象。

 */
int GetPowerStatus();

/*
 * function	:	有线modem电源控制
 * param1	:	执行标志位 0-关闭 1-开启
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int PowerModem(int value);

/*
 * function	:	切换串口通道
 * param1	:	切换串口2到有线modem通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_Modem();

/*
 * function	:	切换串口通道
 * param1	:	切换串口0到有线modem通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
// 今后不在使用
int SwitchCom0_Modem();

/*
 * function	:	切换串口通道
 * param1	:	切换串口0到MC323通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom0_MC323();

/*
 * function	:	切换串口通道
 * param1	:	切换串口0到MC55通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom0_MC55();

/*
 * function	:	切换串口通道
 * param1	:	切换串口0到Pinpad通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_Pinpad();// 不再使用
int SwitchCom0_Pinpad();

/*
 * function	:	切换串口通道
 * param1	:	切换串口2到scan通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_Scan();

/*
 * function	:	切换串口通道
 * param1	:	切换串口2到非接通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_Rf(int port);

/*
 * function	:	切换串口通道
 * param1	:	切换串口2到德卡非接通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_DC();

/*
 * function	:	切换串口通道
 * param1	:	切换串口2到环宏非接通道
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int SwitchCom2_HH();

/*
 * function	:	打开MC55电源
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int PowerOnMC55();

/*
 * function	:	关闭MC55电源
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int PowerOffMC55();

/*
 * function	:	重置MODEM
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int ModemReset();

/*
 * function	:	重置MC55
 * return	:	0	-	正确执行
 				其它-	执行错误
 */
int MC55Reset();

//
/**********************************************************
*函数：	se955_init
*说明：	
*输入： 
*返回： 
***********************************************************/ 
int se955_init();
int InitScan();// 建议使用

//
/**********************************************************
*函数：	se955_scan
*说明：	
*输入： se955_uart_handle:uart handle.
*       timeout:扫描超时,以0.1秒为单位,最大0.1秒*200=20秒.最小超时为0.5秒.
*返回： *recv_data
*       <=0:recv fail. 
*       >0:recv byte.
***********************************************************/ 
int se955_scan(char *recv_data,int timeout);
int StartScan(char *pszRecvData, int nTimeOut);//建议使用

//
/**********************************************************
*函数： SERIAL_Open
*说明:  open serial port.
*输入：	
*NOTE:  
*返回： 
***********************************************************/
int SERIAL_Open(char *port,int speed);

//
/**********************************************************
*函数： SERIAL_Read
*说明:  
*输入：	File Handle             int iHandle
        Output Data Buffer      unsigned char *recv_data
        Output Data Buffer Len  unsigned short recv_len
*NOTE:  
*返回： 
***********************************************************/
int SERIAL_Read(int iHandle,unsigned char *recv_data,unsigned short recv_len);

//
/**********************************************************
*函数： SERIAL_Write
*说明:  
*输入：	
*NOTE:  
*返回： 
***********************************************************/
int SERIAL_Write(int iHandle,unsigned char *send_data,unsigned short send_len);

//
/******************************************************
函数：  SERIAL_Write
说明：
输入：  File Handle             int iHandle 
返回： 
dairy: 
Feb25,2011-MK modified.
*******************************************************/
int SERIAL_Close(int iHandle);
int M35_power_on();

#endif
