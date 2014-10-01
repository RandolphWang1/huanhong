/***************************************************************************
    结构、公用变量以及宏的定义
    Li,Hengbo
    2009-11-05
***************************************************************************/
#ifndef _PARAM_H_
#define _PARAM_H_


#define APP_VERSION				"Version 14.06.19"

#define SIZE_TEL_NO					20

#define SIZE_IP						15
#define SIZE_PORT					5

#define SIZE_TPDU					4

#define YY_MM_DD					1
#define YY_MM						2
#define MM_DD						3
#define YY_MM_DD_HH_MM				4



/*
 * 以太网配置
 */
typedef struct
{
	char szServerIP[SIZE_IP+1];							//以太网 主机IP地址
	char szServerPort[SIZE_PORT+1];						//以太网 主机PORT
	char szLocalIP[SIZE_IP+1];							//以太网 本地IP地址
	char szMask[SIZE_IP+1];								//以太网 子网掩码
	char szGateway[SIZE_IP+1];							//以太网 网关
}T_ETHERNET;

typedef struct
{
	char szTPDU[4+1];									//TPDU
	char szOutNo;										//外线号码前缀
	int delay_times;									//外线号码与电话号码之间爱的延时秒数(逗号个数)
	char szTelNo[SIZE_TEL_NO+1];						//电话号码
	BOOL bOut;											//拨外线开关
	UINT uiModemDataNum;								//通讯数据量

	char szGprsServerIP[SIZE_IP+1];						//GPRS IP地址
	char szGprsServerPort[SIZE_PORT+1];					//GPRS PORT
        char szGprsApn[20+1];
	char szGprsUser[32 + 1];
	char szGprsPwd[32 + 1];
	UINT uiGprsDataNum;									//通讯数据量

	T_ETHERNET tEthernet;								//以太网IP地址
	UINT uiEthDataNum;									//通讯数据量
        UINT uiWifiDataNum;                 //通讯数据量

	char szMemKey[6+1];									//MEM卡写卡密钥
	char szM1KeyA[12+1];								//M1卡读卡密钥
	char szM1KeyB[12+1];								//M1卡写卡密钥
	 T_WIFI  t_Wifi;
	BOOL  bIsDC;			// TRUE是德卡,FALSE是环宏非接
	BOOL  bAutoIP;
	char szPos_sn[9];
	BOOL bAutoWifi;
}T_TERM;


extern T_TERM gTerm;
void InitTerm();

/*
 *SMS短信配置
 */
typedef struct
{
	char SCA[16];			// 短消息服务中心号码(SMSC地址)
	char TP_PID;			// 用户信息协议标识(TP-PID)
	char TP_DCS;			// 用户信息编码方式(TP-DCS)
	UINT uiMax;
}T_SMS;

extern T_SMS gSMS;
void InitSMS();

#endif

